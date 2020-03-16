#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swscale.lib")

#include <iostream>
#include <fstream>
extern "C"{
#include <libavformat/avformat.h>
}

using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::ios_base;

int main(){
    AVFormatContext *fmt_ctx = nullptr;
    const char * in_path = "E:\\whys_pr.mp4";
    const char * out_path = "E:\\out.yuv";
    if(avformat_open_input(&fmt_ctx,in_path,nullptr,nullptr) != 0){
    cout << "avformat_open_input错误" << endl;
    system("pause");
    return 1;
}

    //打印出视频信息
    //av_dump_format(fmt_ctx,0,in_path,0);

    if(avformat_find_stream_info(fmt_ctx,nullptr) < 0){
        avformat_free_context(fmt_ctx);
        cout << "avformat_find_stream_info错误" << endl;
        system("pause");
        return 1;
    }

    AVCodec *codec;
    AVCodecContext * cd_ctx;
    unsigned video_index = 0;

    for(unsigned i = 0;fmt_ctx->nb_streams;i++){
        //寻找到视频流
        if(fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_index = i;

            //获取到视频流
            AVStream *streams = fmt_ctx->streams[i];
            //streams->r_frame_rate
            cout << ((float)streams->avg_frame_rate.num) / streams->avg_frame_rate.den << "fps，";

            //视频编码类型
            cout << avcodec_get_name(streams->codecpar->codec_id) << endl;

            codec = avcodec_find_decoder(streams->codecpar->codec_id);
            if(!codec){
                cout << "没有找到解码器" << endl;
                system("pause");
                return 1;
            }

            //分配AVCodecContext空间
            cd_ctx = avcodec_alloc_context3(codec);

            //填充数据
            avcodec_parameters_to_context(cd_ctx,streams->codecpar);

            //视频尺寸
            cout << streams->codecpar->width << 'x' << streams->codecpar->height << endl;
            break;
        }
    }

    /* open it */

    if(avcodec_open2(cd_ctx,codec,NULL) < 0){
        cout << "avcodec_open2错误" << endl;
        system("pause");
        return 1;
    }

    AVPacket *pkt = av_packet_alloc();
    if(!pkt){
        cout << "av_packet_alloc错误" << endl;
        system("pause");
        return 1;
    }

    AVFrame *frame_yuv = av_frame_alloc();
        if(!frame_yuv){
        cout << "av_frame_alloc错误" << endl;
        system("pause");
        return 1;
    }

    //打开输出视频的文件
    ofstream out = ofstream(out_path,ios_base::binary);

    int frame_count = 0;//记录获取的帧数

    while(av_read_frame(fmt_ctx,pkt) >= 0 && frame_count < 100){//这里只获取100帧了

        //找到视频流
        if(pkt->stream_index == video_index){
            if(avcodec_send_packet(cd_ctx,pkt) != 0){
                cout << "avcodec_send_packet错误" << endl;
                break;
             }
            while(avcodec_receive_frame(cd_ctx,frame_yuv) == 0){
                frame_count++;

                //AV_PIX_FMT_YUV420P,  ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
                cout << "解码到第" << cd_ctx->frame_number << "帧" << endl;
                for(int i = 0;i < frame_yuv->height;i++){
                    out.write((char *)(frame_yuv->data[0] + i * frame_yuv->linesize[0]),frame_yuv->width);
                }

                int loop = frame_yuv->height / 2;
                int len_uv = frame_yuv->width / 2;

                for(int i = 0;i < loop;i++){
                    out.write((char *)(frame_yuv->data[1] + i * frame_yuv->linesize[1]),len_uv);
                }
                for(int i = 0;i < loop;i++){
                    out.write((char *)(frame_yuv->data[2] + i * frame_yuv->linesize[2]),len_uv);
                }
            }
        }
    }

    cout << "解码完毕，文件保存在" << out_path << endl;

    out.close();

    av_packet_free(&pkt);
    avcodec_close(cd_ctx);
    avcodec_free_context(&cd_ctx);
    av_frame_free(&frame_yuv);
    avformat_close_input(&fmt_ctx);

    system("pause");
    return 0;
}
