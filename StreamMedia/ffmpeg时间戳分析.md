



- 
- 
- 输入流打开流程
  - **avformat_open_input**     utils.c
    - avformat_alloc_context
    - init_input
      - av_probe_input_buffer2
      - io_open = io_open_default
        - ffio_open_whitelist
          - ffurl_open_whitelist
            - ffurl_alloc
              - url_find_protocol
              - url_alloc_for_protocol
            - ffurl_connect
              - url_open2()  ：调用对于协议的open方法，代码生成 url_protocols数组
          - ffio_fdopen
            - avio_alloc_context
              - ffio_init_context
      - av_probe_input_buffer2
        - 循环探测
          - av_reallocp
          - avio_read
          - av_probe_input_format2
            - av_probe_input_format3
    - avformat_queue_attached_pictures
    - update_stream_avctx





- de_profile.c 定义了时间基
- avformat文件夹下dv.c
  - AVInputFormat ff_dv_demuxer = {
        .name           = "dv",
        .long_name      = NULL_IF_CONFIG_SMALL("DV (Digital Video)"),
        .priv_data_size = sizeof(RawDVContext),
        .read_probe     = dv_probe,
        .read_header    = dv_read_header,
        .read_packet    = dv_read_packet,
        .read_close     = dv_read_close,
        .read_seek      = dv_read_seek,
        .extensions     = "dv,dif",
    };
- AVIOContext
  - read_packet = io_read_packet
    - ffurl_read
      - retry_transfer_wrapper 参数有个函数指针：transfer_func
        - transfer_func = h->prot->url_read  (URLContext *h,)
- URLContext
  - URLProtocol *prot
- URLProtocol
  - url_read
- av_read_frame   读取数据包
  - read_frame_internal
    - ff_read_packet
      - av_init_packet
        - 时间戳设置无效值
      - read_packet : AVInputFormat
      - update_wrap_reference 
        - 取得第一个dts，开始时间，当前dts
      - wrap_timestamp  : **处理时间戳，如果时间戳预测溢出，则修改时间戳在一个范围内**
      - force_codec_ids
      - 如果use_wallclock_as_timestamps为1(默认值为0)，**重新设置pts和dts，**
        - `pkt->dts = pkt->pts = av_rescale_q(av_gettime(), AV_TIME_BASE_Q, st->time_base);`
    - parse_packet
      - AVPacket **out_pkt**：初始化
      - av_parser_parse2
      - 修改显示时长
      - 设置显示时间戳和解码时间戳
      - compute_pkt_fields：重设**out_pkt**中的时间戳
        - 如果是视频&&有效的解码时间戳 
          - 如果显示时间戳等于解码时间戳&& 上一次解码时间戳有效
          - 修改上一次解码时间戳
          - 判断处理xxx
        - 满足标记条件&&显示时间戳有效
          - 设置解码时间戳为无效值
        - 如果有**B帧**
          - 设置有**B帧**标记
        - delay值设置为有**B帧**标记
        - 时间戳都有效&&显示时间戳位数小于63&&解码时间戳减去一个值小于显示时间戳
          - 判断（st->当前时间戳是相对的 || 解码时间戳减去一个值小于st->当前时间戳）
            - 是: 修改解码时间戳
            - 否：修改显示时间戳
        - 判断某些情况
          - 解码时间戳设置无效值
        - 计算显示时长
        - 如果显示时长小于0
          - 重新计算显示时长
        - 时长有效
          - 更新时长
        - 判断条件
          - 更新时间戳
        - 调试模式 并且TS调试模式
          - 打印时间戳
        - 判断条件
          - 内部篡改时间戳
      - ff_packet_list_put：**out_pkt** 放进列表
    - ff_packet_list_get
    - 调试模式 并且TS调试模式
      - 打印时间戳
- AVStream 
  - avformat_new_stream 初始化方法
    - avpriv_set_pts_info**(st, 33, 1, 90000)**; 设置时间基
- AVInputFormat
  - read_header
- AVOutputFormat
- rawdec.h  去取原始格式文件,如xx.h264文件
- rawdec.c  **读原始格式文件**
  - ff_raw_video_read_header 设置到AVInputFormat 成员read_header中
    - avformat_new_stream  **初始化流** 
    - avpriv_set_pts_info(st, 64, 1, 1200000);//**初始化裸流的时间基：1/1200000**
  - ff_raw_read_partial_packet
    - av_new_packet   **时间戳设置无效值**
    - avio_read_partial 读取裸流数据
    - av_shrink_packet
- **avpriv_set_pts_info**  **设置时间基函数**
  - **rtsp.c  sdp:**   
    - 音频：avpriv_set_pts_info(st, 32, 1, par->sample_rate);
    - 视频：avpriv_set_pts_info(st, 32, 1, i);
  - **裸流视频：**avpriv_set_pts_info(st, 64, 1, 1200000)



- rtsp.c   rtpdec.c  AVInputFormat
  - ff_rtsp_fetch_packet
    - ff_rdt_parse_packet
    - ff_rtp_parse_packet
      - rtp_parse_one_packet
        - rtp_parse_queued_packet
        - handler->parse_packet
        - finalize_packet 处理显示时间戳





