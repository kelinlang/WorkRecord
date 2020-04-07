## FFmpeg加入新模块流程

- 编译选项	

  - ./ffmpeg -hwaccels

- 媒体协议

- 编解码

  - codeclist

    - ff_libx264_encoder,
    - ff_libx264rgb_encoder
    -  ff_h264_v4l2m2m_encoder,

  - AVCodec

    - allcodecs.c文件中定义所有支持的编解码器

    extern AVCodec ff_libx264_encoder;
    extern AVCodec ff_libx264rgb_encoder;

    extern AVCodec ff_libopenh264_encoder;
    extern AVCodec ff_libopenh264_decoder;
    extern AVCodec ff_h264_amf_encoder;
    extern AVCodec ff_h264_cuvid_decoder;
    extern AVCodec ff_h264_nvenc_encoder;
    extern AVCodec ff_h264_omx_encoder;
    extern AVCodec ff_h264_qsv_encoder;
    extern AVCodec ff_h264_v4l2m2m_encoder;
    extern AVCodec **ff_h264_vaapi_encoder**;
    extern AVCodec ff_h264_videotoolbox_encoder;

    - 

- 硬件加速

  - 命令行
    - ffmpeg -vcodec h264_vda -i input.mp4 -vcodec h264_videotoolbox -b:v 2000k 
    - ./ffmpeg -y -vsync 0 -hwaccel cuvid -c:v h264_cuvid -i ./video-h265.mkv -vf
      scale_cuda=1280:720 -c:a copy -c:v h264_nvenc -b:v 5M ./video-h265-1.mp4  
  -  **硬件加速列表文件：libavcodec/hwaccels.h**
    - extern const AVHWAccel ff_h264_d3d11va_hwaccel;
      extern const AVHWAccel ff_h264_d3d11va2_hwaccel;
      extern const AVHWAccel ff_h264_dxva2_hwaccel;
      extern const AVHWAccel ff_h264_nvdec_hwaccel;
      extern const AVHWAccel ff_h264_vaapi_hwaccel;
      extern const AVHWAccel ff_h264_vdpau_hwaccel;
      extern const AVHWAccel ff_h264_videotoolbox_hwaccel;
  - avcodec.h中
    - struct AVHWAccel;定义硬件加速相关接口
    - AVCodec持有AVCodecHWConfigInternal硬件配置数组
    - AVCodecContext
      - const struct AVHWAccel* hwaccel
  - 硬件配置结构体
    - typedef struct AVCodecHWConfigInternal {
          /**
           * This is the structure which will be returned to the user by
                * avcodec_get_hw_config().
                */
              AVCodecHWConfig public;
              /**
                * If this configuration uses a hwaccel, a pointer to it.
                     * If not, NULL.
                     */
                  const AVHWAccel *hwaccel;
      } AVCodecHWConfigInternal;
    - AVHWDeviceType保存了硬件类型
  - decode.c 中方法ff_get_format用来更加配置的参数选择对应的硬件编码器
  - hwaccel_init
  - hwaccel_uninit
  - 
  - 
  - - 

  



硬件视频加速

- [视频加速API (Video Acceleration API, 缩写为VA-API)](http://www.freedesktop.org/wiki/Software/vaapi) 是一套 Intel 提供的视频硬件编解码的开源库和标准。
- [Unix视频解码及演示 API （Video Decode and Presentation API for Unix，缩写为VDPAU）](https://www.freedesktop.org/wiki/Software/VDPAU/) (VDPAU) 是一套 Nvidia 提供的把部分视频解码和后期处理工作转移到显卡的开源库和 API。





mplayer

gstreamer