#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <byteswap.h>
#include <alsa/asoundlib.h>
#include "wave.h"

//判断系统是小端序还是大端序
#if __BYTE_ORDER == __LITTLE_ENDIAN
	
	#define RIFF ('F'<<24 | 'F'<<16 | 'I'<<8 | 'R'<<0)
	#define WAVE ('E'<<24 | 'V'<<16 | 'A'<<8 | 'W'<<0)
	#define FMT  (' '<<24 | 't'<<16 | 'm'<<8 | 'f'<<0)
	#define DATA ('a'<<24 | 't'<<16 | 'a'<<8 | 'd'<<0)

	#define LE_SHROT(val)	(val)
	#define LE_INT(val)		(val)

#elif __BYTE_ORDER == __BIG_ENDIAN

	#define RIFF ('R'<<24 | 'I'<<16 | 'F'<<8 | 'F'<<0)
	#define WAVE ('W'<<24 | 'A'<<16 | 'V'<<8 | 'E'<<0)
	#define FMT  ('f'<<24 | 'm'<<16 | 't'<<8 | ' '<<0)
	#define DATA ('d'<<24 | 'a'<<16 | 't'<<8 | 'a'<<0)

	#define LE_SHROT(val)	bswap_16(val)
	#define LE_INT(val)		bswap_32(val)

#endif

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
	wav->format.channels = LE_SHROT(2); //音轨数量2
	wav->format.sample_rate = LE_INT(44100); //采样率
	wav->format.bits_per_sample = LE_SHORT(16); //AD转换分辨率，量化位数
	wav->format.block_align = LE_SHORT(wav->format.channels * wav->format.bits_per_sample/8); //帧大小
	wav->format.byte_rate = LE_INT(wav->format.sample_rate * wav->format.block_align); //码率
	/*data区块*/
	wav->data.data_id = DATA;
	wav->data.data_size = LE_INT(0); //数据区块大小暂定为0,等全部数据写完再填写该值
	
}
int main(int argc, char *argv[])
{
	usage(argc, argv);

	int wfd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if(wfd == -1)
	{
		fprintf(stderr, "%s open() faild\n", argv[1]);
		exit(1);
	}

	wav_format *wav = calloc(1, sizeof(wav_format));
	
	
	return 0;
}
