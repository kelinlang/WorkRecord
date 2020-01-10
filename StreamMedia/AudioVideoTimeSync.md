# 音视频同步

## 1.基础知识

- **I/B/P帧**

  - **I帧** ：关键帧，帧内压缩技术
  - **B帧**：前后参考帧，它属由帧间压缩技术。也就是说在压缩成 B帧前，它会参考它前面的非压缩视频帧，和后面的非压缩的视频帧，记录下前后两帧都不存放的“残差值”，这样可以达到更好的压缩率；
  - **P帧**：向前参考帧，也就是它参考的是前一个关键帧的数据。P帧也属于帧间压缩技术，相对于 B帧来说，P帧的压缩率要比B帧低。

  ​       实时互动直播系统中，很少使用B帧。主要的原因是压缩和解码B帧时，由于要双向参考，所以它需要缓冲更多的数据，且使用的CPU也会更高。由于实时性的要求，所以一般不使用它。不过对于播放器来说，遇到带有B帧的H264数据是常有的事儿。

- **PTS/DTS**

  - **PTS** （Presentation TimeStamp）是渲染用的时间戳
  - **DTS**（Decoding TimeStamp）解码时间戳，是用于视频解码的。

- **时间基**

  - 概念

    - **时间刻度。我们以帧率为例，如果每秒钟的帧率是 25帧，那么它的时间基（时间刻度）就是 1/25。也就是说每隔1/25 秒后，显示一帧。**

  - 显示时将PTS转成以秒未单位的时间

  - FFmpeg/ffplay中时间参数

    - **tbr:** 是我们通常所说的帧率。**time base of rate**
    - **tbn:** 视频流的时间基。 **time base of stream**
    - **tbc: **视频解码的时间基。**time base of codec**
  
  ##  2.**FFmpeg内部时间基**
  
  - **AV_TIME_BASE** **1000000** ：毫秒单位 转换为秒：**1/1000000**
  
    ```
    #define         AV_TIME_BASE   1000000
    ```
  
  -  **分数所表式法：**
  
    ```
    #define         AV_TIME_BASE_Q   (AVRational){1, AV_TIME_BASE}
    ```
    
  - 在 ffmpeg中进行换算，将不同时间基的值转成按秒为单位的值计算如下：
  
    ```
    timestamp(秒) = pts * av_q2d(time_base)
    ```
  
  - 这里引入了 av_q2d 这个函数，它的定义非常简单：
  
    ```
    typedef struct AVRational{
    int num; //numerator
    int den; //denominator
    } AVRational;
    
    static inline double av_q2d(AVRational a)｛
    /**
    * Convert rational to double.
    * @param a rational to convert
    **/
        return a.num / (double) a.den;
    }
    ```
  
- **不同时间基换算**

  -  FFmpeg中转换函数

    ```
    av_rescale_q()
    ```

    `av_rescale_q(a,b,c)` 的作用是，把时间戳从一个时基调整到另外一个时基时候用的函数。其中，a 表式要换算的值；b 表式原来的时间基；c表式要转换的时间基。其计算公式为 `a * b / c`。看似简单的方法，方法内部还要考虑数值溢出的问题

  -  计算公式
  
     -  时间戳转秒
  
        ```
        time_in_seconds = av_q2d(AV_TIME_BASE_Q) * timestamp
        ```
  
     - 秒转时间戳
  
       ```
       timestamp = AV_TIME_BASE * time_in_seconds
       ```
  
  
  - av_packet_rescale_ts() ：将AVPacket中各种时间值从一种时间基转换为另一种时间基
  
    ```javascript
    /**
     * Convert valid timing fields (timestamps / durations) in a packet from one
     * timebase to another. Timestamps with unknown values (AV_NOPTS_VALUE) will be
     * ignored.
     *
     * @param pkt packet on which the conversion will be performed
     * @param tb_src source timebase, in which the timing fields in pkt are
     *               expressed
     * @param tb_dst destination timebase, to which the timing fields will be
     *               converted
     */
    void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);
    ```

## 3.转封装过程中时间基转换

容器中的时间基(AVStream.time_base，3.2节中的tbn)定义如下：

```javascript
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
```

AVStream.time_base是AVPacket中pts和dts的时间单位，输入流与输出流中time_base按如下方式确定： 对于输入流：打开输入文件后，调用avformat_find_stream_info()可获取到每个流中的time_base 对于输出流：打开输出文件后，调用avformat_write_header()可根据输出文件封装格式确定每个流的time_base并写入输出文件中

不同封装格式具有不同的时间基，在转封装(将一种封装格式转换为另一种封装格式)过程中，时间基转换相关代码如下：

```javascript
av_read_frame(ifmt_ctx, &pkt);
pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
```

下面的代码具有和上面代码相同的效果：

```javascript
// 从输入文件中读取packet
av_read_frame(ifmt_ctx, &pkt);
// 将packet中的各时间值从输入流封装格式时间基转换到输出流封装格式时间基
av_packet_rescale_ts(&pkt, in_stream->time_base, out_stream->time_base);
```

这里流里的时间基`in_stream->time_base`和`out_stream->time_base`，是容器中的时间基

## 4. 转码过程中的时间基转换

