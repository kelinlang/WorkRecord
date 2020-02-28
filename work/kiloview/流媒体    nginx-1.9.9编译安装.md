1.源码下载，放同一路径
https://nginx.org/en/download.html  nginx-1.17.6
https://github.com/AlexWoo/nginx-toolkit-module.git
https://github.com/nats-io/nginx-nats.git
https://github.com/AlexWoo/nginx-client-module.git
https://github.com/AlexWoo/nginx-multiport-module.git
https://github.com/im-pingo/nginx-rtmp-module.git

tar -zxvf xxxx.tar.gz

2.执行下面命令配置
./configure --prefix=/home/kelinlang/workspace/install/nginx_rtmp/nginx-1.9.9 --with-pcre=/home/kelinlang/workspace/server/nginx/pcre-8.34 --with-openssl=/home/kelinlang/workspace/server/nginx/openssl-1.0.2d --add-module=/home/kelinlang/workspace/server/nginx/nginx-rtmp-module
./configure --prefix=/home/kelinlang/workspace/install/nginx_rtmp/nginx-1.9.9 --with-pcre=/home/kelinlang/workspace/server/nginx/pcre-8.34 --with-openssl=/home/kelinlang/workspace/server/nginx/openssl-1.0.2d --add-module=/home/kelinlang/workspace/server/nginx/nginx-rtmp-module --with-cc-opt="-Wno-error"
./configure --prefix=/home/kelinlang/workspace/install/nginx_rtmp/nginx-1.9.9 --with-pcre=/home/kelinlang/workspace/server/nginx/pcre-8.34 --with-openssl=/home/kelinlang/workspace/server/nginx/openssl-1.0.2d --add-module=/home/kelinlang/workspace/server/nginx/nginx-rtmp-module --without-http_gzip_module
./configure --prefix=/home/kelinlang/workspace/install/nginx_rtmp/nginx-1.9.9 --with-pcre=/home/kelinlang/workspace/server/nginx/pcre-8.34 --with-openssl=/home/kelinlang/workspace/server/nginx/openssl-1.0.2d --add-module=/home/kelinlang/workspace/server/nginx/nginx-rtmp-module --add-module=/home/kelinlang/workspace/server/nginx/nginx-rtmp-module/hls

1.17.6版本配置
./configure --prefix=/home/kelinlang/workspace/install/nginx_rtmp/nginx-1.17.6 --add-module=../nginx-client-module       --add-module=../nginx-multiport-module             --add-module=../nginx-toolkit-module                   --add-module=../nginx-rtmp-module --with-http_ssl_module  --with-cc-opt="-Wno-error"


3. 执行make -j8&& make install
错误解决：/home/vagrant/nginx-1.11.2/objs/Makefile 修改如下
./configure --with-cc-opt="-Wno-error"
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror -g  -I/home/kelinlang/workspace/server/nginx/nginx-rtmp-module  

修改连接地址：/etc/ld.so.conf   ldconfig

4.rtmp://192.168.3.157:1395/mylive/rtmpstream
5.启动   
6.查进程  ps -ef | grep nginx

复制状态html：cp /home/kelinlang/workspace/server/nginx/nginx-rtmp-module/stat.xsl html/
网页查看状态：http://192.168.2.162:8080/rtmp_stat

7.推流
ffmpeg -re -i E:\YunvaProject\视频文件\VID_20190104_111210.mp4 -f flv rtmp://192.168.3.157:1395/mylive/rtmpstream
./ffmpeg -re -i ./E1.mp4 -c copy -f flv rtmp://192.168.2.162:1935/live/test
./ffmpeg -re -i ./E1h265.mp4 -f flv rtmp://192.168.2.162:1935/live/test
./ffmpeg -re -i ./E1.mp4 -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i ./E1h265.mp4 -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -f flv rtmp://192.168.0.18:1935/live/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f flv rtmp://192.168.2.162:1935/live/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f flv rtmp://192.168.2.162:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f flv rtmps://192.168.2.199:1935/live/test


