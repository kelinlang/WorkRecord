#include "FFmpegRemuxing.h"


//static void* input_thread(void* arg)
//{
//	InputFile* f = arg;
//	unsigned flags = f->non_blocking ? AV_THREAD_MESSAGE_NONBLOCK : 0;
//	int ret = 0;
//
//	while (1) {
//		AVPacket pkt;
//		ret = av_read_frame(f->formatContext, &pkt);
//
//		if (ret == AVERROR(EAGAIN)) {
//			av_usleep(10000);
//			continue;
//		}
//		if (ret < 0) {
//			av_thread_message_queue_set_err_recv(f->in_thread_queue, ret);
//			break;
//		}
//		ret = av_thread_message_queue_send(f->in_thread_queue, &pkt, flags);
//		if (flags && ret == AVERROR(EAGAIN)) {
//			flags = 0;
//			ret = av_thread_message_queue_send(f->in_thread_queue, &pkt, flags);
//			av_log(f->formatContext, AV_LOG_WARNING,
//				"Thread message queue blocking; consider raising the "
//				"thread_queue_size option (current value: %d)\n",
//				f->thread_queue_size);
//		}
//		if (ret < 0) {
//			if (ret != AVERROR_EOF)
//				av_log(f->formatContext, AV_LOG_ERROR,
//					"Unable to send packet to main thread: %s\n",
//					av_err2str(ret));
//			av_packet_unref(&pkt);
//			av_thread_message_queue_set_err_recv(f->in_thread_queue, ret);
//			break;
//		}
//	}
//
//	return NULL;
//}
//
//static void free_input_thread(int i,RemuxingContext* remuxingContext)
//{
//	InputFile* f = remuxingContext->inputFiles[i];
//	AVPacket pkt;
//
//	if (!f || !f->in_thread_queue)
//		return;
//	av_thread_message_queue_set_err_send(f->in_thread_queue, AVERROR_EOF);
//	while (av_thread_message_queue_recv(f->in_thread_queue, &pkt, 0) >= 0)
//		av_packet_unref(&pkt);
//
//	pthread_join(f->thread, NULL);
//	f->joined = 1;
//	av_thread_message_queue_free(&f->in_thread_queue);
//}
//
//static void free_input_threads(RemuxingContext* remuxingContext)
//{
//	int i;
//
//	for (i = 0; i < remuxingContext->inputFiles[i]; i++)
//		free_input_thread(i, remuxingContext);
//}
//
//static int init_input_thread(int i, RemuxingContext* remuxingContext)
//{
//	int ret;
//	InputFile* f = remuxingContext->inputFiles[i];
//
//	if (remuxingContext->nbInputFiles == 1)
//		return 0;
//
//	if (f->formatContext->pb ? !f->formatContext->pb->seekable :
//		strcmp(f->formatContext->iformat->name, "lavfi"))
//		f->non_blocking = 1;
//	ret = av_thread_message_queue_alloc(&f->in_thread_queue,
//		f->thread_queue_size, sizeof(AVPacket));
//	if (ret < 0)
//		return ret;
//
//	if ((ret = pthread_create(&f->thread, NULL, input_thread, f))) {
//		av_log(NULL, AV_LOG_ERROR, "pthread_create failed: %s. Try to increase `ulimit -v` or decrease `ulimit -s`.\n", strerror(ret));
//		av_thread_message_queue_free(&f->in_thread_queue);
//		return AVERROR(ret);
//	}
//
//	return 0;
//}
//
//static int init_input_threads(RemuxingContext* remuxingContext)
//{
//	int i, ret;
//
//	for (i = 0; i < remuxingContext->nbInputFiles; i++) {
//		ret = init_input_thread(i, remuxingContext);
//		if (ret < 0)
//			return ret;
//	}
//	return 0;
//}
//
//static int get_input_packet_mt(InputFile* f, AVPacket* pkt)
//{
//	return av_thread_message_queue_recv(f->in_thread_queue, pkt,
//		f->non_blocking ?
//		AV_THREAD_MESSAGE_NONBLOCK : 0);
//}

#define GROW_ARRAY(array, nb_elems)\
    array = grow_array(array, sizeof(*array), &nb_elems, nb_elems + 1)

void* grow_array(void* array, int elem_size, int* size, int new_size)
{
	if (new_size >= INT_MAX / elem_size) {
		av_log(NULL, AV_LOG_ERROR, "Array too big.\n");
		//exit_program(1);
	}
	if (*size < new_size) {
		uint8_t* tmp = av_realloc_array(array, new_size, elem_size);
		if (!tmp) {
			av_log(NULL, AV_LOG_ERROR, "Could not alloc buffer.\n");
			//exit_program(1);
		}
		memset(tmp + *size * elem_size, 0, (new_size - *size) * elem_size);
		*size = new_size;
		return tmp;
	}
	return array;
}

RemuxingContext remuxingContext;

static void freeInputFile(InputFile* inputfile) {
	if (inputfile != NULL) {
		avformat_close_input(&inputfile->formatContext);
		if (inputfile->fileName != NULL) {
			av_free(inputfile->fileName);
		}
		av_free(inputfile);
	}
}

