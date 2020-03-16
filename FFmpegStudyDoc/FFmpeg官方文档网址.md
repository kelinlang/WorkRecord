## FFmpeg系统化学习

- 下载源码，编译一次，生成命令行工具、api头文件和动态库静态库
- 命令行测试，熟悉ffmpeg推拉流编解码相关命令,官网有详细的命令使用方法和示例
- 阅读ffmpeg命令行工具源码及ffmpeg 源码，了解主要的数据结构，熟悉ffmpeg 如何使用api开发项目
- FFmpeg官网已经非常成熟，关于编解码、封装格式转换、音视频处理支持的参数有系统详细的介绍，平常使用过程中需要了解都可以上官网查找，结合官网资料介绍，可以查看对应源码
- 官网是英文的，使用google浏览器打开可以右击翻译成中文，有些翻译不太准的地方可以对照英文看看理解



## FFmpeg官方资料

- 源码仓库地址：https://git.ffmpeg.org/ffmpeg.git

- 官网地址：https://www.ffmpeg.org/

- 编译的各平台FFmpeg命令行工具下载地址：https://ffmpeg.zeranoe.com/builds/

  - 下载后执行命令行：ffmpeg -v可以看到编译之前配置的**编译参数**

- **命令行工具文档** ：https://www.ffmpeg.org/documentation.html
  
  - **ffmpeg**：https://www.ffmpeg.org/ffmpeg.html
  
    - 目录
  
      - 简介

      - 说明
  
      - 详细说明
  
        - 过滤
          - 简单滤镜
          - 复杂滤镜
        - 流复制
  
      - 流选择
  
        - 说明
          - 自动流选择
          - 手动流选择
          - 复杂的滤镜
          - 流处理
        - 范例
  
      - ## [选择](https://www.ffmpeg.org/ffmpeg.html#toc-Options)
  
        - ### [流说明符](https://www.ffmpeg.org/ffmpeg.html#toc-Stream-specifiers-1)
  
        - ### [通用选项](https://www.ffmpeg.org/ffmpeg.html#toc-Generic-options)
  
        - ### [AVOptions](https://www.ffmpeg.org/ffmpeg.html#toc-AVOptions)
  
        - ### [主要选项](https://www.ffmpeg.org/ffmpeg.html#toc-Main-options)
  
        - ### [视频选项](https://www.ffmpeg.org/ffmpeg.html#toc-Video-Options)
  
        - ### [高级视频选项](https://www.ffmpeg.org/ffmpeg.html#toc-Advanced-Video-options)
  
        - ### [音频选项](https://www.ffmpeg.org/ffmpeg.html#toc-Audio-Options)
  
        - ### [高级音频选项](https://www.ffmpeg.org/ffmpeg.html#toc-Advanced-Audio-options)
  
        - ### [字幕选项](https://www.ffmpeg.org/ffmpeg.html#toc-Subtitle-options)
  
        - ### [高级字幕选项](https://www.ffmpeg.org/ffmpeg.html#toc-Advanced-Subtitle-options)
  
        - ### [高级选项](https://www.ffmpeg.org/ffmpeg.html#toc-Advanced-options)
  
  - ffmpeg-all:https://www.ffmpeg.org/ffmpeg-all.html
  
  - ffplay：https://www.ffmpeg.org/ffplay.html
  
  - ffplay-all：https://www.ffmpeg.org/ffplay-all.html
  
  - ffprobe：https://www.ffmpeg.org/ffprobe.html
  
  - ffprobe-all：https://www.ffmpeg.org/ffprobe-all.html
  
- 组件文档
  - libavutil 库工具：https://www.ffmpeg.org/ffmpeg-utils.html
  
    介绍了libavutil库提供的一些常规功能和实用程序。
  
  - 视频缩放和像素格式转换：https://www.ffmpeg.org/ffmpeg-scaler.html
  
    FFmpeg重缩放器为libswscale库图像转换实用程序提供了高级接口。特别是它允许执行图像缩放和像素格式转换。
  
  - 音频重采样：https://www.ffmpeg.org/ffmpeg-resampler.html
  
    FFmpeg重采样器为libswresample库音频重采样实用程序提供了一个高级接口。特别是，它允许执行音频重采样，音频通道布局重新矩阵化以及转换音频格式和打包布局。
  
  - 编解码：https://www.ffmpeg.org/ffmpeg-codecs.html
  
    介绍了libavcodec库提供的编解码器（解码器和编码器）。
  
  - 比特流过滤器：https://www.ffmpeg.org/ffmpeg-bitstream-filters.html
  
    介绍了libavcodec库提供的位流过滤器。
  
    比特流滤波器对编码的流数据进行操作，并且在不执行解码的情况下执行比特流级别的修改。
  
  - 封装格式：https://www.ffmpeg.org/ffmpeg-formats.html
  
    介绍了libavformat库提供的受支持的格式（混合器和解复用器）。
  
  - 网络协议：https://www.ffmpeg.org/ffmpeg-protocols.html
  
    档描述了libavformat库提供的输入和输出协议。
  
  - 输入输出设备文档：https://www.ffmpeg.org/ffmpeg-devices.html
  
    文档描述了libavdevice库提供的输入和输出设备。
  
  - 滤镜：https://www.ffmpeg.org/ffmpeg-filters.html
  
    文档描述了libavfilter库提供的过滤器，源和接收器。

