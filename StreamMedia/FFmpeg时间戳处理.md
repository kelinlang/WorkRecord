

- 阅读ffmpeg加log测试验证

  - E1.mp4  无B帧
    - 显示时间戳和解码时间戳一样
  - video-h265.mkv 视频文件有**B帧**，有显示时间戳，无解码时间戳
    - ffplay播放本地视频
      - 解码前AVPacket：pts:120 dts:NOPTS    
      - 解码后AVFrame：pts:120,pkt_pts:120, pkt_dts:120
    - ffplay 播放rtsp 
      - 解码前AVPacket：pts:3449832383 dts:NOPTS    
      - 解码后AVFrame：pts:3449832383,pkt_pts:3449832383, pkt_dts:3449832383
    - ffplay 播放rtsp 
      - 解码前AVPacket：pts:80,dts:80   
      - 解码后AVFrame：pts:80,pkt_pts:80, pkt_dts:80
  - 裸流 1080P.h265  无B帧
    - ffplay播放裸流h265，显示时间戳和解码时间戳从读取到播放整个流程都是一样的，始终为NOPTS

  

- 结论
  - 解码时间戳和传输顺序存储顺序一样，所以对于文件存储和实时视频的封装格式中是否有解码时间戳无关紧要，只要保证存储顺序和传输顺序即可，对于走udp传输的，接收包必须做排序处理，只要这个顺序保证了，就能正常解码。
  - 带B帧视频，裸流数据送解码器按照顺序传输顺序送，读取解码后的视频帧就是按显示时间戳排序的
  - 实时视频一般都设置无编码器不编码B帧
  - ffmpeg 源码在av_read_frame   这函数中会做些处理，正常情况下读入输入流时获取到的时间戳是什么值，到播放器后就是什么值
    - 处理
      - 溢出处理函数：wrap_timestamp，时间戳不在指定范围就修正到指定范围
      - 时间基设置函数：avpriv_set_pts_info，不同封装格式会设置不同的时间基
      - update_initial_durations 函数
        - rtsp接收的AVPacket中dts为NOPTS，经过此函数后，dts设置成了pts
      - ...

