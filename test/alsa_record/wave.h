#ifndef __WAVE_H
#define __WAVE_H

#include <stdint.h>

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


#endif
