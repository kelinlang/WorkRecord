#ifndef FFMPEG_REMUXING
#define FFMPEG_REMUXING

#include "libavutil/timestamp.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
//#include "libavutil/threadmessage.h"


typedef struct InputStream {
	int file_index;
	AVStream* st;

	int outStreamId;
}InputStream;

typedef struct OutputStream {
	int file_index;
	AVStream* st;
}OutputStream;

typedef struct InputFile {
	char* fileName;
	AVFormatContext* formatContext;
	int index;
	int numStream;

	//AVThreadMessageQueue* in_thread_queue;
	//pthread_t thread;           /* thread reading from this file */
	//int non_blocking;           /* reading packets from the thread should not block */
	//int joined;                 /* the thread has been joined */
	//int thread_queue_size;      /* maximum number of queued packets */
}InputFile;


typedef struct OutputFile {
	char* fileName;
	AVFormatContext* formatContext;
	AVOutputFormat* ofmt;
	AVDictionary* opts;
}OutputFile;

typedef struct RemuxingContext {
	InputFile** inputFiles;
	int        nbInputFiles;

	OutputFile* outputFile;

	int totalStreams;

	InputStream** inStreams;
	int numInStream;
} RemuxingContext;



void testRemuxing(int numInputFile, char** inputFileName, char* outputFileName,char* outputFormat);



#endif