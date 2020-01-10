## PTS的由来

音视频同步依赖的一个东西就是**pts（persentation time stamp ）显示时间戳** 告诉我们该什么时间显示这一帧 ,那么，这个东西是从哪里来的呢？

刨根问底栏目组将带你深度挖掘

PTS是在拍摄的时候打进去的时间戳,假如我们现在拍摄一段小视频(别想歪啊)，什么特效都不加，那么走的就是以下的步骤



![img](https://user-gold-cdn.xitu.io/2019/4/10/16a05a146e4de5e3?imageView2/0/w/1280/h/960/format/webp/ignore-error/1)



我们根据这个图可以知道，PTS是在录制的时候就打进Frame里的

## 音视频同步的方式

在ffplay中 音视频同步有三种方式

1. 以视频为基准，同步音频到视频
   - 音频慢了就加快音频的播放速度，或者直接丢掉一部分音频帧
   - 音频快了就放慢音频的播放速度
2. 以音频为基准，同步视频到音频
   - 视频慢了则加快播放或丢掉部分视频帧
   - 视频快了则延迟播放
3. 以外部时钟为准，同步音频和视频到外部时钟
   - 根据外部时钟改版音频和视频的播放速度

### 视频基准

如果以视频为基准进行同步，那么我们就要考虑可能出现的情况，比如：

**掉帧**

此时的音频应该怎么做呢？通常的方法有

1. 音频也丢掉相应的音频帧(会有断音，比如你说了一句，我的天啊，好漂亮的草原啊  很不凑巧丢了几帧视频，就成，，，卧槽！)
2. 音频加快速度播放(此处可以直接用Audition加快个几倍速的播放一首音乐)

### 音频基准

如果以音频为基准进行同步，很不幸的碰到了掉帧的情况，那么视频应该怎么做呢？通常也有两种做法

1.视频丢帧 （画面跳帧,丢的多的话，俗称卡成PPT） 2.加快播放速度(画面加快播放)

### 外部时钟为基准

假如以外部时钟为基准，如果音视频出现了丢帧，怎么办呢？

如果丢帧较多，直接重新初始化外部时钟 (pts和时钟进行对比，超过一定阈值重设外部时钟，比如1s)

## 音视频时间换算

### PTS 时间换算

之前我们稍微讲过pts的时间换算，pts换算成真正的秒是用以下操作

```
realTime = pts * av_q2d(stream.time_base)
```

stream是当前的视频/音频流

我们这里主要讲一下在音频解码pts可能会遇到的情况，有时候音频帧的pts会以1/采样率为单位，像

pts1 = 0 pts2 = 1024 pts3 = 2048

像我们例子中的这个视频，我们在解码一帧音频之后打印出来他的pts `std::cout<<"audio pts : "<pts<



![img](https://user-gold-cdn.xitu.io/2019/4/10/16a05a17a2eeabb6?imageView2/0/w/1280/h/960/format/webp/ignore-error/1)



我们知道当前视频的音频采样率为44100，那么这个音频帧pts的单位就是`1/44100`,那么

pts1 = 0 * 1 / 44100 = 0 pts2 = 1024 * 1 / 44100 = 0.232 pts3 = 2048 * 1 / 44100 = 0.464

音频流的time_base里面正是记录了这个值,我们可以通过debug来看一下



![img](https://user-gold-cdn.xitu.io/2019/4/10/16a05a1b819f22ff?imageView2/0/w/1280/h/960/format/webp/ignore-error/1)

利用 `realTime = pts * av_q2d(stream.time_base)` 我们可以直接算出来当前音频帧的pts





![img](https://user-gold-cdn.xitu.io/2019/4/10/16a05a1e510b6715?imageView2/0/w/1280/h/960/format/webp/ignore-error/1)



另外需要注意

**在ffplay中做音视频同步，都是以秒为单位**

### 音视频帧播放时间换算

#### 音频帧播放时间计算

音频帧的播放和音频的属性有关系，是采用

```
采样点数 * 1 / 采样率
```

来计算，像AAC当个通道采样是**1024**个采样点，那么

- 如果是44.1khz,那么一帧的播放时长就是  1024 * 1 / 44100 = 23.3**毫秒**
- 如果是48khz，那么一帧的播放时长就是  1024 * 1 / 48000 = 21.33**毫秒**

#### 视频帧的播放时间计算

视频帧的播放时间也有两个计算方式

1. **利用`1/帧率`获取每个帧平均播放时间**，这个方式有一个很大的缺点就是，不能动态响应视频帧的变化，比如说我们做一些快速慢速的特效，有的厂商或者SDK(我们的SDK不是)是直接改变视频帧的增加/减少视频帧之间的pts间距来实现的，这就导致在一些拿帧率计算显示时间的播放器上发现是整体（快/慢）了，达不到想要的效果；还有一种情况就是丢帧之后，时间显示仍然是固定的
2. **相邻帧相减** 这大程度上避免利用帧率去算的各种弊端，但是缺点是使用起来比较复杂，尤其是暂停/Seek之类的操作的时候需要进行一些时间差值的计算

## 时间校正

### 视频时间校正

在看`ffplay`的时候我们会发现，他在里面默认情况下是用了 `frame->pts = frame->best_effort_timestamp;` 其实大多数情况下`pts`和`best_effort_timestamp`的值是一样的,这个值是利用各种探索方法去计算当前帧的视频戳

### 音频时间校正

音频的pts获取比视频的要复杂一点,在ffplay中对音频的pts做了三次修改

1. `frame->pts = av_rescale_q(frame->pts, d->avctx->pkt_timebase, tb);` 将其由stream->time_base转为（1/采样率）（`decoder_decode_frame()`中）
2. `af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);` 将其由(1/采样率)转换为秒 (`audio_thread()`中)
3. `is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec` 根据实际输入进SDL2播放的数据长度做调整 （`sdl_audio_callback`中）

## ffplay 时钟框架

ffplay中的时钟框架主要依靠`Clock`结构体和相应的方法组成

```
/** 时钟结构体 **/
typedef struct Clock {
    double pts;           /* clock base 时间基准*/
    double pts_drift;     /* clock base minus time at which we updated the clock 时间基减去更新时钟的时间 */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/** 初始化时钟 **/
static void init_clock(Clock *c, int *queue_serial);

/** 获取当前时钟 **/
static double get_clock(Clock *c);

/** 设置时钟  内部调用set_clock_at()**/
static void set_clock(Clock *c, double pts, int serial);

/** 设置时钟 **/
static void set_clock_at(Clock *c, double pts, int serial, double time);

/** 设置时钟速度 **/
static void set_clock_speed(Clock *c, double speed);

/** 音/视频设置时钟的时候都回去跟外部时钟进行对比，防止丢帧或者丢包情况下时间差距比较大而进行的纠偏 **/
static void sync_clock_to_slave(Clock *c, Clock *slave);

/** 获取做为基准的类型  音频 外部时钟 视频 **/
static int get_master_sync_type(VideoState *is);

/** 获取主时间轴的时间 **/
static double get_master_clock(VideoState *is);

/** 检查外部时钟的速度 **/
static void check_external_clock_speed(VideoState *is);
复制代码
```

这个时钟框架也是比较简单,可以直接去看FFplay的源码，这里就不过多的叙述

## 音视频同步时间轴

在`ffplay`中,我们不管是以哪个方式做为基准，都是有一个时间轴



![img](https://user-gold-cdn.xitu.io/2019/4/10/16a05a28d58826c9?imageView2/0/w/1280/h/960/format/webp/ignore-error/1)



就像这样子，有一个时钟一直在跑，所谓基于音频、视频、外部时间 做为基准，也就是将那个轴的的时间做为时间轴的基准，另一个在轴参照主时间轴进行同步

假如是以音频为基准，视频同步音频的方式，那么就是音频在每播放一帧的时候，就去将当前的时间同步到时间轴，视频参考时间轴做调整

### 音频时钟设置

音频时钟的设置的话需要考虑注意  **硬件缓存数据**  设置音频时钟的时候需要将 `pts - 硬件缓冲数据的播放时间` 详情参考 ffplay 中 `sdl_audio_callback(void *opaque, Uint8 *stream, int len)`

```
set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
复制代码
```

这是就是将**音频的pts - 硬件缓冲区里剩下的时间**设置到了音频的时钟里

### 视频时钟设置

视频时钟设置的话就比较简单了，直接设置pts,在ffplay中 `queue_picture(VideoState *is, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)`内，我们可以直接看到 `vp->pts = pts;` ,然后在`video_refresh`里面`update_video_pts(is, vp->pts, vp->pos, vp->serial);`去调用了`set_clock`

```
static void update_video_pts(VideoState *is, double pts, int64_t pos, int serial) {
    /* update current video pts */
    set_clock(&is->vidclk, pts, serial);
    sync_clock_to_slave(&is->extclk, &is->vidclk);
}
复制代码
```

## 音视频同步操作

音视频在同步上出的处理我们上面有简单讲到过，我们这里来详细看一下他具体是真么做的

### 音频同步操作

音频的同步操作是在`audio_decode_frame()`中的`wanted_nb_samples = synchronize_audio(is, af->frame->nb_samples);`，注意`synchronize_audio` 方法，我们来看他注释

```
/* return the wanted number of samples to get better sync if sync_type is video
 * or external master clock
 *
 * 如果同步类型为视频或外部主时钟，则返回所需的采样数来更好的同步。
 *
 * */
static int synchronize_audio(VideoState *is, int nb_samples)
复制代码
```

这个方法里面的操作有点多，我这边简单说一下这个方法，主要是利用音频时钟与主时钟相减得到差值(需要先判断音频是不是主时间轴)，然后返回如果要同步需要的采样数，在`audio_decode_frame()`中用`len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);` 进行重采样,然后才在`sdl_audio_callback()`中进行播放

### 视频同步操作

视频同步操作的主要步骤是在`video_refresh()`方法中，我们来看一下关键的地方

```
   /* compute nominal last_duration 根据当前帧和上一帧的pts计算出来上一帧显示的持续时间 */
            last_duration = vp_duration(is, lastvp, vp);
            /** 计算当前帧需要显示的时间 **/
            delay = compute_target_delay(last_duration, is);

            /** 获取当前的时间 **/
            time= av_gettime_relative()/1000000.0;
            /** 如果当前时间小于显示时间 则直接进行显示**/
            if (time < is->frame_timer + delay) {
                *remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
                goto display;
            }

            /** 更新视频的基准时间 **/
            is->frame_timer += delay;
            /** 如果当前时间与基准时间偏差大于 AV_SYNC_THRESHOLD_MAX 则把视频基准时间设置为当前时间 **/
            if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
                is->frame_timer = time;

            /** 更新视频时间轴 **/
            SDL_LockMutex(is->pictq.mutex);
            if (!isnan(vp->pts))
                update_video_pts(is, vp->pts, vp->pos, vp->serial);
            SDL_UnlockMutex(is->pictq.mutex);

            /** 如果队列中有未显示的帧，如果开启了丢帧处理或者不是以视频为主时间轴，则进行丢帧处理 **/
            if (frame_queue_nb_remaining(&is->pictq) > 1) {
                Frame *nextvp = frame_queue_peek_next(&is->pictq);
                duration = vp_duration(is, vp, nextvp);
                if(!is->step && (framedrop>0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) && time > is->frame_timer + duration){
                    is->frame_drops_late++;
                    frame_queue_next(&is->pictq);
                    goto retry;
                }
            }

复制代码
```

到这里，ffplay中主要的音视频同步就讲完了，建议去看一下ffplay的源码，多体会体会 印象才会比较深刻，说实话ffplay中同步的操作是比较复杂的，我们在平常开发中要根据自己的实际业务进行一些简化和改进的，下一章我们就来写一个以音频为基准的视频播放器