./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/1080P.h265 -c copy -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/1080P.h265  -c copy -f flv rtmp://192.168.2.162:1935/live/test

./ffmpeg -re -i /tmp/workspace/testFiles/E1h265.mp4  -c copy -f flv rtmp://192.168.2.199:1935/live/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f rtsp rtsp://192.168.2.198:5555/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f rtsp rtsp://192.168.2.198:5555/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f rtsp rtsp://192.168.2.162:5555/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f rtsp rtsp://192.168.2.162:5555/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f rtsp rtmp://192.168.2.162:1936/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f rtsp rtmp://192.168.2.162:1936/live/test
./ffmpeg  -i rtsp://192.168.2.162:5555/live/test  -c copy -f flv rtmp://192.168.2.162:1935/hls/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f flv rtmp://192.168.2.162:1935/live/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4  -c copy -f flv /home/kelinlang/workspace/testFiles/E1h265.flv
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4  -c copy -f flv /home/kelinlang/workspace/testFiles/E1.flv

./ffmpeg -re -i rtsp://192.168.3.101/ch01 -c:v libx265 -c:a aac -b:a 128k -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i rtsp://192.168.3.101/ch01  -f flv rtmp://192.168.0.18:1935/live/test
./ffmpeg -re -i rtsp://192.168.3.101/ch01  -f flv rtmp://192.168.2.162:1935/live/test
~/bin/ffmpeg -re -i rtsp://192.168.3.101/ch01 -c:v libx265 -profile:v main -c:a aac -b:a 128k -f flv rtmp://192.168.2.162:1935/live/test
~/bin/ffmpeg -re -i rtsp://192.168.3.101/ch01 -c:v libx265 -profile:v main -c:a aac -b:a 128k -f flv /home/kiloview/ch01_h265.flv

ffmpeg -re -i rtsp://192.168.3.101/ch01 -c:v libx265 -profile:v main -c:a aac -b:a 128k -f flv rtmp://192.168.2.162:1935/live/test
ffmpeg -re -i rtsp://192.168.3.38/ch01 -c:v libx265 -profile:v main -c:a aac -b:a 128k -f flv rtmp://192.168.0.18:1935/live/test

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1h265.mp4 -vcodec copy -f mpegts "udp://192.168.2.200:1234?pkt_size=1316"  

./ffmpeg -re -i /home/kelinlang/workspace/testFiles/test1.ts  -c copy -f mpegts udp://192.168.2.198:55555?pkt_size=1316


udp://@192.168.2.34:1234

端口查看命令：netstat -ntpl


8.播放
./ffplay /home/kelinlang/workspace/testFiles/E1.mp4 
./ffplay /home/kelinlang/workspace/testFiles/E1h265.mp4 
./ffplay /home/kelinlang/workspace/testFiles/E1h265.flv
./ffplay rtmp://192.168.0.18:1935/live/test
./ffplay rtmp://192.168.2.162:1935/live/test
./ffplay rtsp://192.168.2.162:5555/live/test
./ffplay http://192.168.2.162:8080/hls/test.m3u8
./ffpaly rtsp://192.168.2.199/record/live/0/E1.mp4

转码
ffmpeg -i /home/kelinlang/workspace/testFiles/E1.mp4 -c:v libx265 -b:v 5000k /home/kelinlang/workspace/testFiles/E1_h265_test.mp4

重封装
./ffmpeg -re -i ./E1.mp4 -i ./Record-null.ts -map 0:0 -map 0:1 -map 1:0 -map 1:1 -c copy -program title=CCTV1:program_num=1:st=0:st=1 -program title=CCTV2:program_num=2:st=2:st=3 -f mpegts "udp://192.168.2.34:1234?pkt_size=1316"
./ffmpeg -re -i /home/kelinlang/workspace/testFiles/E1.mp4 -i /home/kelinlang/workspace/testFiles/Record-null.ts -map 0:0 -map 0:1 -map 1:0 -map 1:1 -c copy -program title=CCTV1:program_num=1:st=0:st=1 -program title=CCTV2:program_num=2:st=2:st=3 -f mpegts /home/kelinlang/workspace/testFiles/tstest.ts
./ffmpeg  -i /home/kelinlang/workspace/testFiles/E1.mp4   -f hevc  /home/kelinlang/workspace/testFiles/E1_h265_2.mp4


