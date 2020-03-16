## FFmpeg测试命令汇总

- **测试命令**
  - 统计B/I/P帧数目
    
    ffprobe -v quiet -show_frames transcoded123.mp4 | grep "pict_type=B" | wc -l
    
  - 编解码
  
    - 插入B帧
  
      `./ffmpeg -i ../../../testFiles/E1.mp4 -c:v libx264 -x264opts "bframes=10:b-adapt=0" -b:v 1000k -maxrate 1000k -minrate 1000k -bufsize 50k -nal-hrd cbr -g 50 -sc_threshold 0 -t 60 ../../../testFiles/E1_had_b.ts`
  
    - 去B帧
  
      `./ffmpeg -re  -stream_loop -1 -i ../testFiles/test1.ts -vcodec libx264 -profile:v baseline -level 3.1 -s 1920x1080 -an -y  ../testFiles/test1_ts_with_no_b.ts`
  
    - 转码音频到mp4文件
  
      `./ffmpeg -i ../testFiles/leishen.wav -codec:a libfdk_aac -vbr 3 ../testFiles/leishen.mp4`
  
    - Use `ffmpeg` to convert an audio file to CBR 64k kbps AAC, using the High-Efficiency AAC profile
  
      `./ffmpeg -i input.wav -c:a libfdk_aac -profile:a aac_he -b:a 64k output.m4a`
  
    - 指定X264编码
  
      `ffmpeg -i foo.mpg -c:v libx264 -x264opts keyint=123:min-keyint=20 -an out.mkv`
  
    - 指定X264特定参数：x264-params
  
      ```
      ffmpeg -i INPUT -c:v libx264 -x264-params level=30:bframes=0:weightp=0:\
      cabac=0:ref=1:vbv-maxrate=768:vbv-bufsize=2000:analyse=all:me=umh:\
      no-fast-pskip=1:subq=6:8x8dct=0:trellis=0 OUTPUT
      ```
  
    - 指定x265编码：-x265-params
  
      ```
      ffmpeg -i input -c:v libx265 -x265-params crf=26:psy-rd=1 output.mp4
      ```
      
    - 硬件加速
  
      ./ffmpeg -y -vsync 0 -hwaccel cuvid -c:v h264_cuvid -i ./video-h265.mkv -vf
      scale_cuda=1280:720 -c:a copy -c:v h264_nvenc -b:v 5M ./video-h265-1.mp4  
  
      测试结果：scale_cuda参数不支持
      
    - 视频设置码率
  
      ```
      ffmpeg -i input.avi -b:v 64k -bufsize 64k output.avi
      ```
  
    - 改变视频帧率
  
      ```
      ffmpeg -i input.avi -r 24 output.avi
      ```
  
    - 将输入文件的帧速率(仅对原始格式有效)强制为1帧，将输出文件的帧速率强制为24帧
  
      ```
      ffmpeg -r 1 -i input.m2v -r 24 output.avi
      ```
  
  - 封装格式
  
    - srt推拉流
  
      ./ffmpeg -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -c copy -f mpegts "srt://192.168.2.169:12500?pkt_size=1316&mode=listener"
  
      ./ffplay -fflags nobuffer "srt://192.168.2.169:12500/live/test?paket_size=1316&mode=caller"
  
    - map使用
  
      `./ffmpeg -re -i ./E1.mp4 -i ./Record-null.ts -map 0:0 -map 0:1 -map 1:0 -map 1:1 -c copy -program title=CCTV1:program_num=1:st=0:st=1 -program title=CCTV2:program_num=2:st=2:st=3 -f mpegts "udp://192.168.2.34:1234?pkt_size=1316"`
  
      选择流例如-map 0:1:表面第1个输入文件的第二个流
  
    - 转封装
  
      ./ffmpeg -re -i rtsp://192.168.3.101/ch01  -f flv rtmp://192.168.0.18:1935/live/test
  
    - 转码转封装
  
      ./fmpeg -re -i rtsp://192.168.3.101/ch01 -c:v libx265 -profile:v main -c:a aac -b:a 128k -f flv rtmp://192.168.2.162:1935/live/test
  
  - 滤镜
  
    - 这个过滤器图将输入流分成两个流，然后通过裁剪过滤器和vflip过滤器发送一个流，然后将它与另一个流合并，并将其覆盖在上面
  
      ./ffmpeg -i ../testFiles/E1.mp4 -vf "split [main][tmp]; [tmp] crop=iw:ih/2:0:0, vflip [flip]; [main][flip] overlay=0:H/2" ../testFiles/E1_with_crop_and_vflip.mp4
  
    - 视频转个分辨率
  
      ./ffmpeg -i ../testFiles/E1.mp4 -vf scale=640:360 ../testFiles/E1_640_360.mp4
    
    
    
    



