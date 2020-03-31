- EncodedImageBufferInterface : public rtc::RefCountInterface

  ​	buffer带引用计数，可以考虑智能指针实现

  - EncodedImageBuffer

- EncodedImage：代表编码后的视频帧，持有buffer的指针，并且有视频帧的其他信息

  ​	rtc::scoped_refptr<EncodedImageBufferInterface> encoded_data_

  - VCMEncodedFrame
    - VCMFrameBuffer

- VCMPacket：代表接收的rtp视频包，一帧h264会拆成多个包发送

- class FrameList : public std::map<uint32_t, VCMFrameBuffer*, TimestampLessThan>

  时间戳做为key，插入map后自动排序，TimestampLessThan比较时间戳

- VCMJitterBuffer

  - 成员函数
    - GetFrame
      - incomplete_frames_查找是否有对应时戳帧
      - decodable_frames_查找是否有对应时戳帧
      - free_frames_获取一个空闲帧

- VCMReceiver

  - 成员
    - VCMJitterBuffer

- Module

  - VideoReceiver
    - 成员函数：Decode
      - 调用Video_receiver.cc函数FrameForDecoding
        - 调用receiver.cc函数FrameForDecoding
          - 调用jitter_buffer.cc函数NextCompleteFrame

