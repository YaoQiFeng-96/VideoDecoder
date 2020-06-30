#pragma once

extern "C"
{
#include "libavformat\avformat.h"
#include "libavcodec\avcodec.h"
}
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")

class videoDecoder
{
public:
	videoDecoder() = delete;
	videoDecoder(AVCodecID codec_id);
	~videoDecoder();

	bool Init();
	void AddData(char *pData, int size);
	bool GetData(char *&pData, int *size);
	int GetPixWidth();
	int GetPixHeight();

private:
	AVCodecID		m_codecid;
	char			*m_YuvBuf;
	AVPacket		*packet;
	AVFrame			*frame;
	const AVCodec	*codec;
	AVCodecContext	*c;
	AVCodecParserContext	*parser;

	int pix_w;
	int pix_h;

};