static InputFile* allocInputFile() {
	InputFile*  inputfile = av_mallocz(sizeof(InputFile));
	return inputfile;
}


static int openInputFile(RemuxingContext* remuxingContext,InputFile* inputfile,int index,char* fileName) {
	int ret = -1;
	AVFormatContext* fc = NULL;
	
	if ((ret = avformat_open_input(&fc, fileName, NULL, NULL)) < 0)
	{
		printf("Error: Open input file failed.\n");
		return ret;
	}

	//获取输入视频文件中的流信息
	if ((ret = avformat_find_stream_info(fc, NULL)) < 0)
	{
		printf("Error: Failed to retrieve input stream information.\n");
		return ret;
	}
	av_dump_format(fc, index, fileName, 0);

	inputfile->formatContext = fc;
	inputfile->index = index;
	inputfile->numStream = fc->nb_streams;
	inputfile->fileName = av_strdup(fileName);

	//strcpy(inputfile->fileName, fileName);

	printf("%s stream is %d.\n", inputfile->fileName, inputfile->numStream);

	for (int i = 0; i < fc->nb_streams;i++) {
		GROW_ARRAY(remuxingContext->inStreams, remuxingContext->numInStream);
		AVStream* as = fc->streams[i];
		InputStream* inputStream = av_mallocz(sizeof(InputStream));
		inputStream->file_index = index;
		inputStream->st = as;
		remuxingContext->inStreams[i] = inputStream;
	}

	return ret;
}

static void freeOutputFile(OutputFile* outputfile) {
	if (outputfile != NULL) {
		/*avformat_close_input(outputfile->formatContext);*/
		/* close output */
		if (outputfile->formatContext && !(outputfile->ofmt->flags & AVFMT_NOFILE)) {
			avio_closep(&outputfile->formatContext->pb);
		}
			

		if (outputfile->fileName != NULL) {
			av_free(outputfile->fileName);
		}
		av_free(outputfile);
	}
}

static OutputFile* allocOutputFile() {
	OutputFile* outputfile = av_mallocz(sizeof(OutputFile));
	return outputfile;
}

static int openOutputFile(RemuxingContext* remuxingContext, OutputFile* outputfile,  char* fileName, char* format) {
	AVFormatContext* fc = NULL;
	int ret = -1;
	//按照文件名获取输出文件的句柄
	avformat_alloc_output_context2(&fc, NULL, format, fileName);
	if (!fc)
	{
		printf("Error: Could not create output context.\n");
		return ret;
	}
	AVOutputFormat* ofmt = fc->oformat;
	outputfile->formatContext = fc;
	outputfile->ofmt = fc->oformat;
	remuxingContext->outputFile = outputfile;
	outputfile->fileName = av_strdup(fileName);
	//strcpy(outputfile->fileName, fileName);

	int curStId = 0;
	for (int i = 0; i < remuxingContext->nbInputFiles;i++) {
		InputFile* inputFile = remuxingContext->inputFiles[i];

		

		for (int j = 0; j < inputFile->numStream; j++) {
			//av_program_add_stream_index(fc, program->id, curStId);//设置输出流id

			AVStream* inStream = inputFile->formatContext->streams[j];

			AVCodecParameters* avCodecParameters = inStream->codecpar;
			AVCodecContext* inAVCodecContext = avcodec_alloc_context3(NULL);
			if (!inAVCodecContext) {
				printf("alloc avcodec fail.");
				return ret;
			}
			ret = avcodec_parameters_to_context(inAVCodecContext, avCodecParameters);
			if (ret < 0) {
				printf("avcodec_parameters_to_context fail.");
				return ret;
			}

			AVStream* outStream = avformat_new_stream(fc, inAVCodecContext->codec);
			if (!outStream)
			{
				printf("Error: Could not allocate output stream.\n");
				return ret;
			}
			outStream->id = curStId;

			AVCodecContext* outAVCodecContext = avcodec_alloc_context3(NULL);
			if (!outAVCodecContext) {
				printf("alloc out avcodec fail.");
				return ret;
			}
			ret = avcodec_parameters_to_context(outAVCodecContext, avCodecParameters);
			if (ret < 0) {
				printf("out avcodec_parameters_to_context fail.");
				return ret;
			}

			ret = avcodec_copy_context(outAVCodecContext, inAVCodecContext);
			outAVCodecContext->codec_tag = 0;
			if (fc->oformat->flags & AVFMT_GLOBALHEADER)
			{
				outAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
			}

			ret = avcodec_parameters_from_context(outStream->codecpar, outAVCodecContext);
			if (ret < 0) {
				printf("avcodec_parameters_from_context fail.");
				return ret;
			}

			avcodec_free_context(&inAVCodecContext);
			avcodec_free_context(&outAVCodecContext);
			curStId++;
		}

		
	}

	curStId = 0;
	if (!strcmp(format, "mpegts")) {
		for (int i = 0; i < remuxingContext->nbInputFiles;i++) {//节目信息放在创建流之后，还没弄清楚缘由
			AVProgram* program = av_new_program(fc, i + 1);
			//av_dict_set(&program->metadata, "title", inputFile->fileName, 0);

			av_dict_set(&program->metadata, "title", "tv", 0);
			InputFile* inputFile = remuxingContext->inputFiles[i];

			for (int j = 0; j < inputFile->numStream; j++) {
				av_program_add_stream_index(fc, program->id, curStId++);//设置输出流id
			}
		}
	}
	

	av_dump_format(fc, 0, fileName, 1);

	if (!(ofmt->flags & AVFMT_NOFILE))
	{
		AVDictionary* options = NULL;
		av_dict_set(&options, "protocol_whitelist", "file,udp,rtp,rtmp,rtsp,tcp", 0);
		ret = avio_open2(&fc->pb, fileName, AVIO_FLAG_WRITE,NULL,&options);
		//ret = avio_open(&fc->pb, fileName, AVIO_FLAG_WRITE);
		if (ret < 0)
		{
			printf("Error: Could not open output file ：%s\n",fileName);
			return ret;
		}
	}

	return ret;
}


