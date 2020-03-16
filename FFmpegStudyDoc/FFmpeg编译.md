# FFmpeg编译

官方编译手册地址：

https://trac.ffmpeg.org/wiki/CompilationGuide



## Windows版本编译

- 配置

  ./configure --enable-ffplay --enable-gpl --enable-libx264  --enable-libx265  --enable-static --enable-shared  --enable-debug  --disable-x86asm  --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac  --enable-libsrt --enable-nonfree



## Linux x86/64 环境下编译

### 参考文档

《[Ubuntu / Debian / Mint FFmpeg编译手册](https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu)》

### 编译环境

- 操作系统

Ubuntu 18.04.4 x86_64（Windows 10 1909  WSL）

- 硬件配置

| 配件   | 型号                                   |
| ------ | -------------------------------------- |
| 处理器 | 英特尔（Intel）酷睿 i5-9400F           |
| 主板   | 微星（MSI）B360M MORTAR                |
| 内存   | 科赋（KLEVV）BOLT X DDR4 16Gx2 3200Mhz |
| 显卡   | 华硕（ASUS） 雪豹 DUAL-RX580-O8G       |

### 编译过程

1. 安装编译所需开发工具

```
apt-get update -qq && sudo apt-get -y install \
  autoconf \
  automake \
  build-essential \
  cmake \
  git-core \
  libass-dev \
  libfreetype6-dev \
  libsdl2-dev \
  libtool \
  libva-dev \
  libvdpau-dev \
  libvorbis-dev \
  libxcb1-dev \
  libxcb-shm0-dev \
  libxcb-xfixes0-dev \
  pkg-config \
  texinfo \
  wget \
  zlib1g-dev
```

2. 创建源代码和输出程序目录

```
mkdir -p ~/ffmpeg_sources ~/bin
```

3. 编译/安装相关依赖库

```shell
#NASM An assembler used by some libraries
apt-get install nasm

#Yasm An assembler used by some libraries
apt-get install yasm

#libx264 H.264 video encoder
apt-get install libx264-dev

#libx265 H.265/HEVC video encoder
apt-get install libx265-dev libnuma-dev

#libvpx VP8/VP9 video encoder/decoder
apt-get install libvpx-dev

#libfdk-aac AAC audio encoder
apt-get install libfdk-aac-dev

#libmp3lame MP3 audio encoder
apt-get install libmp3lame-dev

#libopus Opus audio decoder and encoder
apt-get install libopus-dev

#libaom AV1 video encoder/decoder
cd ~/ffmpeg_sources && \
git -C aom pull 2> /dev/null || git clone --depth 1 https://aomedia.googlesource.com/aom && \
mkdir -p aom_build && \
cd aom_build && \
PATH="$HOME/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$HOME/ffmpeg_build" -DENABLE_SHARED=off -DENABLE_NASM=on ../aom && \
PATH="$HOME/bin:$PATH" make && \
make install
```

4. 下载/配置/编译FFmpeg

```shell
#下载最新版本源码
cd ~/ffmpeg_sources && \
wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2 && \
tar xjvf ffmpeg-snapshot.tar.bz2

#配置
cd ffmpeg && \
PATH="$HOME/bin:$PATH" PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig" ./configure \
  --prefix="$HOME/ffmpeg_build" \
  --pkg-config-flags="--static" \
  --extra-cflags="-I$HOME/ffmpeg_build/include" \
  --extra-ldflags="-L$HOME/ffmpeg_build/lib" \
  --extra-libs="-lpthread -lm" \
  --bindir="$HOME/bin" \
  --enable-gpl \
  --enable-libaom \
  --enable-libass \
  --enable-libfdk-aac \
  --enable-libfreetype \
  --enable-libmp3lame \
  --enable-libopus \
  --enable-libvorbis \
  --enable-libvpx \
  --enable-libx264 \
  --enable-libx265 \
  --enable-nonfree 

#编译安装
PATH="$HOME/bin:$PATH" make -j6 && \
make install && \
hash -r
```


5. 查看FFmpeg版本 

源码下载编译时间：2020-02-22

```shell
~/bin/FFmpeg -version
ffmpeg version N-96792-g553d836 Copyright (c) 2000-2020 the FFmpeg developers
built with gcc 7 (Ubuntu 7.4.0-1ubuntu1~18.04.1)
configuration: --prefix=/root/ffmpeg_build --pkg-config-flags=--static --extra-cflags=-I/root/ffmpeg_build/include --extra-ldflags=-L/root/ffmpeg_build/lib --extra-libs='-lpthread -lm' --bindir=/root/bin --enable-gpl --enable-libaom --enable-libass --enable-libfdk-aac --enable-libfreetype --enable-libmp3lame --enable-libopus --enable-libvorbis --enable-libvpx --enable-libx264 --enable-libx265 --enable-nonfree
libavutil      56. 41.100 / 56. 41.100
libavcodec     58. 72.100 / 58. 72.100
libavformat    58. 39.100 / 58. 39.100
libavdevice    58.  9.103 / 58.  9.103
libavfilter     7. 76.100 /  7. 76.100
libswscale      5.  6.100 /  5.  6.100
libswresample   3.  6.100 /  3.  6.100
libpostproc    55.  6.100 / 55.  6.100
```



## 配置参数说明

查看配置编译参数：

```
./configure --help
```

部分参数说明：

https://blog.csdn.net/momo0853/article/details/78043903