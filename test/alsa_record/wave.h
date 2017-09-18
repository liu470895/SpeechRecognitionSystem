#ifndef __WAVE_H
#define __WAVE_H

#include <stdint.h>
#include <alsa/asoundlib.h>

struct wave_header
{
	uint32_t id;
	uint32_t size;
	uint32_t format;
};

struct wave_fmt
{
	uint32_t fmt_id; //fmt区块标记
	uint32_t fmt_size; //包含fmt区块标记以及wave_header的大小，不包含自己的大小
	uint16_t fmt; //模拟信号转数字信号方式，0x0001是PCM
	uint16_t channels; //音轨数量
	uint32_t sample_rate; //采样率
	uint32_t byte_rate; //码率=采样率 × 帧大小
	uint16_t block_align; //帧大小=音轨数量 × AD转换分辨率/8，也就是每次采样的字节数
	uint16_t bits_per_sample; //AD转换分辨率，也就是量化位数
};

struct wave_data
{
	uint32_t data_id; //data区块标记
	uint32_t data_size; //数据块的大小
};

typedef struct
{
	struct wave_header head;
	struct wave_fmt format;
	struct wave_data data;

}wav_format;

typedef struct 
{
	snd_pcm_t *handle;
	snd_pcm_format_t format;
	uint16_t channels;
	size_t bits_per_sample; //AD转换分辨率，8,16,...
	size_t bytes_per_frame; //一帧的字节数

	snd_pcm_uframes_t frames_per_period; //一个周期的帧个数
	snd_pcm_uframes_t frames_per_buffer; //系统缓存区帧个数

	uint8_t *period_buf; //用来存放从wav文件读取的最多一个周期的数据
}pcm_container;

#endif
