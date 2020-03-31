# 什么是JitterBuffer

本质就是一个缓冲区， 但是有限定词Jitter, 主要是用于处理网络抖动（什么是网络抖动见下一节的相关知识介绍）。 较常用JitterBuffer的一个场景就是音视频通讯（voip)， 在这个场景中，缓冲区中存储的是从网络中接收到的音视频数据，因此跟音视频的编解码以及网络传输强相关， 下一节专门介绍必须的前提知识，以使只有通用编程基础的人都能搞明白JitterBuffer的作用和原理。



# 前提知识

## audio相关知识

audio sample: 音频采样， 声音经过录音设备，由模拟信号转为数字信号， 将连续的数据转化为离散的数据。





![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-33ca7ffeebae3610.jpg)

653161-20190417105101524-891531685.jpg

比如，每秒做16000次采样， 采样率就是1.6k, 而每个采样点用多少位来存储，可以是8bit,也可以是16bit,甚至更多，精度会区别。



audio frame vs audio packet: 假如每采样一次，就将数据传输出去交给别人去播放，对1.6k的采样率，每1/16000秒就要走这一次这个流程，对于cpu和网络传输都很不友好。简单的解决办法就是， 多个连续的（比如每160个）采样点打包数据处理一次， 这样一块数据称为一帧， 即audio frame（也有将一个采样点作为一帧的）。 而对于向网络发送数据，往往要加上包头携带一些额外的信息，称为一个包， 即audio packet,一个网络包的大小是有限制的，如1500bytes。



音频编解码： 即音频数据的压缩和解压缩。



**有了上面的知识后，做这样的一个设想**：如果一帧音频数据很大， 即使压缩之后，也很大， 就需要分成多个网络包来发送， 那么在收到网络包之后，由于一个包不是完整的一帧， 一般情况就需要等待一帧中所有的包都收到之后才能解码， 从工程实现来说，怎么解决这个等待过程呢？ 简单的方法就是建立一个缓冲区，将网络包缓冲起来，直到能拼凑起完整的一帧。这就是JitterBuffer的作用之一了。



## video相关知识

