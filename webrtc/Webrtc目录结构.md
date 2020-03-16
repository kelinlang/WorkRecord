# 目录结构说明

```
在本节中将介绍WebRTC的目录结构以及各个目录的作用
通过阅读本节将会在未来需要修改源代码时为你提供帮助
```

## 外层目录结构

**1.api**
接口层，外部通过调用本层来使用WebRTC的核心功能

**2.call**
管理层，通过本层可以对数据流进行管理

**3.video**
视频相关的逻辑，包括处理、编解码等

**4.audio**
音频相关的逻辑

**5.common_audio**
音频相关的算法目录

**6.common_video**
视频相关的算法目录

**7.media**
多媒体相关的逻辑，包括编码、解码的逻辑处理

**8.logging**
日志目录

**9.module**
模块目录，包含众多模块，具有可扩展性，在后文中具体介绍

**10.pc**
连接层，Peer Connection，包含连接相关的逻辑和轨的逻辑
通过本层可以拿到视频、音频信息等

**11.p2p**
包含端对端的相关逻辑

**12.rtc_base**
基础代码，包含线程、锁等相关的统一接口代码
可以统一处理多端、多平台的差异等

**13.rtc_tool**
工具目录，包含音频、视频等分析工具

**14.tool_webrtc**
WebRTC相关测试工具目录，如网络测试、音视频测试等

**15.system_wrappers**
与操作系统、环境相关的逻辑代码，如cpu、系统差异等

**16.stats**
统计数据目录，包含丢包率、网络抖动等统计数据

**17.sdk**
存放Android和IOS层代码，包含音视频采集和渲染等

## Modules目录结构

**1.audio_coding**
音频编解码器

**2.audio_device**
音频采集、播放模块

**3.audio.mixer**
混音模块，如多人互动音频传输

**4,audio_processing**
音频前处理和后处理模块，如回音消除、降噪、增益等

**5.bitrate_controller**
码率控制模块

**6.congestion_controller**
流控制模块，如在网络流量高时的具体处理

**7.desktop_capture**
桌面采集模块

**8.pacing**
码率探测器及平滑处理模块，

**9.remote_bitrate_estimator**
远端码率探测器

**10.rtp_rtcp**
rtp/rtcp协议相关模块

**11.video_capture**
视频采集模块

**12.video_coding**
视频编解码模块，如h264等

**13.video_processing**
视频前处理和后处理模块