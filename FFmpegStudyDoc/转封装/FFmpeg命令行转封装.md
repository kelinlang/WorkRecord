本文主要记录通过FFmpeg对MP4，FLV，M3U8格式的封装和文件分析以及对这些格式的一些了解。后面还对音视频的抽取做了简单的介绍。

MP4格式
在互联网常见的格式中，跨平台最好的就是MP4，它可以在PC的Flashplayer中播放，又可以在Android和iOS中播放

简介
MP4的官方术语介绍我们就不看了，对于MP4文件要了解如下几个重要的概念：

MP4文件由多个Box与FullBox组成
每个Box由Header和Data两部分组成
FullBox是Box的扩展，在Box的基础上，在Header中增加了8位version标志和24位flags标志
Header包含了整个Box的长度和大小和类型，当size等于0时，表示这个Box是文件的最后一个Box。当size为1时，说明Box长度需要更多位来描述，在后面会定义一个64位的largesize来描述Box的长度。当Type为uuid时，说明这个Box中的数据是用户自定义扩展类型
Data为Box的实际数据，可以是纯数据，也可以是更多的子Box
当一个Box的Data是一些列的子Box时，这个Box也可以称为Container Box

对于各种Box类型的分析我们就不在这里记录，因为太多了。具体参考书籍《FFmpeg从入门到精通》

MP4分析工具
MP4文件的分析工具除了FFmpeg外，还有很多可视化的工具，例如Elecard StreamEye,mp4box,mp4info,mediainfo等 这些工具我看了在Mac上面感觉都不咋行，于是在Windows上面玩了一下。

使用FFmpeg查看MP4文件
查看MP4文件解封装Demuxer
ffmpeg -h demuxer=mp4
1
查看MP4文件封装Muxer
ffmpeg -h muxer=mp4
1
上面两个命令可以查看FFmpeg的解封装和封装的参数，下面列举一些参数使用的示例

faststart参数使用
使用mp4info查看一个mp4文件的Box信息通常如下所示：



moov在madt写完以后再写入，所以它出现在容器的最后面。通过faststart可以把moov移到mdat的前面：注意要从一个flv文件转换为mp4文件让它重新生成Box数据
由于我只准备了一个mp4文件，所以先通过下面的命令把我的mp4文件转换为flv文件再来验证这个参数：
ffmpeg -i ~/Movies/i_am_you.mp4 -vcodec libx264 -preset fast -profile baseline -x264opts bitrate=300 -s 320x240 -r 12 -g 12 -f flv ~/Movies/output.flv
1
下面可以验证我们的faststart参数的效果了

ffmpeg -i output.flv -c copy -f mp4 -movflags faststart output.mp4
1
转换完成以后通过mp4info来查看MP4文件的Box信息


dash参数使用示例
使用DASH格式的时候，生成的MP4文件的Box信息和之前的不同，它主要包含的容器为sidx,moof和mdat这三个Box
FFmpeg命令生成DASH格式的MP4文件：

ffmpeg -i input.flv -c copy -f mp4 -movflags dash output.mp4
1
查看生成的MP4文件的Box组成


FLV格式
在网络直播和点播场景中，FLV是一种常见的格式。它的封装格式很简单，均以FLVTAG的形式存在，并且每一个TAG都是独立存在的。
FLV格式的分析我就不记录了。有点儿复杂

FLV文件格式分析工具
可视化工具可以使用flvparse或者FlvAnalyzer来分析FLV文件格式的信息。我使用FlvAnalyzer工具来查看了一个flv文件的内容如下：



使用ffmpeg也可以分析flv文件的格式命令行如下：
ffprobe -v trace -i ~/Movies/output.flv
1
输出如下：


使用FFmpeg操作转换FLV
主要介绍MP4转换为FLV格式 ，上面我们没有FLV格式的素材的时候转换了一下，并没有了解其中的参数的具体含义。下面我们着重了解一下。
如果封装FLV的时候，内部的音频或者视频不符合标准时，那么它们就无法封装进去。因为FLV的音视频的编码支持如下
支持视频编码：

Sorenson H.263
Screen Video
On2 VP6
带Alpha通道的On2 VP6
Screen Video2
H.264(AVC)
支持音频编码：
限行PCM 大小端取决于平台
ADPCM音频格式
MP3
线性PCM 小端
Nellymoser 16kHz Mono
Nellymoser 8kHz Mono
Nellymoser
MP4封装为FLV
如果源视频的音视频编码是FLV所支持的，那就可以顺利的封装
上面的封装命令是从网上copy的其实如果我的源视频支持FLV的封装格式只要如下命令：

