# RTMP协议

**RTMP**是Real Time Messaging Protocol（实时消息传输协议）的首字母缩写。是Adobe公司开发的一个基于TCP的应用层协议，也就是说，RTMP是和HTTP/HTTPS一样，是应用层的一个协议族。**RTMP在TCP通道上一般传输的是flv 格式流**。请注意，RTMP是网络传输协议，而flv则是视频的封装格式。flv封装格式设计出来的目的是为了用于网络传输使用的，因此RTMP+FLV可以说是”黄金搭档“。

RTMP协议包括：基本协议及RTMPT/RTMPS/RTMPE等多种变种。

1. RTMP工作在TCP之上，默认使用端口1935，这个是基本形态；
2. RTMPE在RTMP的基础上增加了加密功能；
3. RTMPT封装在HTTP请求之上，可穿透防火墙；
4. RTMPS类似RTMPT，增加了TLS/SSL的安全功能；
5. RTMFP使用UDP进行传输的RTMP；

RTMP就是专门针对多媒体数据流的实时通信设计出来的一种网络数据传输协议，主要用来在Flash/AIR平台和支持RTMP协议的流媒体/交互服务器之间进行音视频和数据通信。现在Adobe公司已经不支持了，导致rtmp、flv协议停留在h264编码时代，近些年h265编码火起来之后rtmp和flv协议是无法支持它的，为了支持rtmp和flv编码国内几个CDN厂商约定将flv中codecId为 **12（十六进制 0x0c）** 时代表h265编码。

参考链接：

https://cloud.tencent.com/developer/article/1004970



协议参数

FFmpeg针对RTMP协议有下面参数：

libavformat/rtmpproto.c librtmp.c

| **参数**       | 类型       | **说明**                                                     |
| -------------- | ---------- | ------------------------------------------------------------ |
| rtmp_app       | 字符串     | RTMP 流发布点，又称 APP                                      |
| rtmp_buffer    | 整数       | 客户端 buffer 大小（单位：毫秒），默认为 3 秒 （3000）       |
| rtmp_conn      | 字符串     | 在 RTMP 的 Connect 命令中增加自定义 AMF 数据                 |
| rtmp_flashver  | 字符串     | 设置模拟的 flashplugin 的版本号                              |
| rtmp_live      | 整数       | 指定 RTMP 流媒体播放类型，具体如下：<br/>any：直播或点播随意<br/>live：直播<br/>recorded：点播 |
| rtmp_pageurl   | 字符串     | RTMP 在 Connect 命令中设置的 PageURL 字段，其为播放时所在的 Web 页面 URL |
| rtmp_playpath  | 字符串     | RTMP 流播放的 Stream 地址，或者称为密钥，或者称为发布流      |
| rtmp_subscribe | 字符串     | 直播流名称，默认设置为 rtmp_playpath 的值                    |
| rtmp_swfhash   | 二进制数据 | 解压 swf 文件后的 SHA256 的 hash 值                          |
| rtmp_swfsize   | 整数       | swf 文件解压后的大小，用于 swf 认证                          |
| rtmp_swfurl    | 字符串     | RTMP 的 Connect 命令中设置的 swfURL 播放器的 URL             |
| rtmp_swfverify | 字符串     | 设置 swf 认证时 swf 文件的 URL 地址                          |
| rtmp_tcurl     | 字符串     | RTMP 的 Connect 命令中设置的 tcURL 目标发布点地址，一般形如 rtmp://xxx.xxx.xxx/app |
| rtmp_listen    | 整数       | 开启 RTMP 服务时所监听的端口                                 |
| listen         | 整数       | 与 rtmp_listen 相同                                          |
| timeout        | 整数       | 监听 rtmp 端口时设置的超时时间，以秒为单位                   |

使用rtmp_app、rtmp_playpath参数示例：

```
ffmpeg -re -i test.mp4 -c copy -f flv -rtmp_app live -rtmp_playpath steam rtmp://live.pingos.io
```


等价于

```
ffmpeg -re -i test.mp4 -c copy -f flv rtmp://live.pingos.io/live/stream
```



**给rtmp地址添加参数**

一般的推流和拉流地址长这样，rtmp://xxx.xxx.xxx.xxx/app/streamname

但是很多时候我们需要服务器做一些权限验证，就要求rtmp连接时携带token，我们就可以通过以下两种方式将token带给服务器。

```
ffmpeg -re -i test.mp4 -c copy -f flv -rtmp_app live -rtmp_playpath "steam?token=xxx" rtmp://live.pingos.io
```

```
ffmpeg -re -i test.mp4 -c copy -f flv "rtmp://live.pingos.io/live/stream?token=xxx"
```

参考链接：

https://blog.csdn.net/impingo/article/details/104163365



# 推流指令

## 文件推流

```
ffmpeg -re -i localFile.mp4 -c copy -f flv rtmp://server/live/streamName 
```

-re 以本地帧频读数据，主要用于模拟捕获设备。表示ffmpeg将按照帧率发送数据，不会按照最高的效率发送。



## rtsp转推

```
ffmpeg -re -rtsp_transport tcp -i rtsp://x.x.x.x/ch01 -c copy -f flv rtmp://server/live/streamName
```

-rtsp_transport tcp 以TCP方式连接rtsp服务器获取视频流



## rtmp转推

```
ffmpeg -re -i rtmp://URL1 -c copy -f flv rtmp://URL2
```





