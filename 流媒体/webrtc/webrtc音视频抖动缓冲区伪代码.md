webrtc音视频抖动缓冲区伪代码
原创zhenfei2017 最后发布于2019-04-08 11:01:25 阅读数 8531  收藏
展开
webrtc jitterbuffer

rtp包几个时间值： 发送时间，接收时间，网络传输时间，网络延迟时间（网络排队）
理想情况下 rtp包的接收时间 = sendtime + transferTime
实际环境中 rtp包的接收时间 = sendtime + transferTime + netdelayTime

抖动缓冲区目的就是为了消除网络延迟（netdelayTime）对音视频的平滑播放造成的影响，如果没有抖动缓冲区，就可能
播放的视频时快时慢，播放的音频断断续续。

计算抖动的第一步就是计算网络的延迟（netdelayTime），第二部根据延迟设置合适的抖动缓冲区大小。

一.webrtc视频抖动缓冲计算伪代码

   FrameBuffer  抖动缓冲区（保存数据，并有延迟估算逻辑）

   //接收线程处理， 
   //接收rtp包，组包成Frame，按frame原始时间戳顺序插入队列中。接收线程没有对网络抖动相对处理
   void receivedThread(){
      while(1){
         //网络接收rtp包，并组包成Frame
         recv(data);
         //组包后的Frame插入FrameBuffer
         FrameBuffer.insertPacket(receiverTime,data);
      }
   }

   void insertPacket(frame){
      1. 对frame按时间戳进行排序

      2. 更新frame的依赖状态，例如p帧依赖于前面的i帧，依赖帧都ok更新continue标志
  }

-----------------------------------------------------------------------------------------------------
   //解码，渲染线程 ，这里面就进行的抖动缓冲区的建立（netdelayTime大小的缓冲区），
   void renderThread(){
	   while(1){
	      packet=FrameBuffer.nextPacket();
          renderPacket(packet);
	   } 
   }

   void* FrameBuffer.nextPacket（）{
        while(1){
	       1.从Frame队列中的队列中获取最早的(timestamp靠前的)数据
	       2.更新上一步获取的frame的渲染时间(renderTime)   
	         计算renderTime=VCMTiming.RenderTimeMs(timestamp,)；
	         renderTime= 根据frame.timestamp计算理论上显示时间 +解码延迟+渲染延迟+网络延迟（netdelayTime）
            这样抖动缓冲区的大小=解码延迟+渲染延迟+网络延迟（netdelayTime）
            
             . frame.timestamp计算理论上显示时间, 简单的来说就是一个差值              
               now + (frame.timestamp - pretimestamp) * k;
               
             . 计算解码延迟
               根据最近的多个解码延迟值，计算了一个滑动平均值
               VCMTiming。setDecoderDealy();
              
             . 计算渲染延迟 基本是一个固定值
                VCMTiming.set_render_delay
                                       
             . 计算网络延迟，这是一个动态值，缓冲区就可以动态维持大小的
              (1.)  VCMInterFrameDelay 计算跟上一帧的帧间延迟
              (2.)  VCMJitterEstimator 估算网络延迟 
              (3.)  更新 VCMTiming 网络延时值                     


​	        
​	
	       3. VCMTiming.MaxWaitingTime 计算当前出队帧的最大等待时间，
	          waitTime=renderTime-decodeDelay-renderDelay;  
	          if(now<waitTime){
	            如果当前还没有到达渲染时间，就等待。 否则获取下一帧（这其实就跳帧了）
	            sleep(now - waitTime)
	            break;
	          }
	         }
	   }

二.webrtc音频抖动缓冲计算 伪代码
音频主要逻辑在NetEqImpl中

PacketBuffer       抖动缓冲区，仅仅保存数据没有计算逻辑
DelayManager       计算网络延迟
BufferLevelFilter  计算抖动缓冲区的大小
网络延迟单位： 包数量，延迟了几个包。音频包打包间隔固定的，基本上以20ms为准，包大小是一样的。

   //接收线程处理，计算网络延迟
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
   //解码，渲染线程，根据缓冲区大小和网络延迟比较后，改变播放策略
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
————————————————
版权声明：本文为CSDN博主「zhenfei2017」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_16135205/java/article/details/89083884