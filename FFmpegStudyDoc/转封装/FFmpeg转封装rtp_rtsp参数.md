## FFmpeg rtsp封装协议

- rtp 

  -  url格式

    rtp://hostname[:port][?option=val...]

  - 支持选项参数：参见libavformat/rtpproto.c中定义的AVOption options[]

    - ttl = n

      Set the TTL (Time-To-Live) value (for multicast only).

    - rtcpport=n

      Set the remote RTCP port to n.

    - localrtpport=n

      Set the local RTP port to n.

    - localrtcpport=n

      Set the local RTCP port to n.

    - pk_size=n

      Set max packet size (in bytes) to n.

    - connect=0|1

      Do a `connect()` on the UDP socket (if set to 1) or not (if set to 0).

    - source=ip[.ip]

      List allowed source IP addresses.

    - block=ip[.ip]

      List disallowed (blocked) source IP addresses.

    - write_to_source=0|1

      Send packets to the source address of the latest received packet (if set to 1) or to a default remote address (if set to 0).

    - localport=n

      Set the local RTP port to n.

      This is a deprecated option. Instead, localrtpport should be used.

  - ffmpeg命令行示例

    - 推流

      ./ffmpeg -re -i /home/kelinlang/workspace/testFiles/1080P.h265 -vcodec copy -f rtp rtp://127.0.0.1:1234>test_rtp_h264.sdp

      ./ffmpeg -re -i /home/kelinlang/workspace/testFiles/1080P.h265 -c copy -f rtp rtp:*//127.0.0.1:1234*

    - 播放

      ./ffplay -protocol_whitelist "file,udp,rtp" -i rtp:*//127.0.0.1:1234*

      ./ffplay -protocol_whitelist "file,udp,rtp" -i test_rtp_h264.sdp





- **RTSP URL的语法结构**

  以“rtsp”或是“rtspu”开始的URL链接用于指定当前使用的是RTSP 协议。RTSP URL的语法结构如下：

  ​	rtsp_url = (“rtsp:”| “rtspu:”) “//” host [“:”port”] /[abs_path]/content_name

  **host**：可以是一个有效的域名或是IP地址。

  **port：**端口号，对于RTSP协议来说，缺省的端口号为554。当我们在确认流媒体服务器提供的端口号为554时，此项可以省略 说明：当HMS服务器使用的端口号为554时，我们在写点播链接时，可以不用写明端口号，但当使用非554端口时，在RTSP URL中一定要指定相应的端口。

  **abs_path:** 为RTSPServer中的媒体流资源标识，见下章节的录像资源命名。 

  RTSPURL用来标识RTSPServer的媒体流资源，可以标识单一的媒体流资源，也可以标 识多个媒体流资源的集合。 

- **ffmpeg中rtsp参数，参见libavformat/rtsp.c中定义的AVOption ff_rtsp_options[]：**

  - initial_pause：不要立马开始播放流
  - rtsp_transport：媒体传输方式
    - udp
    - tcp
    - udp_multicast
    - http
    - https
  - rtsp_flags
    - listen
    - prefer_tcp
  - 端口号
  - listen_timeout ：拉流时监听输入连接超时时间，时间单位秒
  - timeout/stimeout ：socket tcp I/O超时时间，时间单位微妙

- **ffmpeg命令行示例**

  - 推流

    ./ffmpeg -re  -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f rtsp rtsp://192.168.2.199:5555/live/test

  - tcp方式传输流媒体

    ./ffplay -rtsp_transport tcp rtsp://192.168.2.199:5555/live/test

  - 设置超时时间

    ./ffplay -stimeout 30000000 -rtsp_transport tcp rtsp://192.168.2.199:5555/live/test

    目前测试超时时间无效，推流端中断后，播放端播放程序未异常退出

- **参考文档**

  [RTSP协议详解](https://blog.51cto.com/machh85/1827154)

  [FFmpeg官方文档](https://www.ffmpeg.org/ffmpeg-protocols.html#rtsp)

  ffmpeg最新源码