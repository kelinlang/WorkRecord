[]()

# 参考资料

- [FFmpeg时间戳详解](https://cloud.tencent.com/developer/article/1409507)
- [详解FFplay音视频同步](https://juejin.im/post/5cad790f51882518b87e140)
- [ffplay源码分析4-音视频同步](https://www.cnblogs.com/leisure_chn/p/10307089.html)

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

  - avformat_alloc_context
  - avformat_open_input
  - avformat_find_stream_info
  - 调用：stream_component_open
    - 寻找Codec
    - 创建解码线程
      - 音频
        - 调用：audio_open	
          - 设置回调：sdl_audio_callback
        - 开启音频解码线程： **audio_thread**
      - 视频
        - 开启视频解码线程：**video_decoder**
      - 字幕
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

  - **retry块流程**

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

      **视频同步操作开始**

      - 计算：last_duration  上一帧显示的持续时间
      - 计算：delay  当前帧需要显示的时间
      - 获取当前时间戳
      - 如果当前时间小于显示时间 则直接进行显示
        - 跳转到：**display 块流程**
      - 更新视频的基准时间
      - 如果当前时间与基准时间偏差大于 AV_SYNC_THRESHOLD_MAX 则把视频基准时间设置为当前时间 
      - 更新视频时间轴
      - 如果队列中有未显示的帧，如果开启了丢帧处理或者不是以视频为主时间轴，则进行丢帧处理

  - **display 块流程**

- **sdl_audio_callback**  sdl播放音频的回调

  - 保存回调时间戳
  - audio_decode_frame
    - synchronize_audio
      - 非音频作为主时钟
        - 获取音频时钟和主时钟差值
        - 计算并返回同步需要的采样数
      - 

- **存放AVPacket队列分析**：链表实现

  typedef struct PacketQueue {
      MyAVPacketList *first_pkt, *last_pkt;
      int nb_packets;
      int size;
      int64_t duration;
      int abort_request;
      int serial;//初始化为0，后面累加
      SDL_mutex *mutex;
      SDL_cond *cond;
  } PacketQueue;

- **存放AVFrame队列分析**：数组实现

  `typedef struct FrameQueue {`
      `Frame queue[FRAME_QUEUE_SIZE];`//数组来实现队列
      `int rindex;//读索引`
      `int windex;//写索引`
      `int size;` 
      `int max_size;`
      `int keep_last;//是否保存最后的，音频和视频都需要`
      `int rindex_shown;//读索引 显示`
      `SDL_mutex *mutex;`
      `SDL_cond *cond;`
      `PacketQueue *pktq;`
  `} FrameQueue;

  - frame_queue_init : 初始化方法
  - frame_queue_destory：销毁方法
  - frame_queue_signal：信号方法，通知等待的那边
  - frame_queue_peek：获取当前显示的