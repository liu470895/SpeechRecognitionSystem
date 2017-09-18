#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <byteswap.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include "wave.h"

//判断系统是小端序还是大端序
#if __BYTE_ORDER == __LITTLE_ENDIAN
	
	#define RIFF ('F'<<24 | 'F'<<16 | 'I'<<8 | 'R'<<0)
	#define WAVE ('E'<<24 | 'V'<<16 | 'A'<<8 | 'W'<<0)
	#define FMT  (' '<<24 | 't'<<16 | 'm'<<8 | 'f'<<0)
	#define DATA ('a'<<24 | 't'<<16 | 'a'<<8 | 'd'<<0)

	#define LE_SHORT(val)	(val)
	#define LE_INT(val)		(val)

#elif __BYTE_ORDER == __BIG_ENDIAN

	#define RIFF ('R'<<24 | 'I'<<16 | 'F'<<8 | 'F'<<0)
	#define WAVE ('W'<<24 | 'A'<<16 | 'V'<<8 | 'E'<<0)
	#define FMT  ('f'<<24 | 'm'<<16 | 't'<<8 | ' '<<0)
	#define DATA ('d'<<24 | 'a'<<16 | 't'<<8 | 'a'<<0)

	#define LE_SHORT(val)	bswap_16(val)
	#define LE_INT(val)		bswap_32(val)

#endif

wav_format *wav = NULL;
int wfd;

void usage(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "usage:%s <wav_file>\n", argv[0]);
		exit(1);
	}
}

void prepare_wav_params(wav_format *wav)
{
	/*header区块*/
	wav->head.id = RIFF;
	wav->head.size = LE_INT(0); //文件大小暂时定为0,待文件全部写完再填写该值
	wav->head.format = WAVE;
	/*fmt区块*/
	wav->format.fmt_id = FMT;
	wav->format.fmt_size = LE_INT(16); 
	wav->format.fmt = LE_INT(1); //pcm
	wav->format.channels = LE_SHORT(2); //音轨数量2
	wav->format.sample_rate = LE_INT(44100); //采样率
	wav->format.bits_per_sample = LE_SHORT(16); //AD转换分辨率，量化位数
	wav->format.block_align = LE_SHORT(wav->format.channels * wav->format.bits_per_sample/8); //帧大小
	wav->format.byte_rate = LE_INT(wav->format.sample_rate * wav->format.block_align); //码率
	/*data区块*/
	wav->data.data_id = DATA;
	wav->data.data_size = LE_INT(0); //数据区块大小暂定为0,等全部数据写完再填写该值
	
}

void set_wav_params(pcm_container *sound, wav_format *wav)
{
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_hw_params_alloca(&hwparams);

	snd_pcm_hw_params_any(sound->handle, hwparams);//初始化硬件参数空间

	//设置访问模式为交错模式（即帧连续模式）
	snd_pcm_hw_params_set_access(sound->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	
	//设置pcm的AD分辨率转换参数
	snd_pcm_format_t pcm_format = SND_PCM_FORMAT_S16_LE;
	snd_pcm_hw_params_set_format(sound->handle, hwparams, pcm_format);
	
	sound->format = pcm_format;

	//设置pcm采集的音轨数目
	snd_pcm_hw_params_set_channels(sound->handle, hwparams, LE_SHORT(wav->format.channels));
	sound->channels = LE_SHORT(wav->format.channels);

	//设置采用频率，最终值存放到exact_rate
	uint32_t exact_rate = LE_SHORT(wav->format.sample_rate);
	snd_pcm_hw_params_set_rate_near(sound->handle, hwparams, &exact_rate, 0);


	//设置pcm系统缓冲区的大小
	snd_pcm_uframes_t buffer_size;
	snd_pcm_hw_params_get_buffer_size_max(hwparams, &buffer_size);
	snd_pcm_hw_params_set_buffer_size_near(sound->handle, hwparams, &buffer_size);

	//设置pcm设备的缓存周期
	snd_pcm_uframes_t period_size = buffer_size / 4;
	snd_pcm_hw_params_set_period_size_near(sound->handle, hwparams, &period_size, 0);

	//应用设置参数到pcm设备
	snd_pcm_hw_params(sound->handle, hwparams);

	//设置生效后，取具体的buffer_size和period_size，这里以帧为单位，帧 = 声道数×(量化位数/8)
	snd_pcm_hw_params_get_buffer_size(hwparams, &sound->frames_per_buffer);
	snd_pcm_hw_params_get_period_size(hwparams, &sound->frames_per_period, 0);

	//保存参数到pcm_container中
	sound->bits_per_sample = snd_pcm_format_physical_width(pcm_format); //量化位数
	sound->bytes_per_frame = sound->bits_per_sample/8 * wav->format.channels;

	//保存每个周期最大缓存的字节数，以bytes为单位
	sound->period_buf = (uint8_t *)calloc(1, sound->frames_per_period * sound->bytes_per_frame);
		
}



snd_pcm_uframes_t read_pcm_data(pcm_container *sound, snd_pcm_uframes_t frames)
{
	snd_pcm_uframes_t exact_frames = 0;
	snd_pcm_uframes_t n = 0;

	uint8_t *p = sound->period_buf;
	while(frames > 0)	
	{
		n = snd_pcm_readi(sound->handle, p, frames);

		frames -= n;
		exact_frames += n;
		p += (n * sound->bytes_per_frame);
	}

	return exact_frames;
}

uint32_t total_bytes = 0;

void recorder(int fd, pcm_container *sound)
{
	lseek(fd, sizeof(wav_format), SEEK_SET);

	uint32_t nwrite = 0;

	while(1)
	{
		snd_pcm_uframes_t n = sound->frames_per_period;

		uint32_t frames_read = read_pcm_data(sound, n);
		nwrite = write(fd, sound->period_buf, frames_read * sound->bytes_per_frame);
		total_bytes += nwrite;

		uint32_t total_frame = total_bytes / (sound->bytes_per_frame);
		printf("%d\n", total_frame);
	}
}

void stop(int sig)
{
	lseek(wfd, 0, SEEK_SET);
	
	wav->data.data_size = LE_INT(total_bytes);
	wav->head.size = LE_INT(36 + wav->data.data_size);

	write(wfd, &wav->head, sizeof(wav->head));
	write(wfd, &wav->format, sizeof(wav->format));
	write(wfd, &wav->data, sizeof(wav->data));


	printf("recorder over:%dbytes\n", total_bytes);
	exit(1);


}

int main(int argc, char *argv[])
{
	usage(argc, argv);

	signal(SIGINT, stop);

	wfd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if(wfd == -1)
	{
		fprintf(stderr, "%s open() faild\n", argv[1]);
		exit(1);
	}


	pcm_container *sound = calloc(1, sizeof(pcm_container));
	int err = snd_pcm_open(&sound->handle, "default", SND_PCM_STREAM_CAPTURE, 0);
	
	if(err != 0)
	{
		printf("[%d]: %d\n", __LINE__, err);
		perror("snd_pcm_open( ) failed");
		exit(1);
	}

	wav = calloc(1, sizeof(wav_format));
	prepare_wav_params(wav); //wav参数初始化

	//将wav的参数设置到pcm硬件设备的参数空间
	set_wav_params(sound, wav); 

	recorder(wfd, sound);

	//释放相关资源
	snd_pcm_drain(sound->handle);
	snd_pcm_close(sound->handle);
	close(wfd);

	free(sound->period_buf);
	free(sound);
	free(wav);
	return 0;
}
