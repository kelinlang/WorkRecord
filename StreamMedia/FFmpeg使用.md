- FFmpeg

- ffplay

- ffprobe

  - ```javascript
    ffprobe -show_frames -select_streams v /home/kelinlang/workspace/testFiles/E1.mp4 | grep pkt_pts  
    ```

  - ffprobe -show_frames -select_streams a ../testFiles/video-h265.mkv > ../log/vdd_vddd_audio.tst 2>&1
  
  - 判断是否有B帧
  
    - ```text
      ffprobe -v error -show_frames gemfield.mp4 
      ```







# FFmpeg编译

- 编译正常版本
  - 指令
    - ./configure --enable-ffplay --enable-gpl --enable-libx264  --enable-libx265  --enable-static --enable-shared  --enable-debug  --disable-yasm --prefix=/home/kelinlang/workspace/install/ffmpeg3.3.6
  - make -j8 && sudo make install
- 编译调试版本
  - 指令
    - ./configure --enable-ffplay --enable-gpl --enable-libx264  --enable-libx265  --enable-static --enable-shared  --enable-debug  --disable-yasm  --extra-cflags=-g --extra-ldflags=-g
    - ./configure --enable-ffplay --enable-gpl --enable-libx264  --enable-libx265  --enable-static --enable-shared  --enable-debug  --disable-yasm  --extra-cflags=-g --extra-ldflags=-g prefix=/home/kelinlang/workspace/install/ffmpegDev/master
  - 编译成功后：会生成ffmpeg_g
  - 删除安装目录下面的ffmpeg相关的动态库，安装默认不带调试信息的库文件
    - 
  - `sudo cp ./*/*.so* /usr/local/lib`



# git 命令

- 保存用户名和密码
  - git config --global credential.helper store  





# VIM

- 

