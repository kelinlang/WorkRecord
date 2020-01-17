

- 阅读ffmpeg加log测试验证

  - E1.mp4  无B帧
    - 显示时间戳和解码时间戳一样
  - video-h265.mkv 视频文件有**B帧**，有显示时间戳，无解码时间戳
    - ffplay播放本地视频
      - 解码前AVPacket：pts:120 dts:NOPTS    
      - 解码后AVFrame：pts:120,pkt_pts:120, pkt_dts:120
    - ffplay 播放rtsp  会出现马赛克
      - 解码前AVPacket：pts:3449832383 dts:NOPTS    
      - 解码后AVFrame：pts:3449832383,pkt_pts:3449832383, pkt_dts:3449832383
    - ffplay 播放rtsp  没有马赛克
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
      - rtsp拉流
        - 接收包pts会放入pts_buffer数组做排序处理
        - AVStream 里面有个pts_buffer，select_from_pts_buffer从中选择pts设置给dts
        - select_from_pts_buffer 里面算法
          - 如果dts无效值，会根据pts_reorder_error[i]/pts_reorder_error_count[i]计算出一个分数值，如果分数值小于最大分数值，则把当前分数值设置为最大分数值，取pts_buffer[i]设置给dts
    - 总结：
      - 实时视频中传输过程中必须要有pts
      - 如果实时视频中没有B帧
        - 视频帧dts=pts，ffmpeg是对解码时间戳的处理是没问题的
      - 如果实时视频中**有B帧**
        - 视频帧中会有dts != dts,ffmpeg的处理是拿记录的pts做一个权衡计算，取一个计算最优pts设置给dts，实际测试过程中发现ffplay播放有B帧的rtsp视频流，会出现马赛克**模糊**，排除虚拟机性能问题，马赛克模糊问题源于对dts的处理。
        - 实时视频有B帧，模糊解决办法
          - dts不应该取dts来生成，而应该用视频帧的传输顺序来生成dts，因为传输顺序同解码顺序