void freeRemuxingContext(RemuxingContext* rc) {
	if (rc) {
		for (int i = 0; i < rc->nbInputFiles;i++) {
			freeInputFile(rc->inputFiles[i]);
		}
		freeOutputFile(rc->outputFile);
	}
}


void testRemuxing(int numInputFile,char** inputFileName, char* outputFileName, char* outputFormat)
{
	av_log_set_level(AV_LOG_DEBUG);
	//#if CONFIG_AVDEVICE
	avdevice_register_all();
	//#endif
	avformat_network_init();


	RemuxingContext remuxingContext;
	RemuxingContext* pRemuxingContext = &remuxingContext;
	memset(&remuxingContext, 0, sizeof(struct RemuxingContext));
	remuxingContext.nbInputFiles = 0;//
	printf("open in put file\n");

	for (int i = 0; i < numInputFile;i++) {
		GROW_ARRAY(remuxingContext.inputFiles, remuxingContext.nbInputFiles);
		InputFile* inputFile = allocInputFile();
		if (openInputFile(pRemuxingContext,inputFile,i, inputFileName[i]) >= 0) {
			remuxingContext.inputFiles[remuxingContext.nbInputFiles - 1] = inputFile;
			pRemuxingContext->totalStreams += inputFile->numStream;
		}else {
			printf("open input error");
			freeInputFile(inputFile);
		}
	}
	printf("open out put file\n");
	OutputFile* outputFile = allocOutputFile();
	openOutputFile(pRemuxingContext, outputFile, outputFileName, outputFormat);


	int needRead[numInputFile];
	for (int i = 0; i < numInputFile;i++) {
		needRead[i] = 1;
	}

	int ret = 0;
	ret = avformat_write_header(outputFile->formatContext, NULL);
	if (ret < 0)
	{
		printf("Error: Could not write output file header.\n");
		goto end;
	}

	
	AVPacket pkt;
	int loop = numInputFile;
	int  readNum = 0;
	while (loop)
	{
		AVStream* in_stream, * out_stream;
		int curStId = 0;
		
		OutputFile* outputFile = pRemuxingContext->outputFile;
		for (int i = 0; i < numInputFile;i++) {
			if (needRead[i] == 0) {
				continue;
			}
			InputFile* inputFile = pRemuxingContext->inputFiles[i];

			ret = av_read_frame(inputFile->formatContext, &pkt);
			if (ret == AVERROR(EAGAIN)) {
				av_usleep(10000);
				continue;
			}
			readNum++;
			if (ret < 0) {
				printf("    av_read_frame ret : %d\n", -ret);
				//av_log(NULL, AV_LOG_ERROR,"av_read_frame ret : %d\n",-ret);
				loop--;
				needRead[i] = 0;
				break;
			}
			in_stream = inputFile->formatContext->streams[pkt.stream_index];
			if (i == 1) {
				out_stream = outputFile->formatContext->streams[pkt.stream_index+2];//取对应的输出流id
			}else{
				out_stream = outputFile->formatContext->streams[pkt.stream_index];//取对应的输出流id
			}
			

			/* copy packet */
			pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, /*(AVRounding)*/(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, /*(AVRounding)*/(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
			pkt.pos = -1;

			if (i == 1) {
				pkt.stream_index = pkt.stream_index + 2;//取对应的输出流id
			}
			

			ret = av_interleaved_write_frame(outputFile->formatContext, &pkt);
			if (ret < 0)
			{
				fprintf(stderr, "Error muxing packet\n");
				break;
			}
			av_free_packet(&pkt);
			
		}
		av_usleep(10000);//延迟10ms，要不然一下推完流，播放端缓存不了那么多，会做处理

	}
	printf("    readNum : %d\n", readNum);
	av_write_trailer(pRemuxingContext->outputFile->formatContext);
	printf("finish remux------------------------------\n");
end:
	freeRemuxingContext(pRemuxingContext);
}
