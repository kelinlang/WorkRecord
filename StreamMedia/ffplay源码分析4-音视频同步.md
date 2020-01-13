ffplay是FFmpeg工程自带的简单播放器，使用FFmpeg提供的解码器和SDL库进行视频播放。本文基于FFmpeg工程4.1版本进行分析，其中ffplay源码清单如下：
https://github.com/FFmpeg/FFmpeg/blob/n4.1/fftools/ffplay.c

在尝试分析源码前，可先阅读如下参考文章作为铺垫：
[1]. [雷霄骅，视音频编解码技术零基础学习方法](https://blog.csdn.net/leixiaohua1020/article/details/18893769)
[2]. [视频编解码基础概念](https://www.cnblogs.com/leisure_chn/p/10285829.html)
[3]. [色彩空间与像素格式](https://www.cnblogs.com/leisure_chn/p/10290575.html)
[4]. [音频参数解析](https://blog.csdn.net/caoshangpa/article/details/51218597)
[5]. [FFmpeg基础概念](https://www.cnblogs.com/leisure_chn/p/10297002.html)

“ffplay源码分析”系列文章如下：
[1]. [ffplay源码分析1-概述](https://www.cnblogs.com/leisure_chn/p/10301215.html)
[2]. [ffplay源码分析2-数据结构](https://www.cnblogs.com/leisure_chn/p/10301253.html)
[3]. [ffplay源码分析3-代码框架](https://www.cnblogs.com/leisure_chn/p/10301831.html)
[4]. [ffplay源码分析4-音视频同步](https://www.cnblogs.com/leisure_chn/p/10307089.html)
[5]. [ffplay源码分析5-图像格式转换](https://www.cnblogs.com/leisure_chn/p/10311376.html)
[6]. [ffplay源码分析6-音频重采样](https://www.cnblogs.com/leisure_chn/p/10312713.html)
[7]. [ffplay源码分析7-播放控制](https://www.cnblogs.com/leisure_chn/p/10316225.html)

# 4. 音视频同步

音视频同步的目的是为了使播放的声音和显示的画面保持一致。视频按帧播放，图像显示设备每次显示一帧画面，视频播放速度由帧率确定，帧率指示每秒显示多少帧；音频按采样点播放，声音播放设备每次播放一个采样点，声音播放速度由采样率确定，采样率指示每秒播放多少个采样点。如果仅仅是视频按帧率播放，音频按采样率播放，二者没有同步机制，即使最初音视频是基本同步的，随着时间的流逝，音视频会逐渐失去同步，并且不同步的现象会越来越严重。这是因为：一、播放时间难以精确控制，二、异常及误差会随时间累积。所以，必须要采用一定的同步策略，不断对音视频的时间差作校正，使图像显示与声音播放总体保持一致。

我们以一个44.1KHz的AAC音频流和25FPS的H264视频流为例，来看一下理想情况下音视频的同步过程：
一个AAC音频frame每个声道包含1024个采样点(也可能是2048，参“[FFmpeg关于nb_smples,frame_size以及profile的解释](https://blog.csdn.net/zhuweigangzwg/article/details/53335941)”)，则一个frame的播放时长(duration)为：(1024/44100)×1000ms = 23.22ms；一个H264视频frame播放时长(duration)为：1000ms/25 = 40ms。声卡虽然是以音频采样点为播放单位，但通常我们每次往声卡缓冲区送一个音频frame，每送一个音频frame更新一下音频的播放时刻，即每隔一个音频frame时长更新一下音频时钟，实际上ffplay就是这么做的。我们暂且把一个音频时钟更新点记作其播放点，理想情况下，音视频完全同步，音视频播放过程如下图所示：

![音视频同步理想情况](https://leichn.github.io/img/ffplay_analysis/ffplay_avsync_ideal.jpg)

音视频同步的方式基本是确定一个时钟(音频时钟、视频时钟、外部时钟)作为主时钟，非主时钟的音频或视频时钟为从时钟。在播放过程中，主时钟作为同步基准，不断判断从时钟与主时钟的差异，调节从时钟，使从时钟追赶(落后时)或等待(超前时)主时钟。按照主时钟的不同种类，可以将音视频同步模式分为如下三种：
**音频同步到视频**，视频时钟作为主时钟。
**视频同步到音频**，音频时钟作为主时钟。
**音视频同步到外部时钟**，外部时钟作为主时钟。
ffplay中同步模式的定义如下：

```
enum {
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};
```

## 4.1 time_base

time_base是PTS和DTS的时间单位，也称时间基。不同的封装格式time_base不一样，转码过程中的不同阶段time_base也不一样。以mpegts封装格式为例，假设视频帧率为25FPS。编码数据包packet(数据结构AVPacket)的time_base为AVRational{1,90000}，这个是容器层的time_base，定义在AVStream结构体中。原始数据帧frame(数据结构AVFrame)的time_base为AVRational{1,25}，这个是视频层的time_base，是帧率的倒数，定义在AVCodecContext结构体中。time_base的类型是AVRational，表示一个分数，例如AVRational{1,25}表示值为1/25(单位是秒)。

```
typedef struct AVStream {
    ......
    
    /**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented.
     *
     * decoding: set by libavformat
     * encoding: May be set by the caller before avformat_write_header() to
     *           provide a hint to the muxer about the desired timebase. In
     *           avformat_write_header(), the muxer will overwrite this field
     *           with the timebase that will actually be used for the timestamps
     *           written into the file (which may or may not be related to the
     *           user-provided one, depending on the format).
     */
    AVRational time_base;
    
    ......
}

typedef struct AVCodecContext {
    ......
    
    /**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. For fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identically 1.
     * This often, but not always is the inverse of the frame rate or field rate
     * for video. 1/time_base is not the average frame rate if the frame rate is not
     * constant.
     *
     * Like containers, elementary streams also can store timestamps, 1/time_base
     * is the unit in which these timestamps are specified.
     * As example of such codec time base see ISO/IEC 14496-2:2001(E)
     * vop_time_increment_resolution and fixed_vop_rate
     * (fixed_vop_rate == 0 implies that it is different from the framerate)
     *
     * - encoding: MUST be set by user.
     * - decoding: the use of this field for decoding is deprecated.
     *             Use framerate instead.
     */
    AVRational time_base;
    
    ......
}

/**
 * Rational number (pair of numerator and denominator).
 */
typedef struct AVRational{
    int num; ///< Numerator
    int den; ///< Denominator
} AVRational;
```

time_base是一个分数，av_q2d(time_base)则可将分数转换为对应的double类型数。因此有如下计算：

```
AVStream *st;
double duration_of_stream = st->duration * av_q2d(st->time_base);   // 视频流播放时长
double pts_of_frame = frame->pts * av_q2d(st->time_base);           // 视频帧显示时间戳
```

## 4.2 PTS/DTS/解码过程

DTS(Decoding Time Stamp, 解码时间戳)，表示压缩帧的解码时间。
PTS(Presentation Time Stamp, 显示时间戳)，表示将压缩帧解码后得到的原始帧的显示时间。
音频中DTS和PTS是相同的。视频中由于B帧需要双向预测，B帧依赖于其前和其后的帧，因此含B帧的视频解码顺序与显示顺序不同，即DTS与PTS不同。当然，不含B帧的视频，其DTS和PTS是相同的。下图以一个开放式GOP示意图为例，说明视频流的解码顺序和显示顺序
![图4 解码和显示顺序](https://leichn.github.io/img/avideo_basics/decode_order.jpg)
**采集顺序**指图像传感器采集原始信号得到图像帧的顺序。
**编码顺序**指编码器编码后图像帧的顺序。存储到磁盘的本地视频文件中图像帧的顺序与编码顺序相同。
**传输顺序**指编码后的流在网络中传输过程中图像帧的顺序。
**解码顺序**指解码器解码图像帧的顺序。
**显示顺序**指图像帧在显示器上显示的顺序。
**采集顺序与显示顺序相同。编码顺序、传输顺序和解码顺序相同。**
以图中“B[1]”帧为例进行说明，“B[1]”帧解码时需要参考“I[0]”帧和“P[3]”帧，因此“P[3]”帧必须比“B[1]”帧先解码。这就导致了解码顺序和显示顺序的不一致，后显示的帧需要先解码。

上述内容可参考“[视频编解码基础概念](https://www.cnblogs.com/leisure_chn/p/10285829.html)”。

理解了含B帧视频流解码顺序与显示顺序的不同，才容易理解解码函数decoder_decode_frame()中对视频解码的处理：
**avcodec_send_packet()按解码顺序发送packet。**
**avcodec_receive_frame()按显示顺序输出frame。**
这个过程由解码器处理，不需要用户程序费心。
decoder_decode_frame()是非常核心的一个函数，代码本身并不难理解。decoder_decode_frame()是一个通用函数，可以解码音频帧、视频帧和字幕帧，本节着重关注视频帧解码过程。音频帧解码过程在注释中。

```
// 从packet_queue中取一个packet，解码生成frame
static int decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub) {
    int ret = AVERROR(EAGAIN);

    for (;;) {
        AVPacket pkt;

        // 本函数被各解码线程(音频、视频、字幕)首次调用时，d->pkt_serial等于-1，d->queue->serial等于1
        if (d->queue->serial == d->pkt_serial) {
            do {
                if (d->queue->abort_request)
                    return -1;

                // 3. 从解码器接收frame
                switch (d->avctx->codec_type) {
                    case AVMEDIA_TYPE_VIDEO:
                        // 3.1 一个视频packet含一个视频frame
                        //     解码器缓存一定数量的packet后，才有解码后的frame输出
                        //     frame输出顺序是按pts的顺序，如IBBPBBP
                        //     frame->pkt_pos变量是此frame对应的packet在视频文件中的偏移地址，值同pkt.pos
                        ret = avcodec_receive_frame(d->avctx, frame);
                        if (ret >= 0) {
                            if (decoder_reorder_pts == -1) {
                                frame->pts = frame->best_effort_timestamp;
                            } else if (!decoder_reorder_pts) {
                                frame->pts = frame->pkt_dts;
                            }
                        }
                        break;
                    case AVMEDIA_TYPE_AUDIO:
                        // 3.2 一个音频packet含多个音频frame，每次avcodec_receive_frame()返回一个frame，此函数返回。
                        // 下次进来此函数，继续获取一个frame，直到avcodec_receive_frame()返回AVERROR(EAGAIN)，
                        // 表示解码器需要填入新的音频packet
                        ret = avcodec_receive_frame(d->avctx, frame);
                        if (ret >= 0) {
                            AVRational tb = (AVRational){1, frame->sample_rate};
                            if (frame->pts != AV_NOPTS_VALUE)
                                frame->pts = av_rescale_q(frame->pts, d->avctx->pkt_timebase, tb);
                            else if (d->next_pts != AV_NOPTS_VALUE)
                                frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
                            if (frame->pts != AV_NOPTS_VALUE) {
                                d->next_pts = frame->pts + frame->nb_samples;
                                d->next_pts_tb = tb;
                            }
                        }
                        break;
                }
                if (ret == AVERROR_EOF) {
                    d->finished = d->pkt_serial;
                    avcodec_flush_buffers(d->avctx);
                    return 0;
                }
                if (ret >= 0)
                    return 1;   // 成功解码得到一个视频帧或一个音频帧，则返回
            } while (ret != AVERROR(EAGAIN));
        }

        do {
            if (d->queue->nb_packets == 0)  // packet_queue为空则等待
                SDL_CondSignal(d->empty_queue_cond);
            if (d->packet_pending) {        // 有未处理的packet则先处理
                av_packet_move_ref(&pkt, &d->pkt);
                d->packet_pending = 0;
            } else {
                // 1. 取出一个packet。使用pkt对应的serial赋值给d->pkt_serial
                if (packet_queue_get(d->queue, &pkt, 1, &d->pkt_serial) < 0)
                    return -1;
            }
        } while (d->queue->serial != d->pkt_serial);

        // packet_queue中第一个总是flush_pkt。每次seek操作会插入flush_pkt，更新serial，开启新的播放序列
        if (pkt.data == flush_pkt.data) {
            // 复位解码器内部状态/刷新内部缓冲区。当seek操作或切换流时应调用此函数。
            avcodec_flush_buffers(d->avctx);
            d->finished = 0;
            d->next_pts = d->start_pts;
            d->next_pts_tb = d->start_pts_tb;
        } else {
            if (d->avctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
                int got_frame = 0;
                ret = avcodec_decode_subtitle2(d->avctx, sub, &got_frame, &pkt);
                if (ret < 0) {
                    ret = AVERROR(EAGAIN);
                } else {
                    if (got_frame && !pkt.data) {
                       d->packet_pending = 1;
                       av_packet_move_ref(&d->pkt, &pkt);
                    }
                    ret = got_frame ? 0 : (pkt.data ? AVERROR(EAGAIN) : AVERROR_EOF);
                }
            } else {
                // 2. 将packet发送给解码器
                //    发送packet的顺序是按dts递增的顺序，如IPBBPBB
                //    pkt.pos变量可以标识当前packet在视频文件中的地址偏移
                if (avcodec_send_packet(d->avctx, &pkt) == AVERROR(EAGAIN)) {
                    av_log(d->avctx, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
                    d->packet_pending = 1;
                    av_packet_move_ref(&d->pkt, &pkt);
                }
            }
            av_packet_unref(&pkt);
        }
    }
}
```

本函数实现如下功能：
[1]. 从视频packet队列中取一个packet
[2]. 将取得的packet发送给解码器
[3]. 从解码器接收解码后的frame，此frame作为函数的输出参数供上级函数处理

注意如下几点：
[1]. 含B帧的视频文件，其视频帧存储顺序与显示顺序不同
[2]. 解码器的输入是packet队列，视频帧解码顺序与存储顺序相同，是按dts递增的顺序。dts是解码时间戳，因此存储顺序解码顺序都是dts递增的顺序。avcodec_send_packet()就是将视频文件中的packet序列依次发送给解码器。发送packet的顺序如IPBBPBB。
[3]. 解码器的输出是frame队列，frame输出顺序是按pts递增的顺序。pts是解码时间戳。pts与dts不一致的问题由解码器进行了处理，用户程序不必关心。从解码器接收frame的顺序如IBBPBBP。
[4]. 解码器中会缓存一定数量的帧，一个新的解码动作启动后，向解码器送入好几个packet解码器才会输出第一个packet，这比较容易理解，因为解码时帧之间有信赖关系，例如IPB三个帧被送入解码器后，B帧解码需要依赖I帧和P帧，所在在B帧输出前，I帧和P帧必须存在于解码器中而不能删除。理解了这一点，后面视频frame队列中对视频帧的显示和删除机制才容易理解。
[5]. 解码器中缓存的帧可以通过冲洗(flush)解码器取出。冲洗(flush)解码器的方法就是调用avcodec_send_packet(..., NULL)，然后多次调用avcodec_receive_frame()将缓存帧取尽。缓存帧取完后，avcodec_receive_frame()返回AVERROR_EOF。ffplay中，是通过向解码器发送flush_pkt(实际为NULL)，每次seek操作都会向解码器发送flush_pkt。

如何确定解码器的输出frame与输入packet的对应关系呢？可以对比frame->pkt_pos和pkt.pos的值，这两个值表示packet在视频文件中的偏移地址，如果这两个变量值相等，表示此frame来自此packet。调试跟踪这两个变量值，即能发现解码器输入帧与输出帧的关系。为简便，就不贴图了。

## 4.3 视频同步到音频

视频同步到音频是ffplay的默认同步方式。在视频播放线程中实现。视频播放函数video_refresh()实现了视频显示(包含同步控制)，是非常核心的一个函数，理解起来也有些难度。这个函数的调用过程如下：

```
main() -->
event_loop() -->
refresh_loop_wait_event() -->
video_refresh()
```

函数实现如下：

```
/* called to display each frame */
static void video_refresh(void *opaque, double *remaining_time)
{
    VideoState *is = opaque;
    double time;

    Frame *sp, *sp2;

    if (!is->paused && get_master_sync_type(is) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
        check_external_clock_speed(is);

    // 音频波形图显示
    if (!display_disable && is->show_mode != SHOW_MODE_VIDEO && is->audio_st) {
        time = av_gettime_relative() / 1000000.0;
        if (is->force_refresh || is->last_vis_time + rdftspeed < time) {
            video_display(is);
            is->last_vis_time = time;
        }
        *remaining_time = FFMIN(*remaining_time, is->last_vis_time + rdftspeed - time);
    }

    // 视频播放
    if (is->video_st) {
retry:
        if (frame_queue_nb_remaining(&is->pictq) == 0) {    // 所有帧已显示
            // nothing to do, no picture to display in the queue
        } else {                                            // 有未显示帧
            double last_duration, duration, delay;
            Frame *vp, *lastvp;

            /* dequeue the picture */
            lastvp = frame_queue_peek_last(&is->pictq);     // 上一帧：上次已显示的帧
            vp = frame_queue_peek(&is->pictq);              // 当前帧：当前待显示的帧

            if (vp->serial != is->videoq.serial) {
                frame_queue_next(&is->pictq);
                goto retry;
            }

            // lastvp和vp不是同一播放序列(一个seek会开始一个新播放序列)，将frame_timer更新为当前时间
            if (lastvp->serial != vp->serial)
                is->frame_timer = av_gettime_relative() / 1000000.0;

            // 暂停处理：不停播放上一帧图像
            if (is->paused)
                goto display;

            /* compute nominal last_duration */
            last_duration = vp_duration(is, lastvp, vp);        // 上一帧播放时长：vp->pts - lastvp->pts
            delay = compute_target_delay(last_duration, is);    // 根据视频时钟和同步时钟的差值，计算delay值

            time= av_gettime_relative()/1000000.0;
            // 当前帧播放时刻(is->frame_timer+delay)大于当前时刻(time)，表示播放时刻未到
            if (time < is->frame_timer + delay) {
                // 播放时刻未到，则更新刷新时间remaining_time为当前时刻到下一播放时刻的时间差
                *remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
                // 播放时刻未到，则不更新rindex，把上一帧再lastvp再播放一遍
                goto display;
            }

            // 更新frame_timer值
            is->frame_timer += delay;
            // 校正frame_timer值：若frame_timer落后于当前系统时间太久(超过最大同步域值)，则更新为当前系统时间
            if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
                is->frame_timer = time;

            SDL_LockMutex(is->pictq.mutex);
            if (!isnan(vp->pts))
                update_video_pts(is, vp->pts, vp->pos, vp->serial); // 更新视频时钟：时间戳、时钟时间
            SDL_UnlockMutex(is->pictq.mutex);

            // 是否要丢弃未能及时播放的视频帧
            if (frame_queue_nb_remaining(&is->pictq) > 1) {         // 队列中未显示帧数>1(只有一帧则不考虑丢帧)
                Frame *nextvp = frame_queue_peek_next(&is->pictq);  // 下一帧：下一待显示的帧
                duration = vp_duration(is, vp, nextvp);             // 当前帧vp播放时长 = nextvp->pts - vp->pts
                // 1. 非步进模式；2. 丢帧策略生效；3. 当前帧vp未能及时播放，即下一帧播放时刻(is->frame_timer+duration)小于当前系统时刻(time)
                if(!is->step && (framedrop>0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration){
                    is->frame_drops_late++;         // framedrop丢帧处理有两处：1) packet入队列前，2) frame未及时显示(此处)
                    frame_queue_next(&is->pictq);   // 删除上一帧已显示帧，即删除lastvp，读指针加1(从lastvp更新到vp)
                    goto retry;
                }
            }

            // 字幕播放
            ......

            // 删除当前读指针元素，读指针+1。若未丢帧，读指针从lastvp更新到vp；若有丢帧，读指针从vp更新到nextvp
            frame_queue_next(&is->pictq);
            is->force_refresh = 1;

            if (is->step && !is->paused)
                stream_toggle_pause(is);
        }
display:
        /* display picture */
        if (!display_disable && is->force_refresh && is->show_mode == SHOW_MODE_VIDEO && is->pictq.rindex_shown)
            video_display(is);                      // 取出当前帧vp(若有丢帧是nextvp)进行播放
    }
    is->force_refresh = 0;
    if (show_status) {                              // 更新显示播放状态
        ......
    }
}
```

视频同步到音频的基本方法是：**如果视频超前音频，则不进行播放，以等待音频；如果视频落后音频，则丢弃当前帧直接播放下一帧，以追赶音频。**
此函数执行流程参考如下流程图：
![video_refresh()流程图](https://leichn.github.io/img/ffplay_analysis/video_refresh_flow.jpg)

步骤如下：
[1] 根据上一帧lastvp的播放时长duration，校正等到delay值，duration是上一帧理想播放时长，delay是上一帧实际播放时长，根据delay值可以计算得到当前帧的播放时刻
[2] 如果当前帧vp播放时刻未到，则继续显示上一帧lastvp，并将延时值remaining_time作为输出参数供上级调用函数处理
[3] 如果当前帧vp播放时刻已到，则立即显示当前帧，并更新读指针

在video_refresh()函数中，调用了compute_target_delay()来根据视频时钟与主时钟的差异来调节delay值，从而调节视频帧播放的时刻。

```
// 根据视频时钟与同步时钟(如音频时钟)的差值，校正delay值，使视频时钟追赶或等待同步时钟
// 输入参数delay是上一帧播放时长，即上一帧播放后应延时多长时间后再播放当前帧，通过调节此值来调节当前帧播放快慢
// 返回值delay是将输入参数delay经校正后得到的值
static double compute_target_delay(double delay, VideoState *is)
{
    double sync_threshold, diff = 0;

    /* update delay to follow master synchronisation source */
    if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
        /* if video is slave, we try to correct big delays by
           duplicating or deleting a frame */
        // 视频时钟与同步时钟(如音频时钟)的差异，时钟值是上一帧pts值(实为：上一帧pts + 上一帧至今流逝的时间差)
        diff = get_clock(&is->vidclk) - get_master_clock(is);
        // delay是上一帧播放时长：当前帧(待播放的帧)播放时间与上一帧播放时间差理论值
        // diff是视频时钟与同步时钟的差值

        /* skip or repeat frame. We take into account the
           delay to compute the threshold. I still don't know
           if it is the best guess */
        // 若delay < AV_SYNC_THRESHOLD_MIN，则同步域值为AV_SYNC_THRESHOLD_MIN
        // 若delay > AV_SYNC_THRESHOLD_MAX，则同步域值为AV_SYNC_THRESHOLD_MAX
        // 若AV_SYNC_THRESHOLD_MIN < delay < AV_SYNC_THRESHOLD_MAX，则同步域值为delay
        sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
        if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
            if (diff <= -sync_threshold)        // 视频时钟落后于同步时钟，且超过同步域值
                delay = FFMAX(0, delay + diff); // 当前帧播放时刻落后于同步时钟(delay+diff<0)则delay=0(视频追赶，立即播放)，否则delay=delay+diff
            else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)  // 视频时钟超前于同步时钟，且超过同步域值，但上一帧播放时长超长
                delay = delay + diff;           // 仅仅校正为delay=delay+diff，主要是AV_SYNC_FRAMEDUP_THRESHOLD参数的作用，不作同步补偿
            else if (diff >= sync_threshold)    // 视频时钟超前于同步时钟，且超过同步域值
                delay = 2 * delay;              // 视频播放要放慢脚步，delay扩大至2倍
        }
    }

    av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
            delay, -diff);

    return delay;
}
```

compute_target_delay()的输入参数delay是上一帧理想播放时长duration，返回值delay是经校正后的上一帧实际播放时长。为方便描述，下面我们将输入参数记作duration(对应函数的输入参数delay)，返回值记作delay(对应函数返回值delay)。
本函数实现功能如下：
[1] 计算视频时钟与音频时钟(主时钟)的偏差diff，实际就是视频上一帧pts减去音频上一帧pts。所谓上一帧，就是已经播放的最后一帧，上一帧的pts可以标识视频流/音频流的播放时刻(进度)。
[2] 计算同步域值sync_threshold，同步域值的作用是：若视频时钟与音频时钟差异值小于同步域值，则认为音视频是同步的，不校正delay；若差异值大于同步域值，则认为音视频不同步，需要校正delay值。
同步域值的计算方法如下：
若duration < AV_SYNC_THRESHOLD_MIN，则同步域值为AV_SYNC_THRESHOLD_MIN
若duration > AV_SYNC_THRESHOLD_MAX，则同步域值为AV_SYNC_THRESHOLD_MAX
若AV_SYNC_THRESHOLD_MIN < duration < AV_SYNC_THRESHOLD_MAX，则同步域值为duration
[3] delay校正策略如下：

a) 视频时钟落后于同步时钟且落后值超过同步域值：
a1) 若当前帧播放时刻落后于同步时钟(delay+diff<0)则delay=0(视频追赶，立即播放)；
a2) 否则delay=duration+diff

b) 视频时钟超前于同步时钟且超过同步域值：
b1) 上一帧播放时长过长(超过最大值)，仅校正为delay=duration+diff；
b2) 否则delay=duration×2，视频播放放慢脚步，等待音频

c) 视频时钟与音频时钟的差异在同步域值内，表明音视频处于同步状态，不校正delay，则delay=duration

对上述视频同步到音频的过程作一个总结，参考下图：

![ffplay音视频同步示意图](https://leichn.github.io/img/ffplay_analysis/ffplay_avsync_illustrate.jpg)

图中，小黑圆圈是代表帧的实际播放时刻，小红圆圈代表帧的理论播放时刻，小绿方块表示当前系统时间(当前时刻)，小红方块表示位于不同区间的时间点，则当前时刻处于不同区间时，视频同步策略为：
[1] 当前时刻在T0位置，则重复播放上一帧，延时remaining_time后再播放当前帧
[2] 当前时刻在T1位置，则立即播放当前帧
[3] 当前时刻在T2位置，则忽略当前帧，立即显示下一帧，加速视频追赶
上述内容是为了方便理解进行的简单而形象的描述。实际过程要计算相关值，根据compute_target_delay()和video_refresh()中的策略来控制播放过程。

## 4.4 音频同步到视频

音频同步到视频的方式，在音频播放线程中，实现代码在audio_decode_frame()及synchronize_audio()中。
函数调用关系如下：

```
sdl_audio_callback() -->
audio_decode_frame() -->
synchronize_audio()
```

以后有时间再补充分析过程。

## 4.5 音视频同步到外部时钟

略