video frame: 一帧视频数据， 可以简单理解为一张照片， 而视频是由多张照片组成的， 因此一段视频有很多video frames。
FPS: 每秒有多少帧， frames per second。
video resolution: 分辨率， 如480p(640x480）， 720p(1280x720), 1080p(1920x1080), 即一帧视频数据的长和宽。
video raw data format: 每一帧数据的格式（未压缩时）， 如RGB, YUV, 为了节省网络带宽和端上的处理时间，同时又兼顾人眼的视觉感受，往往采用yuv420, 即每帧视频采样点占1.5个字节存储。如下图（来源网络）：





![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-3d6c3b11bb930fca.png)

yuv420p.png

视频编码：即对视频流做压缩，如h264, h265， 即通过帧内预测，帧间预测，熵编码等技术来减少传输的数据量。编码后的帧，分I帧，P帧， B帧等， I帧是关键帧可单独解出一帧raw data, 而IDR帧即是I帧加上PPS, SPS等解码器需要的参数信息后的编码帧，可用于配置解码器和解出一帧图像，P帧需要参考前面的帧才能解码，B帧会参考前后的帧。 一个GOP即从一个IDR帧开始包到另一个IDR之前结束内的所有帧。想进一步了265视频编码技术的，可参考经典书籍： High Efficiency Video Coding (HEVC) Algorithms and Architectures



由一帧编码后的视频数据往往超过一个MTU大小（见下一节）， 会分成多个video packets, 如果网络出现丢包，如上图， 第3帧部分包丢失，则在收到数据时无法完整的组合这一帧， 因此解不出这一帧的图像，但不仅仅影响这一帖，第4，5帧同样解不出，因为这两帧参考了第3帧， 因此需要在jitter buffer中扔掉这些解不出的帧直到下一个IDR。





![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-977e17129c9ef2d3.png)

packets_loss.png

## 网络相关知识

voip为了追求低时延，往往选择udp协议来传输，而udp包的大小有限制，称作MTU, 一般1500bytes。
理想情况，由网络上发送1，2，3，4...个包，收到的也是1，2，3，4...个包，但由于网络是一个是非之地，有可能出现丢包和乱序，收到包可能就成了， 2，1，4...这样的。
所谓网络抖动，就是网络时延的变化， 如下：





![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-f806f464d5b0cd11.png)

audio_pipeline_in_voip-Page-2.png

# audio jitter buffer

先来看看AudioJitterBuffer在voip整个流程中的位置，为了把重点放在AudioJitterBuffer上，对整个流程做了很多简化：



![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-584aa83197d335d9.png)

audio_pipeline_in_voip1.png

**AudioRecoder** : 音频录音模块 。



**Audio PreProcessor** : 音频前处理， 如resample重采样，AEC回声消除， AGC声音增益，ANC噪声抑制等。



**Audio Encoder** : 音频编码模块， 编码方式有很多， 如Opus, G729等。



**Packetizer** : 网络分包组包模块。



**TrafficShappingCache** : 网络发送缓冲区。



**AudioJitterBuffer** : 本文要讲解的模块， 字面翻译就是音频抗抖动缓冲区。



**Audio Decoder** : 音频解码模块。



**Audio Mixer** : 混音模块，对于多人通话场景，一个人会同时收到多路音频流， 需要先混音之后再送给播放器。



**AudioPlayer** : 音频播放模块。



## audio jitter buffer的作用

- 抗抖动：因网络时延不均等原因产生的达到时间忽早忽晚，进而导致声音播放忽快忽慢，为了解决这种问题，而增加了jitter buffer, 使得从这个buffer中输出的包是尽可能的平滑。
- 去重：将收到的重复包丢弃。
- 触发重传：jitter buffer中每个packet(包）都有sequence number, 即包的序列号，序列号有间隔，则说明有丢包；　此包是否需要请求重传（重传在下一篇文章介绍），简单来说由两个因素决定，一是此包的播放时间(由jitter估算出来），　另一个是rtt(网络时延）。

## 怎么计算jitter

最简单的想法:两个连续的包，收到的时间点之差，减去，发送的时间点之差，即是当前的抖动，
jitter = (packet[i].recvTime - packet[i-1].recvTime) - (packet[i].sendTime - packet[i-1].sendTime);
jitter > 0 说明需要加速播放
jitter < 0 说明需要延迟播放



但是，网络传输的不只两个包，而是很多包，一个包的jitter会影响下一个，进而累积影响下去，　因此需要一个算法从更全面的角度来计算jitter。 一个扩展的想法，就是设置一个固定size的队列保存这些jitter, 通过计算这个队列中jitter的平滑值来更准确去逼近当前真实jitter。



下面是参考资料一中的一种audio jitter计算方法:





![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-4c937f6610686b88.png)

audiojitterbuffer.png

按照以上方法计算出delay后，就可以计算出play time, 进而用于做平滑播放，或是据此计算重传。



# video jitter buffer

- 下面以开源的webrtc为例来讲video jitter buffer：

  

  ![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-e62e3761a4539346.png)

  video_frame.png

这是video jitter buffer中数据的存储结构， 首先，接收到的网络包按packets sequence number的顺序组成frame, 而frame会有多种状态， uncompleted frame表示这个frame中只有部分包(packet)收到了， completed frame表时这个frame中所有packets都收到了， 而decodable frame肯定是completed frame且它所有参考帧都已经收到并可解码， 当一个imcompleted frame因为缓存堆积或延时等原因被清理后就变成empty frame加到free frame list, 当decodable frame被解码使用后也会回到free frame list。



- video jitter计算：

  

  ![img](https://i0.wp.com/upload-images.jianshu.io/upload_images/20533040-b9158ba0dd30a8de.png)

  video_jitter.png

JitterDelay =theta[0] * （MaxFS – AvgFS） + [noiseStdDevs * sqrt（varNoise） –noiseStdDevOffset]
　　其中theta[0]是信道传输速率的倒数，MaxFS是自会话开始以来所收到的最大帧大小，AvgFS表示平均帧大小。noiseStdDevs表示噪声系数2.33，varNoise表示噪声方差，noiseStdDevOffset是噪声扣除常数30。UpdateEstimate会不断地对varNoise等进行更新。
　　在得到jitterdelay之后，通过jitterdelay+ decodedelay +renderdelay，再确保大于音视频同步的延时，加上当前系统时间得到rendertime，这样就可以控制播放时间。控制
播放，也就间接控制了buffer的大小。



# audio vs video

综上， audio jitter buffer与video jitter buffer的主要区别在于encoded frame, 即audio和video编解码的差异， 一个audio frame 往往由一个audio packet组成，不依赖于其他frame即可解码，而一个video frame往往由多个video packets组成，还依赖于其他frame才可解码。而在计算jitter上，最基本的思想是一样的：jitter = (packet[i].recvTime - packet[i-1].recvTime) - (packet[i].sendTime - packet[i-1].sendTime); 至于计算frame delay则本质上没有区别，算法可以复用。



# 参考资料

[1]: https://arxiv.org/pdf/1205.2798.pdf 计算audio jitter的一种方法