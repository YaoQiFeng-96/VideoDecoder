#include "videoDecoder.h"

videoDecoder::videoDecoder(AVCodecID codec_id) :m_codecid(codec_id)
{
}


videoDecoder::~videoDecoder()
{
	if (nullptr != m_YuvBuf)
	{
		delete[] m_YuvBuf;
		m_YuvBuf = nullptr;
	}
	if (nullptr != packet)
	{
		av_packet_free(&packet);
		packet = nullptr;
	}
	if (nullptr != frame)
	{
		av_frame_free(&frame);
		frame = nullptr;
	}
	if (nullptr != c)
	{
		avcodec_free_context(&c);
		c = nullptr;
	}
	if (nullptr != parser)
	{
		av_parser_close(parser);
		parser = nullptr;
	}
}

bool videoDecoder::Init()
{
	if (m_codecid != AV_CODEC_ID_H264 && m_codecid != AV_CODEC_ID_H265)
	{
		fprintf(stderr, "error AVCodecID.\n");
		return false;
	}
	codec = avcodec_find_decoder(m_codecid);
	if (!codec)
	{
		fprintf(stderr, "Codec not found AV_CODEC_ID_H264.\n");
		return false;
	}
	c = avcodec_alloc_context3(codec);
	if (!c)
	{
		fprintf(stderr, "Could not allocate codec context.\n");
		return false;
	}
	if (avcodec_open2(c, codec, NULL) < 0)
	{
		fprintf(stderr, "Could not open codec.\n");
		return false;
	}
	packet = av_packet_alloc();
	if (nullptr == packet)
		return false;
	frame = av_frame_alloc();
	if (nullptr == frame)
		return false;
	parser = av_parser_init(codec->id);
	if (nullptr == parser)
		return false;
	return true;
}

void videoDecoder::AddData(char * pData, int size)
{
	char *data = pData;
	int data_size = size;
	int ret = 0;
	while (data_size > 0)
	{
		ret = av_parser_parse2(parser, c, &packet->data, &packet->size,
			(uint8_t*)data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
		if (ret < 0)
		{
			fprintf(stderr, "Error while parsing.\n");
			return;
		}
		data += ret;
		data_size -= ret;

		if (packet->size)
		{
			int sendRet = avcodec_send_packet(c, packet);
			if (sendRet < 0)
			{
				fprintf(stderr, "avcodec_send_packet error.\n");
			}
		}
	}
}

bool videoDecoder::GetData(char *& pData, int *size)
{
	int ret = avcodec_receive_frame(c, frame);
	if (ret < 0)
		return false;
	if (nullptr == m_YuvBuf)
		m_YuvBuf = new char[frame->height * frame->width * 3];

	if (0 == pix_w)
		pix_w = frame->width;
	if (0 == pix_h)
		pix_h = frame->height;

	if (c->pix_fmt == AV_PIX_FMT_YUV420P || c->pix_fmt == AV_PIX_FMT_YUVJ420P)
	{
		int a = 0;
		for (auto i = 0; i < frame->height; i++)
		{
			memcpy(m_YuvBuf + a, frame->data[0] + i * frame->linesize[0], frame->width);
			a += frame->width;
		}
		for (auto i = 0; i < frame->height / 2; i++)
		{
			memcpy(m_YuvBuf + a, frame->data[1] + i * frame->linesize[1], frame->width / 2);
			a += frame->width / 2;
		}
		for (auto i = 0; i < frame->height / 2; i++)
		{
			memcpy(m_YuvBuf + a, frame->data[2] + i * frame->linesize[2], frame->width / 2);
			a += frame->width / 2;
		}
		pData = m_YuvBuf;
		*size = frame->height*frame->width*1.5;
	}
	else if (c->pix_fmt == AV_PIX_FMT_YUV420P10LE)
	{
		int a = 0;
		for (auto i = 0; i < frame->height; i++)
		{
			memcpy(m_YuvBuf + a,
				frame->data[0] + i * frame->linesize[0],
				frame->width * 2);
			a += frame->width * 2;
		}
		for (auto i = 0; i < frame->height / 2; i++)
		{
			memcpy(m_YuvBuf + a,
				frame->data[1] + i * frame->linesize[1],
				frame->width);
			a += frame->width;
		}
		for (auto i = 0; i < frame->height / 2; i++)
		{
			memcpy(m_YuvBuf + a,
				frame->data[2] + i * frame->linesize[2],
				frame->width);
			a += frame->width;
		}
		pData = m_YuvBuf;
		*size = frame->height * frame->width * 3;
	}
	return true;
}

int videoDecoder::GetPixWidth()
{
	return pix_w;
}

int videoDecoder::GetPixHeight()
{
	return pix_h;
}
