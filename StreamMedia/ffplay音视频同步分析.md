

# 参考资料

- [FFmpeg时间戳详解](https://cloud.tencent.com/developer/article/1409507)
- [详解FFplay音视频同步](https://juejin.im/post/5cad790f51882518b87e140)

#  ffplay流程

- **主线程**

  - **stream_open** 开启读取线程：**read_thread**
  - **event_loop** 事件循环
    - 等待事件
      - refresh_loop_wait_event 循环
        - 出错 终止循环
        - 读取到事件，终止循环，返回后外部处理事件
        - **remaining_time** :**主线程刷新循环休眠时间**
          - 大于0,主线程**休眠**remaining_time
        - 重设**remaining_time** 为10ms
        - **video_refresh**：视频刷新
    - 处理事件

- **read_thread:** 读取数据线程：stream_open中开启

  - 调用：stream_component_open
    - 调用：audio_open	
      - 设置回调：sdl_audio_callback
    - 开启音频解码线程： **audio_thread**
    - 开启视频解码线程：**video_decoder**
    - 开启字幕解码线程：**subtitle_thread**
  - 循环读取输入流
    - 丢弃不在用户指定范围内的帧
    - 音视频数据放入对应的队列

  

- **video_decoder**  解码线程 循环
  - 调用：get_video_frame
    - 调用：decoder_decode_frame
      - 队列去视频数据解码，返回解码后数据
    - 丢帧处理
  - 显示时间戳计算
    - **pts** = 解码后AVFrame中**pts*** av_q2d**(tb**)
      - **tb**:输入视频流中的时间基
  - 调用：queue_picture
    - 数据放入YUV Frame队列

- **audio_thread** 解码线程 循环
  - 调用：decoder_decode_frame
    - 队列去视频数据解码，返回解码后数据
  - 显示时间戳计算
    - `pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);`
      - **tb**:输入音频流中的时间基
  - 数据放入PCM Frame队列
- **stream_open** 初始化 **Clock**
  - **Video clock**
    - pts = 执行初始化时的时间戳
    - pts_drift = pts - 执行初始化时的时间戳
    - last_updated = 执行初始化时的时间戳
    - speed = 1.0
    - paused = 0
    - queue_serial = 0
    - serial = -1
  - **Audio clock**
    - pts = 执行初始化时的时间戳
    - pts_drift = pts - 执行初始化时的时间戳
    - last_updated = 执行初始化时的时间戳
    - speed = 1.0
    - paused = 0
    - queue_serial = 0
    - serial = -1
  - **ext clock**
    - pts = 执行初始化时的时间戳
    - pts_drift = pts - 执行初始化时的时间戳
    - last_updated = 执行初始化时的时间戳
    - speed = 1.0
    - paused = 0
    - queue_serial = 0
    - serial = -1

- **video_refresh** 视频显示

  - 判断是否是外部时钟基准并且是否是实时视频

    - 检查外部时钟的速率并设置速率

  - 判断显示模式并且是否有音频

    - 满足条件，获取当前时间戳，单位：秒

    - 强制刷新或者显示时间超过**rdftspeed**(20ms)

      - **显示视频**
      - 重设上一次显示时间

    - 设置**主线程刷新循环休眠时间**

      - 取值：

        `FMIN(*remaining_time, is->last_vis_time + rdftspeed - time);`

        **备注：**大于0时下一个循环主线程休眠remaining_time时间，介于0-10ms之间

  - retry块流程

    - Yuv帧队列中**无**Yuv帧
      - 跳过retry块流程
    - Yuv帧队列中**有**Yuv帧
      - 获取上一个显示的Yuv帧：lastvp
      - 获取当前显示的Yuv帧：vp
      - 比较当前帧serial和输入流中读取的数据包的累计数据目
        - 不相等，跳转到retry块流程重新执行
      - 比较当前帧和上一帧的serial
        - 不相等，设置frame_timer，单位：秒
      - 判断是否暂停了
        - 是，跳转到display块流程

- 