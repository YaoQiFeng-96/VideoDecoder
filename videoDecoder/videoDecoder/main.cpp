#include <iostream>

#include "videoDecoder.h"

#pragma warning(disable:4996)


int main(int argc, char* argv[])
{
	std::cout << "hello world." << std::endl;

	videoDecoder videodecoder(AV_CODEC_ID_H264);
	if (!videodecoder.Init())
	{
		getchar();
		return -1;
	}
	char H264Buf[1024 * 512];
	int H264Lenth = 0;
	char frame_buf[1024] = { 0 };
	FILE * OutFile = fopen("test.YUV", "wb");
	FILE * InFile = fopen("bigbuckbunnynoB_480x272.h264", "rb");
	//FILE * InFile = fopen("bigbuckbunny_480x272.h265", "rb");
	int iYuvCount = 0;

	while (true)
	{
		int iReadSize = fread(frame_buf, 1, 512, InFile);
		if (iReadSize <= 0)
		{
			break;
		}
		memcpy(H264Buf + H264Lenth, frame_buf, iReadSize);
		H264Lenth += iReadSize;
		//获取一帧数据
		while (true)
		{
			bool OneFrame = false;
			if (H264Lenth <= 8)
			{
				break;
			}
			for (int i = 4; i < H264Lenth - 4; i++)
			{
				if (H264Buf[i] == 0x00 && H264Buf[i + 1] == 0x00 && H264Buf[i + 2] == 0x00 && H264Buf[i + 3] == 0x01)
				{
					videodecoder.AddData(H264Buf, i);
					while (true)
					{
						char * pOutData = NULL;
						int OutSize = 0;
						if (!videodecoder.GetData(pOutData, &OutSize))
						{
							break;
						}
						fwrite(pOutData, 1, OutSize, OutFile);
						iYuvCount++;
						if (10 == iYuvCount)
							goto end0;
					}
					H264Lenth -= i;
					memcpy(H264Buf, H264Buf + i, H264Lenth);
					OneFrame = true;
					break;
				}
			}
			if (OneFrame)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}
end0:
	printf("pasing end\r\n");
	getchar();
	return 0;

}