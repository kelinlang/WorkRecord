- ffmpeg x264 编码参数对照表:[参考网址](https://blog.csdn.net/byxdaz/article/details/80663718)

- ffmpeg x264官方参数文档：https://www.ffmpeg.org/ffmpeg-codecs.html#libx264_002c-libx264rgb

- **ffmpeg 编解码命令参数** [参考网址](https://blog.csdn.net/Lyman_Ye/article/details/80305904)
  
  - **编码器预设参数preset**
  
    主要调节编码速度和质量的平衡，有ultrafast、superfast、veryfast、faster、fast、medium、slow、slower、veryslow、placebo这10个选项，从快到慢，默认的编码速度是medium模式
  
    ./ffmpeg -i ../testFiles/E1.mp4 -vcodec libx264 -preset ultrafast -b:v 2000k E1_2000k.mp4
  
  - **编码优化参数tune**
  
    这个参数配合视频类型和视觉优化的参数。可选项
  
    - film:电影真人类型
    - animation：动画
    - grain：需要保留大量的grain
    - stillimage：静态图像编码时使用
    - psnr：提高psnr做了优化的参数
    - ssim：提高ssim做了优化参数
    - fastdecode：可以快速解码的参数
    - zerolatency：零延迟，用在需要非常低的延迟的情况，比如电视电话会议编码
  
    ./ffmpeg -i input.mp4 -vcodec libx264 -tune zerolatency -b:v 2000k output.mp4
  
  - **profile和level设置**
  
    profile和level的设置和H.264标准文档ISO-14496-Part10描述的profile和level信息基本相同。profile有如下选项：
  
    - Baseline：编码后的视频不含B帧
    - Extented
    - Main
    - High：编码后的视频含B帧
    - High10
    - High422
    - High444
  
    profile设置信息不同会影响编码出来的视频的很多参数不同。例如是否支持I与P分片。level也会影响很多参数，例如最大解码速度不同。
  
    下面使用baseline profile和high profile编码一个H.264视频，分析两个编码出来的文件的区别。有一个知识我们提取了解一下baseline profile编码出来额视频不会包含B帧，而high profile包含B帧，下面就看它们B帧的差别
  
    ./ffmpeg -i input.mp4 -vcodec libx264 -profile:v baseline -level 3.1 -s 352x288 -an -y -t 10 ouput_baseline.ts
    ./ffmpeg -i input.mp4 -vcodec libx264 -profile:v high -level 3.1 -s 352x288 -an -y -t 10 ouput_high.ts
    生成了两个文件，通过ffprobe来查看包含B帧的信息
  
    ./ffprobe -v quiet -show_frames -select_streams v output_baseline.ts |grep "pict_type=B"|wc -l
    输出0
  
    ./ffprobe -v quiet -show_frames -select_streams v output_high.ts |grep "pict_type=B"|wc -l
    输出161
    验证了我们的理论baseline profile包含0个B帧，而high profile包含B帧。
  
  - **sc_threshold**
  
    FFmpeg可以通过参数-g设置帧数间隔为GOP的长度，但是遇到场景切换的时候，从一个画面突然变为另一个画面时，会强行插入一个关键帧，这时GOP的长度会重新开始。可以通过参数sc_threshold决定是否在场景切换的时候插入关键帧。执行命令控制编码时GOP的大小。
  
    - ./ffmpeg -i input.mp4 -c:v libx264 -g 50 -t 60 output.mp4   遇到场景切换会插入I帧
    - ./ffmpeg -i input.mp4 -c:v libx264 -g 50 -sc_threshold 0 -t 60 output.mp4  遇到场景切换不会插入I帧
  
  - **x264opts**
  
    由于FFmpeg设置x264参数时增加的参数比较多，FFmpeg开放了x264opts，可以通过这个参数设置x264的内部私有参数，如设置IBP帧的顺序以及规律。下面列举在上面生成的GOP文件数据分析基础上控制生成的文件不出现B帧，只要设置x264内部参数bframes=0即可：
  
    - ./ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=0" -g 50 -sc_threshold 0 -t 60 output.mp4  
  
    如果希望控制I帧P帧B帧的频率和规律，可以通过控制GOP中B帧的帧数来实现，P帧的频率可以通过x264的参数b-adapt进行设置。
    例如设置GOP中，每2个P帧之间存放3个B帧：
  
    - ./ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=3:b-adapt=0" -g 50 -sc_threshold 0 -t 60 output.mp4
  
  - **nal-hrd**
  
    编码可以设置VBR，CBR的编码模式，VBR为可变码率，CBR为恒定码率。互联网上VBR居多，但是我们可以使用FFmpeg制作CBR码率视频。
  
    - ffmpeg -i input.mp4 -c:v libx264 -x264opts "bframes=10:b-adapt=0" -b:v 1000k -maxrate 1000k -minrate 1000k -bufsize 50k -nal-hrd cbr -g 50 -sc_threshold 0 -t 60 output.ts
      命令执行参数介绍
      - 设置B帧个数，每两个P帧之间包含10个B帧
      - 设置视频码率为1000kbit/s
      - 设置最大码率为1000kbit/s
      - 设置最小码率为1000kbit/s
      - 设置编码的buffer大小为50KB
      - 设置H.264的编码HRD信号为CBR
      - 设置每50帧一个GOP
      - 设置场景切换不强行插入关键帧
      - 设置视频输出时间为60s
    
  - **硬编解码加速**
  
    测试命令：ffmpeg -vcodec h264_vda -i input.mp4 -vcodec h264_videotoolbox -b:v 2000k output.mp4
  
    windows环境编译繁琐，暂没成功，暂无环境测试
    
    **英伟达gpu硬件加速版本FFmpeg编译文档**（此文档当前目录下面）：	
    
    ​	Using_FFmpeg_with_NVIDIA_GPU_Hardware_Acceleration_v01.4.pdf
    
    文档里面有windows和linux版本编译方法，和相关命令的使用方法
    
    
    
    **下载编译好的ffmpeg**：[下载地址](https://ffmpeg.zeranoe.com/builds/win64/static/ffmpeg-20200213-6d37ca8-win64-static.zip)
    
    - ./ffmpeg -y -vsync 0 -hwaccel cuvid -c:v h264_cuvid -i ./video-h265.mkv -vf
      scale_cuda=1280:720 -c:a copy -c:v h264_nvenc -b:v 5M ./video-h265-1.mp4  
    
      测试结果：scale_cuda参数不支持
    
  - **X264私有参数设置方法**
  
    ./ffmpeg -i ../../../testFiles/E1.mp4 -c:v libx264 -x264-params level=30:bframes=0:weightp=0:\
    cabac=0:ref=1:vbv-maxrate=768:vbv-bufsize=2000:analyse=all:me=umh:\
    no-fast-pskip=1:subq=6:8x8dct=0:trellis=0 ../../../testFiles/E1_x264_params.mp4
  
- **FFmpeg其他测试命令**

  - 视频文件插入B帧

    ./ffmpeg -i ../../../testFiles/E1.mp4 -c:v libx264 -x264opts "bframes=10:b-adapt=0" -b:v 3000k -maxrate 3000k -minrate 3000k  -nal-hrd cbr -g 50 -sc_threshold 0 -t 60 ../../../testFiles/E1_had_b.ts