FFmpeg编译
./configure --enable-shared --enable-static   --enable-ffplay  --prefix=/home/kelinlang/workspace/install/ffmpeg
./configure --enable-shared --enable-static   --enable-ffplay  --prefix=/home/kelinlang/workspace/install/ffmpeg3.3
./configure --enable-shared --enable-static   --enable-ffplay  --prefix=/home/kelinlang/workspace/install/ffmpegDev/3.3

./configure --enable-shared --enable-static   --enable-ffplay --enable-libx264 --enable-libx265 --enable-gpl --enable-libsrt


./configure --enable-gpl --enable-libx264  --enable-libx265  --enable-static --enable-shared  --enable-ffplay --disable-yasm --prefix=/home/kelinlang/workspace/install/ffmpeg3.3.6
./configure --enable-gpl --enable-libx264  --enable-libx265  --enable-ffplay --disable-yasm --prefix=/home/kelinlang/workspace/install/ffmpeg3.3.6


复制命令
scp ffmpeg-3.3.6.tar.xz kelinlang@192.168.2.162:~/
scp ffmpeg-3.3.6.tar.xz kiloview@192.168.0.18:~/

sudo cp libx265.a libx265.so libx265.so.184 ../ -rfpv

linux打补丁
patch -p1 < 0003-The-RTMP-protocol-extensions-for-H.265-HEVC.patch

git config credential.helper store
git rm -r --cached 

色调：85；饱和度：123；亮度：205，

R:     199;          G:    237;       B:204;

颜色代码是： #C7EDCC

./configure --toolchain=msvc --target-os=win64 --enable-ffplay --enable-gpl   --enable-static --enable-shared  --enable-debug  --disable-x86asm  --extra-cflags=-g --extra-ldflags=-g  --enable-nonfree --enable-libx264  --extra-cflags="-I../build/include"  --extra-ldflags="-LIBPATH:../build/lib"  --prefix=./build


./ffmpeg -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -c copy -f mpegts "srt://192.168.2.169:12500?pkt_size=1316?passphrase='password321'&mode=listener"
./ffmpeg -fflags +genpts -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -map 0:0 -map 0:1 -c:v copy -c:a libfdk_aac -profile:a aac_he_v2  -metadata service_provider='TEST' -metadata service_name='TEST-CH' -mpegts_transport_stream_id '0x0001' -mpegts_service_id '0x0bba' -mpegts_start_pid '0x0bba' -f mpegts "srt://192.168.2.169:12500?pkt_size=1316?passphrase='password321'&mode=listener"
./ffplay -fflags nobuffer "srt://192.168.2.169:12500/live/test?paket_size=1316?passphrase='password321'&mode=caller"

./ffmpeg -y -vsync 0 -hwaccel cuvid -c:v libx265 -i ./video-h265.mkv - -c:a copy -c:v h264_nvenc -b:v 5M ./video-h264-1.mp4  

./ffmpeg -i ../testFiles/E1.mp4 -vcodec libx264 -preset ultrafast -b:v 2000k E1_2000k.mp4

./ffmpeg   -i ../testFiles/video-h265.mkv -vcodec libx264   ./testFiles/video-h264-1.mp4

./ffmpeg -i ../testFiles/E1.mp4 -c:v libx264 -x264opts "bframes=3:b-adapt=0" -g 50 -sc_threshold 0 ../testFiles/E1_had_b.mp4

