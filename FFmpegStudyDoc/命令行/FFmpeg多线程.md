# FFmpeg多线程



## 命令行多线程参数

新版本ffmpeg这个默认支持并启用了多线程。线程并不是越多越好，需要找到个平衡点。这就需要用time命令进行计时，并将该屏幕的输出重定向到文件，以便后期进行分析。

假设我这里有个in.mp4的视频文件，分辨率为640*480，码率为580kbps，需要降低分辨率为320*240，降低码率为290kbps（这个参数和分辨率同时使用时不一定起作用，特别是中途退出时）。

普通降低分辨率，码率的命令：

> ffmpeg-y-i in.mp4 -s 320x240 -b290000 out290.mp4



带thread参数的命令：   

> ffmpeg -y -threads 2 -i in.mp4 -s 320x240 -b 290000 out290.mp4

这里用了两个线程。



用time统计时间的命令：   

> time ffmpeg -y -threads 2 -i in.mp4 -s 320x240 -b 290000 out290.mp4



输出重定向命令：

> (time ffmpeg -y -threads 2 -i in.mp4 -s 320x240 -b 290000 out290.mp4) 2>1.txt

由于输出为标准出错，所以这里用2>1.txt



当然也可使用如下命令：

> (time ffmpeg -y -threads 2 -i in.mp4 -s 320x240 -b 290000 out290.mp4) >& 1.txt



为了同时在屏幕上输出，可以用tee命令。首先将标准出错重定向到标准输出，然后通过通道传给文件，命令如下：

> (time ffmpeg -y -threads 2 -i in.mp4 -s 320x240 out290.mp4) 2>&1 | tee 1.txt



如果想测试下服务器到底支持多少个线程比较好，就需要写个脚本运行，仅供参考：

```Shell
#! /bin/bash
echo "Input num : "
read num
for (( i=1; i<=$num; i=i+1 ))
do
    #(time ffmpeg -y -threads $i -i in.mp4 -s 320x240 -vcodec libx264 -vpre fast out290.mp4) >& $i.txt
    (time ffmpeg -y -threads $i -i in.mp4 -s 320x240 -vcodec libx264 -vpre fast out290.mp4) 2>&1 | tee $i.txt
done
```

参考链接：

https://www.cnblogs.com/MikeZhang/archive/2012/09/16/timeoutputredirect20120916.html



## FFmpeg指定x265编码器线程数

> ffmpeg -s 1920x1080 -framerate 25 -i input.yuv -c:v libx265 -x265-params pools=4 -y output.hevc

这里用到的是x265编码器的--pools命令行参数，它的含义是：

--pools <string>, --numa-pools <string>

```
Comma seperated list of threads per NUMA node. If “none”, then no worker pools are created and only frame parallelism is possible. If NULL or “” (default) x265 will use all available threads on each NUMA node ...
```

参考链接：

https://blog.csdn.net/chenyc09/article/details/81090235



## 并发解码

FFMPEG为了提高解码速度，可以使用多线程并发解码，分为线程级并发解码和片级并发编程。并发解码需要解决多帧依赖问题。如同时对I帧和P帧解码，P帧依赖于I帧，怎么办？FFMPEG采用算法如下：

1.每个线程在解码完一行宏块后，更新解码高度H1

2.B/P帧解码宏块时，需要把待解码宏块H2与H1比较，H2>=H1,线程阻塞等待。

3.每个线程更新解码高度是，阻塞线程会被唤醒比较高度。

从线程1到2，4，6解码时间是成倍缩小的，6，8，10再增加线程的话，解码时间就没有明显改进。

https://blog.csdn.net/hiwubihe/article/details/90292062



有调研报告：ffmpeg配线程数只针对解码，不针对编码。需要确认（TODO）

1 在多核的情况下，多线程解码比单线程解码用时要少。

2 线程并不是越多越好。

https://blog.csdn.net/evsqiezi/article/details/51604197



## 多线程相关代码

ffmpeg 内部avcodec_open2后， m_pVideoCodecContext->thread_count 的数量与cpu内核数相同。

所以如果编解码视频数量过多，而且cpu内核数也很大，很容易将整个进程线程数变得很大。而当一个进程线程数大于1000后，该进程将被系统自动杀死。

因此可以在调用avcodec_open2函数之前 ，手动设置thread_count个数。

Sample代码：

```
	m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!m_codec)
	{
		SDLOG_PRINTF("CX264_Decoder", SD_LOG_LEVEL_ERROR, "avcodec_find_decoder find h264 failed!!");
        return false;
    }

    m_ctx = avcodec_alloc_context3(m_codec);
	if (!m_ctx)
	{
		SDLOG_PRINTF("CX264_Decoder", SD_LOG_LEVEL_ERROR, "avcodec_alloc_context3 failed!!");
        return false;
    }
    m_picture = av_frame_alloc();

	m_ctx->width = 1920;
	m_ctx->height = 1080;
	m_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	m_ctx->thread_count = 8;		//指定解码线程数量
 
    if(m_codec->capabilities&AV_CODEC_CAP_TRUNCATED)
        m_ctx->flags|= AV_CODEC_FLAG_TRUNCATED;
        
	avcodec_open2(m_ctx, m_codec, NULL);
```



参考链接：

https://blog.csdn.net/sunxiaopengsun/article/details/53942505

