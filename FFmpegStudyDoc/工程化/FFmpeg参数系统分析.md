## FFmpeg 参数系统分析

- **ffmpeg -v 查看信息**

ffmpeg version N-96061-ga23d781a5a Copyright (c) 2000-2019 the FFmpeg developers
  built with gcc 7 (Ubuntu 7.5.0-3ubuntu1~18.04)
  configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-nonfree
  WARNING: library configuration mismatch
  avutil      configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-libsrt --enable-nonfree
  avcodec     configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-libsrt --enable-nonfree
  avformat    configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-libsrt --enable-nonfree
  avdevice    configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-libsrt --enable-nonfree
  avfilter    configuration: --enable-ffplay --enable-gpl --enable-libx264 --enable-libx265 --enable-static --enable-shared --enable-debug --disable-x86asm --enable-libsrt --extra-cflags=-g --extra-ldflags=-g --enable-libfdk-aac --enable-libsrt --enable-nonfree
  libavutil      56. 36.101 / 56. 39.100
  libavcodec     58. 64.101 / 58. 68.102
  libavformat    58. 35.101 / 58. 38.100
  libavdevice    58.  9.101 / 58.  9.103
  libavfilter     7. 68.100 /  7. 75.100
  libswscale      5.  6.100 /  5.  6.100
  libswresample   3.  6.100 /  3.  6.100
  libpostproc    55.  6.100 / 55.  6.100
Splitting the commandline.
Reading option '-help' ... matched as option 'help' (show help) with argument '(null)'.
Finished splitting the commandline.
Parsing a group of options: global .
Applying option help (show help) with argument (null).
Hyper fast Audio and Video encoder
usage: ffmpeg [options] [[infile options] -i infile]... {[outfile options] outfile}...

**Getting help:**
    -h      -- print basic options
    -h long -- print more options
    -h full -- print all options (including all format and codec specific options, very long)
    -h type=name -- print all options for the named decoder/encoder/demuxer/muxer/filter/bsf
    See man ffmpeg for detailed description of the options.

**Print help / information / capabilities:**
-L                  show license
-h topic            show help
-? topic            show help
-help topic         show help
--help topic        show help
-version            show version
-buildconf          show build configuration
-formats            show available formats
-muxers             show available muxers
-demuxers           show available demuxers
-devices            show available devices
-codecs             show available codecs
-decoders           show available decoders
-encoders           show available encoders
-bsfs               show available bit stream filters
-protocols          show available protocols
-filters            show available filters
-pix_fmts           show available pixel formats
-layouts            show standard channel layouts
-sample_fmts        show available audio sample formats
-colors             show available color names
-sources device     list sources of the input device
-sinks device       list sinks of the output device
-hwaccels           show available HW acceleration methods

