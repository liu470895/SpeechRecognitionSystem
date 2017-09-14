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

    ​
