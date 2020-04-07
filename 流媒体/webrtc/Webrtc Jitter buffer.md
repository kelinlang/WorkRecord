webrtc jitterbuffer

rtp包几个时间值： 接收时间 ，发送时间，网络传输时间，网络延迟时间（网络排队）
rtp包的接收时间 = sendtime + transferTime
rtp包的接收时间 = sendtime + transferTime + netdelayTime

抖动缓冲区目的就是为了消除网络延迟对音视频的平滑播放造成的影响，如果没有抖动缓冲区，就可能
播放的视频时快时慢，播放的音频断断续续。

计算抖动的第一步就是计算网络的延迟，第二部根据延迟设置合适的抖动缓冲区大小。

一.webrtc视频抖动缓冲计算伪代码

```
   FrameBuffer  抖动缓冲区（保存数据，并有延迟估算逻辑）

   //接收线程处理
   void receivedThread(){
      while(1){
         recv(data);
         FrameBuffer.insertPacket(receiverTime,data);
      }
   }

   void insertPacket(){
      1.按时间戳进行排序

      2. 更新包的依赖状态，例如p帧依赖于前面的i帧，依赖帧都ok更新continue标志

      3. 保存包接收时间等状态
       
   }
   
   -----------------------------------------------------------------------------------------------------
   //解码，渲染线程
   void renderThread(){
	   while(1){
	      packet=FrameBuffer.nextPacket();
          renderPacket(packet);
	   } 
   }

   void* FrameBuffer.nextPacket（）{
       1.从已经continue的队列中获取数据
       2.更新renderTime （这主要是进行估算延迟的）
          (1.)  计算renderTime，VCMTiming.RenderTimeMs(timestamp,)
                 VCMTiming根据delaytime(解码延迟，渲染延迟，网络延迟综合估算)，计算这个timestamp对应的渲染时间。

       3.计算网络延迟
          (1.)  VCMInterFrameDelay 计算跟上一帧的帧间延迟
          (2.)  VCMJitterEstimator 估算网络延迟 
          (3.)  更新 VCMTiming 网络延时值                     
       4. 计算解码延迟
          根据最近的多个解码延迟值，计算了一个滑动平均值
          VCMTiming。setDecoderDealy();
       5. 计算渲染延迟 基本是一个固定值
          VCMTiming.set_render_delay

       6. VCMTiming.MaxWaitingTime 计算当前出队帧的最大等待时间，如果渲染时间已经过了则丢掉这一条取下一条，否则到时间后跳出函数  
   }
```

二.webrtc音频抖动缓冲计算 伪代码
音频主要逻辑在NetEqImpl中

```
PacketBuffer       抖动缓冲区，仅仅保存数据没有计算逻辑
DelayManager       计算网络延迟
BufferLevelFilter  计算抖动缓冲区的大小
网络延迟单位： 包数量，延迟了几个包。音频包打包间隔固定的，基本上以20ms为准，包大小是一样的。

   //接收线程处理
   void receivedThread(){
      while(1){
         recv(data);
         保存数据  
         PacketBuffer.InsertPacketList(receiverTime,data);
         计算网络延迟
         DelayManager.Update(seqNum,timestamp);
      }
   }

   void DelayManager.Update(){
     1. 计算从队列中拉取出包的时间开始，到现在的间隔
     
     2. 根据包序号，包时间戳，计算绝对延迟了几个（iat_packets）间隔
       正常到达的包       iat_packets=1
       乱序提前到达的     iat_packets=0
       延迟到达的n个间隔  iat_packets =n
     3. CalculateTargetLevel更新间隔，（计算最近一段时间的延迟间隔概率，延迟峰值，推算出的延迟间隔）
   }

   -----------------------------------------------------------------------------------------------------
   //解码，渲染线程
   void renderThread(){
	   while(1){
	      packet=PacketBuffer.nextPacket();
          /通过抖动缓冲区中数据的大小，估算抖动缓冲区大小（延迟间隔为单位）
          BufferLevelFilter.Update();
          //比较网络延迟和抖动延迟，更新播放策略 加速，减速，正常
          renderPacket(packet);
	   } 
   }


  // buffer_size_packets : 当前本地有多少个包还没有播放
  // time_stretched_samples : 加速，减速播放了多少了包（增加量）
  // packet_len_samples : 一个包有多少个采样点
  void BufferLevelFilter.Update(size_t buffer_size_packets, int time_stretched_samples,
                  size_t packet_len_samples){
         1. 通过动态的遗忘因子（根据网络延迟值计算的），滑动平均计算延迟
         2. 计算加减速的影响（time_stretched_samples） 
         filtered_current_level_=xxx；
  }
```