**Global options (affect whole program instead of just one file:**
-loglevel loglevel  set logging level
-v loglevel         set logging level
-report             generate a report
-max_alloc bytes    set maximum size of a single allocated block
-y                  overwrite output files
-n                  never overwrite output files
-ignore_unknown     Ignore unknown stream types
-filter_threads     number of non-complex filter threads
-filter_complex_threads  number of threads for -filter_complex
-stats              print progress report during encoding
-max_error_rate maximum error rate  ratio of errors (0.0: no errors, 1.0: 100% errors) above which ffmpeg returns an error instead of success.
-bits_per_raw_sample number  set the number of bits per raw sample
-vol volume         change audio volume (256=normal)

**Per-file main options:**
-f fmt              force format
-c codec            codec name
-codec codec        codec name
-pre preset         preset name
-map_metadata outfile[,metadata]:infile[,metadata]  set metadata information of outfile from infile
-t duration         record or transcode "duration" seconds of audio/video
-to time_stop       record or transcode stop time
-fs limit_size      set the limit file size in bytes
-ss time_off        set the start time offset
-sseof time_off     set the start time offset relative to EOF
-seek_timestamp     enable/disable seeking by timestamp with -ss
-timestamp time     set the recording timestamp ('now' to set the current time)
-metadata string=string  add metadata
-program title=string:st=number...  add program with specified streams
-target type        specify target file type ("vcd", "svcd", "dvd", "dv" or "dv50" with optional prefixes "pal-", "ntsc-" or "film-")
-apad               audio pad
-frames number      set the number of frames to output
-filter filter_graph  set stream filtergraph
-filter_script filename  read stream filtergraph description from a file
-reinit_filter      reinit filtergraph on input parameter changes
-discard            discard
-disposition        disposition

**Video options:**
-vframes number     set the number of video frames to output
-r rate             set frame rate (Hz value, fraction or abbreviation)
-s size             set frame size (WxH or abbreviation)
-aspect aspect      set aspect ratio (4:3, 16:9 or 1.3333, 1.7777)
-bits_per_raw_sample number  set the number of bits per raw sample
-vn                 disable video
-vcodec codec       force video codec ('copy' to copy stream)
-timecode hh:mm:ss[:;.]ff  set initial TimeCode value.
-pass n             select the pass number (1 to 3)
-vf filter_graph    set video filters
-ab bitrate         audio bitrate (please use -b:a)
-b bitrate          video bitrate (please use -b:v)
-dn                 disable data

**Audio options:**
-aframes number     set the number of audio frames to output
-aq quality         set audio quality (codec-specific)
-ar rate            set audio sampling rate (in Hz)
-ac channels        set number of audio channels
-an                 disable audio
-acodec codec       force audio codec ('copy' to copy stream)
-vol volume         change audio volume (256=normal)
-af filter_graph    set audio filters

**Subtitle options:**
-s size             set frame size (WxH or abbreviation)
-sn                 disable subtitle
-scodec codec       force subtitle codec ('copy' to copy stream)
-stag fourcc/tag    force subtitle tag/fourcc
-fix_sub_duration   fix subtitles duration
-canvas_size size   set canvas size (WxH or abbreviation)
-spre preset        set the subtitle options to the indicated preset



- **ffmpeg -h demuxer=mp4**

Demuxer mov,mp4,m4a,3gp,3g2,mj2 [QuickTime / MOV]:
    Common extensions: mov,mp4,m4a,3gp,3g2,mj2,psp,m4b,ism,ismv,isma,f4v.
mov,mp4,m4a,3gp,3g2,mj2 AVOptions:
  -use_absolute_path <boolean>    .D.V...... allow using absolute path when opening alias, this is a possible security issue (default false)
  -seek_streams_individually <boolean>    .D.V...... Seek each stream individually to the closest point (default true)
  -ignore_editlist   <boolean>    .D.V...... Ignore the edit list atom. (default false)
  -advanced_editlist <boolean>    .D.V...... Modify the AVIndex according to the editlists. Use this option to decode in the order specified by the edits. (default true)
  -ignore_chapters   <boolean>    .D.V......  (default false)
  -use_mfra_for      <int>        .D.V...... use mfra for fragment timestamps (from -1 to 2) (default auto)
     auto            -1           .D.V...... auto
     dts             1            .D.V...... dts
     pts             2            .D.V...... pts
  -export_all        <boolean>    .D.V...... Export unrecognized metadata entries (default false)
  -export_xmp        <boolean>    .D.V...... Export full XMP metadata (default false)
  -activation_bytes  <binary>     .D........ Secret bytes for Audible AAX files
  -audible_fixed_key <binary>     .D........ Fixed key used for handling Audible AAX files
  -decryption_key    <binary>     .D........ The media decryption key (hex)
  -enable_drefs      <boolean>    .D.V...... Enable external track support. (default false)

- 

- **ffmpeg命令行参数**

  - 选项组定义

    typedef struct OptionGroupDef {
        /**< group name */
        const char *name; //组名:
      /**
         * Option to be used as group separator. Can be NULL for groups which
              * are terminated by a non-option argument (e.g. ffmpeg output files)
              */
            const char *sep;//输入参数：i, 输出参数和全局参数：NULL
            /**
              * Option flags that must be set on each option that is
                   * applied to this group
                   */
                int flags;
  } OptionGroupDef;
    
  - 参数解析上下文

    typedef struct OptionParseContext {
        OptionGroup global_opts;//全局参数

        OptionGroupList *groups;//输入输出参数列表：0输出，1输入
        int           nb_groups;
        
        /* parsing state */
        OptionGroup cur_group;
    } OptionParseContext;

  - 命令行输入的参数被解析后保存在OptionGroup中（ffmpeg_opt.c中）

    typedef struct OptionGroup {
        const OptionGroupDef *group_def;
        const char *arg;

        Option *opts;//指向了一个option数组
        int  nb_opts;
        
        AVDictionary *codec_opts;//编解码相关参数
        AVDictionary *format_opts;//封装格式相关参数
        AVDictionary *resample_opts;//重采样相关参数
        AVDictionary *sws_dict;
        AVDictionary *swr_opts;

    } OptionGroup;

    OptionsContext结构体持有OptionGroup 指针。

  - FFmpeg命令行应用初始化调用：ffmpeg_parse_options执行以下流程

    - split_commandline：分割命令行传入的参数，参数存入内存
    - parse_optgroup;应用全局参数
    - open_files:循环调用**open_input_file**打开所有输入文件
      - 输入参数临时保存到：OptionsContext
    - init_complex_filters：初始化复制滤镜
    - open_files：循环调用**open_output_file**打开所有输出文件
      - 输入参数临时保存到：OptionsContext
    - check_filter_outputs：检查滤镜输出

  - 方法：static int **open_input_file**(OptionsContext *o, const char *filename) 打开输入文件

    ​	整个流程基本就是处理参数、初始化AVFormatContext，探测编码格式获取到对应的解码器

    - 根据参数设置情况修正并设置参数
    - 调用：av_find_input_format 查找输入文件封装格式
    - 调用：**avformat_open_input**(&ic, filename, file_iformat, &o->g->**format_opts**); 传入OptionGroup 中format_opts。
    - 调用：add_input_streams 获取流信息并加到输入流数组中
      - 调用：avcodec_alloc_context3 根据某路流对应的编码器创建新的AVCodecContext
      - 调用：avcodec_parameters_to_context 获取新某路流对应的编码器参数给新创建的AVCodecContext
      - 调用：filter_codec_opts 获取解码器选项参数
      - 解析**帧率、硬件加速**等相关参数
      - 调用：avcodec_parameters_from_context 参数设置给编码器的参数

    - 方法：**init_complex_filters** 解析滤镜相关参数
    - 

  - 方法： int **open_output_file**(OptionsContext *o, const char *filename) 打开输出文件

    ​	整个流程基本就是处理参数、初始化AVFormatContext，转封装新建对应的流，处理输入流和输出流的对应关系，如果需要编码就根据参数创建对应的编码器，打开输出文件

    - 创建出事文件的AVFormatContext
    - 根据参数设置情况修正并设置参数
    - 循环初始化滤镜组
      - 调用：init_output_filter
    - 判断流的maps关系
      - **选择创建输出流，每种流只会创建一个**
        - 调用：**new_video_stream** 根据输入文件最高分辨率创建对应的输出流
          - 调用：new_output_stream 创建视频流，并创建了编码的AVCodecContext ost->**enc_ctx**且获取了命令行的编码器参数初始化编码器
          - 一系列解析命令行传入的视频编码器参数
        - 调用：**new_audio_stream**，选择输入流中通道计算分数最高的流，来创建一个输出音频流
          - 调用：new_output_stream 创建视频流，并创建了编码的AVCodecContext ost->**enc_ctx**且获取了命令行的编码器参数初始化编码器
          - 一系列解析命令行传入的视频编码器参数
      - **根据map关系创建输出流**
        - 调用：**new_video_stream** 根据输入文件最高分辨率创建对应的输出流
          - 调用：new_output_stream 创建视频流，并创建了编码的AVCodecContext ost->**enc_ctx**且获取了命令行的编码器参数初始化编码器
          - 一系列解析命令行传入的视频编码器参数
        - 调用：**new_audio_stream**，选择输入流中通道计算分数最高的流，来创建一个输出音频流
          - 调用：new_output_stream 创建视频流，并创建了编码的AVCodecContext ost->**enc_ctx**且获取了命令行的编码器参数初始化编码器
          - 一系列解析命令行传入的视频编码器参数
    - 处理附加文件
    - 检查所有的编码器参数都被应用了
    - 设置解码器需要的标志，创建简单的滤镜
    - ...
    - 复制元数据信息
    - 复制章节
    - 复制默认的元数据
    - 处理节目信息
    - 处理人工设置元数据

    - 方法：**new_output_stream** 根据传入参数创建输出的视频流、音频流
      - 创建AVStream保存到OutputStream
      - 保存一些参数到OutputStream
      - 选择编码器保存到OutputStream
      - 创建编码的AVCodecContext，保存到OutputStream ost->**enc_ctx**,后面的流程会获取命令行编码器相关参数
      - 分配默认编码器参数,
      - 获取命令行传入的编码器参数，保存到ost->encoder_opts
      - 获取命令行时间基参数
      - 获取命令行max_frames参数
      - ...
      - 

- **FFmpeg 库中与参数相关结构体**

  - **结构体**：**AVDictionary**
    - key，value
  - **结构体：AVOption**
    - 参数项容器
  - **编码器参数项数组**
    -  每种编码都有一个静态AVOption数组，保存了支持的参数及设置参数的规则
  
    - 封装器
  - **结构体：AVClass**
    
    - 持有AVOption指针
  - **结构体：AVCodec**
    - 持有AVClass指针
    
    - init 方法：初始化为对应的编码器初始化方法
        - 方法中使用AVCodecContext中编解码相关参数初始化指定编码器的参数
  - **结构体：AVCodecContext**
    
    - 持有AVCodec指针
      - void *priv_data 保存对应编码器实现的指针
      - **编码解码相关参数**
  - **结构体：AVStream**
  
    - 持有AVCodecContext指针
  - **结构体：AVInputFormat**
    
    - 持有AVClass指针：priv_class，保存对应输入流的封装协议参数
  - **结构体：AVOutputFormat**
    
    - 持有AVClass指针：priv_class，保存对应输入流的封装协议参数
  - **结构体：AVFormatContext**
    
    - 持有AVClass指针
      - 值为avformat_options 定义在libavcodec/options_table.h文件中，定义了封装格式所支持的所有参数
    - 持有AVInputFormat：解封装时才有
    - 持有AVOutputFormat：封装时才有
  
- **参数相关方法及流程**

  - **编解码**
    - 方法：avcodec_alloc_context3 初始化AVCodecContext 并设置默认编解码参数
      - 内部调用init_context_defaults
        - 调用：av_opt_set_defaults  设置默认参数
        - 调用av_opt_set 设置**键值对参数**
    - 方法：avcodec_get_context_defaults3 获取默认编解码参数
  - **封装**
    
    - 方法：avformat_open_input，打开输入，封装参数要没传入指定，要么自动探测
    
      - avformat_open_input
    
        - avformat_alloc_context
    
          - avformat_get_context_defaults
    
            - av_opt_set_defaults
    
              - av_opt_set_defaults2  
    
                遍历AVCodecContext  AVClass里面封装格式AVOption列表，设置成默认值
    
        - 如果传入了options,则用传入的覆盖默认参数
  
- **参数表**

  - 封装参数表 ：libavformat/options_table.h
    - **官方文档地址**：https://www.ffmpeg.org/ffmpeg-formats.html#Format-Options
  - 编解码参数表 ：libavcodec/options_table.h
    - **官方文档地址**：https://www.ffmpeg.org/ffmpeg-codecs.html#Codec-Options



## AVOptions  

- 参数容器，ffmpeg中所有相封装，编解码，滤镜都是用这个来存储参数的
- 官网详细介绍网址：[AVOptions](https://www.ffmpeg.org/ffmpeg.html#toc-AVOptions)



## 总结

- 使用FFmpeg命令行工具需要了解FFmpeg参数规则

  - **ffmpeg**：https://www.ffmpeg.org/ffmpeg.html
  - **ffmpeg-all:**https://www.ffmpeg.org/ffmpeg-all.html
  - **ffplay**：https://www.ffmpeg.org/ffplay.html

  - **ffplay-all**：https://www.ffmpeg.org/ffplay-all.html
  - **ffprobe**：https://www.ffmpeg.org/ffprobe.html
  - **ffprobe-all**：https://www.ffmpeg.org/ffprobe-all.html

- FFmpeg工程应用

  - AVDictionary 键值对容器，sdk外部都是通过该数据结构的方法设置参数，内部再把参数存储到存储AVOptions的容器中

    - 数据结构
    - 增删改查方法

  - 阅读源码了解AVOptionss内部是如何使用的

    -  数据结构
    - 增删改查方法
    - 查看对应模块支持哪些参数
      - 共性参数
      - 三方库特有参数

  - 官方参考资料

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