./ffmpeg -i ../../../testFiles/E1.mp4 -c:v libx264 -x264opts "bframes=10:b-adapt=0" -b:v 3000k -maxrate 3000k -minrate 3000k  -nal-hrd cbr -g 50 -sc_threshold 0 -t 60 ../../../testFiles/E1_had_b.ts
./ffprobe -v quiet -show_frames ../../../testFiles/E1_had_b.ts | grep "pict_type=B" | wc -l


./ffmpeg -i ../../../testFiles/E1.mp4 -c:v libx264 -x264-params level=30:bframes=0:weightp=0:\
cabac=0:ref=1:vbv-maxrate=768:vbv-bufsize=2000:analyse=all:me=umh:\
no-fast-pskip=1:subq=6:8x8dct=0:trellis=0 ../../../testFiles/E1_x264_params.mp4

./ffmpeg -i ../../../testFiles/E1.mp4 -vf "split [main][tmp]; [tmp] crop=iw:ih/2:0:0, vflip [flip]; [main][flip] overlay=0:H/2" ../../../testFiles/E1_test_filter.mp4

./ffmpeg -re -stream_loop -1 -i ../testFiles/test1_ts_with_no_b.ts  -c copy -f mpegts udp://192.168.2.90:1234?pkt_size=1316
./ffmpeg -re -stream_loop -1 -i ../testFiles/test1_ts_with_no_b.ts  -c copy -f mpegts udp://192.168.2.66:1234?pkt_size=1316
./ffmpeg -re -stream_loop -1 -i ../testFiles/test1_ts_with_no_b.ts  -c copy -f mpegts udp://192.168.2.66:1235?pkt_size=1316

./ffplay -protocol_whitelist “file,http,https,rtp,udp,tcp,tls” udp://192.168.2.66:1234?pkt_size=1316

./ffmpeg -re -stream_loop -1 -i ../testFiles/test1.ts  -c copy -f rtsp rtsp://192.168.2.198:5555?live/test
./ffmpeg -re -stream_loop -1 -i ../testFiles/test1.ts  -c copy -f flv rtmp://192.168.2.198:1935?live/test
./ffmpeg -re  -stream_loop -1 -i ../testFiles/test1.ts -vcodec libx264 -profile:v baseline -level 3.1 -s 1920x1080 -an -y  ../testFiles/test1_ts_with_no_b.ts
./ffprobe -v quiet -show_frames ../testFiles/test1_ts_with_no_b.ts | grep "pict_type=B" | wc -l


./ffmpeg -i ../testFiles/leishen.wav -codec:a libfdk_aac -vbr 3 ../testFiles/leishen.mp4

./ffmpeg -i ../testFiles/E1.mp4 -vf "split [main][tmp]; [tmp] crop=iw:ih/2:0:0, vflip [flip]; [main][flip] overlay=0:H/2" ../testFiles/E1_with_crop_and_vflip.mp4
./ffmpeg -i ../testFiles/E1.mp4 -vf scale=640:360 ../testFiles/E1_640_360.mp4

ffplay -max_delay 500000 -rtsp_transport udp rtsp://192.168.2.198:5555/live/test


./ffmpeg -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -c copy -f mpegts "srt://192.168.2.66:12500?pkt_size=1316&mode=listener"
./ffmpeg -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -c copy -f mpegts "srt://192.168.0.18:10000?pkt_size=1316&mode=listener"
./ffmpeg -re -i '/home/kelinlang/workspace/testFiles/E1.mp4' -c copy -f mpegts "srt://192.168.2.66:10000?pkt_size=1316&mode=caller"

ffmpeg -re -i doc/source.200kbps.768x320.flv -c copy \
    -f mpegts 'srt://192.168.0.18:10080?streamid=#!::h=live/livestream,m=publish'
	
	./ffplay rtmp://192.168.0.18/live/livestream
	
	./ffplay 'srt://192.168.0.18:10080?streamid=#!::h=live/livestream,m=request'

./ffplay -fflags nobuffer "srt://192.168.2.169:12500/live/test?paket_size=1316&mode=caller"


.