ffmpeg -i ~/Movies/i_am_you.mp4 -c copy -f flv ~/Movies/output.flv
1
就可以完成转换

ffmpeg -i ~/Movies/i_am_you.mp4 -vcodec libx264 -preset fast -profile baseline -x264opts bitrate=300 -s 320x240 -r 12 -g 12 -f flv ~/Movies/output.flv
1
这个命令就是在其中添加了很多封装的参数。到后面详细了解每个参数的含义。

生成带关键索引的FLV
在网络视频点播文件为FLV格式时，人们常用yamdi工具对FLV文件进行一次转换，主要是将FLV文件中的关键帧建立一个索引，并将索引写入Metadata中，使用FFmpeg也可以实现

ffmpeg -i input.mp4 -c copy -f flv -flvflags add_keyframe_index output.flv
1
M3U8格式
M3U8是一种常见的流媒体格式，主要以文件列表的形式存在，即支持直播也支持点播，尤其在Android和IOS最为常用

MP4文件转HLS切片
FFmpeg中自带HLS的封装参数，使用HLS格式即可进行HLS的封装，但是生成HLS的时候有很多参数可以参考具体的参数使用的时候查看文档
转换文件命令：

ffmpeg -re -i input.mp4 -c copy -f hls -bsf:v h264_mp4toannexb output.m3u8
1
其中多了一个参数命令 -bsf:v h264_mp4toannexb 这个参数的作用是将MP4中的H.264数据转换为H.264 AnnexB标准的编码，AnnexB标准的编码常用于实时传输流中。如果源文件为FLV，TS等可以作为直播传输流的视频，就不要这个设置。

执行完这个命令之后 看到目录下面有一大片ts切片文件我上面的命令默认的每一个切片的时间是5秒，这个时间可以通过 -hls_time这个参数来设置

ffmpeg -re -i input.mp4 -c copy -f hls -bsf:v h264_mp4toannexb -hls_time 40 output.m3u8
1
它还有很多参数可以设置 使用的时候查看具体的需求和文档操作

视频文件切片
视频文件切片和HLS基本类似，但是HLS切片在标准中只支持TS格式的切片，并且是直播与点播切片。我们可以把一个mp4文件来切成多个mp4文件，可以使用segment 方式来进行切片，也可以使用ss加上t参数进行切片。

使用segment参数切片
这里指定一个切片命令的参数-segment_format

ffmpeg -re -i input.mp4 -c copy -f segment -segment_format mp4 test_output-%d.mp4
1
segment还有很多其余的参数，用的时候了解要使用的参数。

使用ss与t参数进行切片
ss进行视频文件的seek定位，ss传递的参数为时间值，t传递的参数也是时间值

使用ss指定剪切开头部分
ffmpeg -ss 10 -i input.mp4 -c copy output.ts
1
命令执行后生成的ouput.ts会比input.mp4视频少大概8秒，因为output.ts是从input.mp4的第八秒开始截取的。使用如下命令看duration信息

ffprobe -v quiet -show_format input.mp4 |grep duration;ffmpeg -v quiet -show_format output.ts |grep duration
1
使用t指定视频总长度
使用FFmpeg除了可以指定开始截取位置，还可以指定截取数据的长度，FFmepg的t参数可以指定截取的视频长度，例如截取input.mp4文件的前十秒的数据
ffmpeg -i input.mp4 -c copy -t 10 -copyts output.mp4
1
使用output_ts_offset指定输出start_time
FFmpeg支持ss与t参数一同使用以达到切割视频的某一段的效果，但其不能指定输出文件的start_time，而且也不希望时间戳归0，可以使用output_ts_offset来达到指定输出文件的start_time的目的
ffmpeg -i input.mp4 -c copy -t 10 -output_ts_offset 120 output.mp4
1
音视频文件抽取音视频流
抽取音视频文件中的AAC音频流
ffmpeg -i input.mp4 -vn -acodec copy output.acc
1
抽取音视频文件中的H.264视频流
ffmpeg -i input.mp4 -vcodec copy -an output.h.264
1
抽取音视频文件中的H.265视频流
ffmpeg -i input.mp4 -vcodec copy -an -bsf hevc_mp4toannexb -f hevc output.hevc

