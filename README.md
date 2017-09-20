# 语音识别系统

一个简单的Linux系统上语音识别系统。

## 开发平台：

简单介绍开发环境的搭配以及一些依赖库安装。

### 软件平台

* 主机平台

  程序开发的系统平台，交叉编译工具链生成目标平台的可执行文件。

  ![ubuntu_16.04](./image/ubuntu_version.png) 

* 目标平台

  交叉编译生成的可执行文件运行的系统平台。

  ![gec6818_linux_version](./image/gec6818_linux_version.png) 

* 交叉编译工具链

  arm-linux-guneabi：

  ``` shell
  #ubuntu16.04
  sudo apt install arm-linux-gnueabi
  ```

  ![arm-linux-gnueabi-gcc_version](./image/arm-linux-gnueabi-gcc_version.png) 

* 音频接口(麦克风、扬声器等)操作库

  alsa-lib：1.1.4.1，alsa-utils：1.1.4.1；

  官网：[Advanced Linux Sound Architecture (ALSA) project](https://www.alsa-project.org/main/index.php/Main_Page) 

  API接口：[ALSA project - the C library reference](http://www.alsa-project.org/alsa-doc/alsa-lib/) 

  * alsa-lib

    安装该音频库，GEC6818上的Linux系统才能驱动音频接口，达到录音和播放声音的目的，因为是在开发板上运行，所以需要在主机ubuntu上交叉编译好库，再把库文件传送到开发板上，一方面在编写程序时交叉编译也需要用到这些库文件和头文件。

    ```  shell
    #下载和解压alsa-lib
    wget ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.4.1.tar.bz2
    tar xvfj alsa-lib-1.1.4.1.tar.bz2
    cd alsa-lib-1.1.4.1
    #交叉编译安装alsa-lib
    ```

    ​


  * alsa-utils

    交叉编译使用alsa库写的一些基础工具，由官方提供。利用这些小工具可以做录音，播放声音等操作，用来测试开发板平台上的音频设备(声卡、麦克风、扬声器)工作是否正常，因程序是在目标平台上运行，所以需要交叉编译生成目标平台的可执行文件。

    ``` shell
    #下载和解压alsa-utils
    wget ftp://ftp.alsa-project.org/pub/utils/alsa-utils-1.1.4.tar.bz2
    tar xvfj alsa-utils-1.1.4.tar.bz2
    cd alsa-utils-1.1.4
    #交叉编译安装alsa-utils
    ```

## 系统模块

这里简单说明下整个系统在实现的过程中划分成的几个模块，这些模块都可单独的实现，而整个系统基于这些模块来组建，进而达到想要实现的功能。可以将每个模块看成一个黑盒子，只有输入和输出。

### gec6818平台录音

录音模块是在gec6818上的Linux系统上运行的，目的是采集外部音频的输入，进而生成一个wav格式的音频文件。因此对于这个模块来说，输入是外部的声音信息，输出一个wav格式的音频文件。

### socket传输文件

对于录音模块输出的音频文件，需要将其传输到主机平台上。因此，利用网络来实现文件的传输。对于文件传输模块来说，输入的是文件，输出的也是文件。

### 语音识别

因免费的语音识别库目前只能在x86平台运行，因此只能通过网络将设备采集到的音频文件传输到主机上，由主机来完成音频文件的解析，语音识别模块的输入的是wav格式的音频文件，输出的xml格式的数据，从而将其写成一个xml文件输出。

### xml格式文本解析

对于语音识别模块输出的结果是一个xml文本格式的文件，因此需要对xml文本格式的数据进行解析，对于xml解析模块的输入是一个xml文本格式的文件，进而输出一堆需要的数据，至于哪些数据，就看程序的定义了。

## 附录A：知识点

记录项目需要用到的关键知识点。

### 音频参数概念

### 音频设备接口

alsa-lib

* playback
* capture

### 音频文件格式

1. WAVE格式

   * WAVE[^1]


   * RIFF与IFF[^2]

     一种档案格式标准，两者的区别在于数据的存放方式，大端序或小端序存放data。`RIFF`采用小端存放`data`，`IFF`采用大端存放`data`。 

   * 文件头信息header[^2][^3]

2. 生成wave格式文件[^3] 

### Linux查看alsa支持的pcm设备号[^4]

``` shell
[root@GEC6818 ~]#cat /proc/asound/pcm
00-00: alc5623 HiFi alc5621-hifi-0 :  : playback 1 : capture 1
01-00: SPDIF PCM Playback dit-hifi-0 :  : playback 1
```

`00-00`前面的`00`是声卡设备号，后面的`00`是声卡设备下的设备号。

使用以下语句可查看设备的声卡信息：

``` shell
 [root@GEC6818 ~]#cat /proc/asound/cards
 0 [I2Salc5623     ]: I2S-alc5623 - I2S-alc5623
                      I2S-alc5623
 1 [SPDIFTranscieve]: SPDIF-Transciev - SPDIF-Transciever
                      SPDIF-Transciever
```

临时参考链接：

1. ALSA wiki. Proc asound documentation[DB/OL]. http://alsa.opensrc.org/Proc_asound_documentation

###参考链接 

1. 维基百科. WAV\[DB/OL\]. https://zh.wikipedia.org/wiki/WAV
2. 维基百科. 资源交换档案格式[DB/OL]. https://zh.wikipedia.org/wiki/%E8%B3%87%E6%BA%90%E4%BA%A4%E6%8F%9B%E6%AA%94%E6%A1%88%E6%A0%BC%E5%BC%8F
3. Digital Audio - Creating a WAV (RIFF) file[DB/OL]. http://www.topherlee.com/software/pcm-tut-wavformat.html
4. markbuntu. How do I find out an ALSA device's code name/description thing?[DB/OL]. https://ubuntuforums.org/showthread.php?t=1288484&p=8090087#post8090087
