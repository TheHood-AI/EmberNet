#include "stdafx.h"
#include "videoplayer/videoplayer.h"
#include <fstream>

#pragma comment(lib, "winmm.lib")

#ifdef USE_VIDEO
using namespace Tga2D;
Tga2D::CVideoPlayer::CVideoPlayer()
{
	myVideoCodec = nullptr;
	myVideoCodecContext = nullptr;
	mySwsVideoContext = nullptr;
	myVideoFormatContext = nullptr;
	myAVVideoFrame = nullptr;
	myAVVideoFrameBGR = nullptr;
	myUIBuffer = nullptr;
	myAudioOutput = nullptr;
	myAudioStream = nullptr;
}

Tga2D::CVideoPlayer::~CVideoPlayer()
{

	// Free the RGB image
	if (myAVVideoFrameBGR)
	{
		av_free(myAVVideoFrameBGR);
	}
	

	// Free the YUV frame
	if (myAVVideoFrame)
	{
		av_free(myAVVideoFrame);
	}
	

	// Close the codecs
	if (myVideoCodecContext)
	{
		avcodec_close(myVideoCodecContext);
	}
	

	// Close the video file
	if (myVideoFormatContext)
	{
		avformat_close_input(&myVideoFormatContext);
	}
	
	if (myUIBuffer)
	{
		av_free(myUIBuffer);
	}


	if (mySwsVideoContext)
	{
		sws_freeContext(mySwsVideoContext);
	}

	if (myAudioOutput)
	{
		delete myAudioOutput;
		myAudioOutput = nullptr;
	}
}

bool Tga2D::CVideoPlayer::DoFirstFrame()
{
	GrabNextFrame();
	return true;
}



void Tga2D::CVideoPlayer::Stop()
{
	av_seek_frame(myVideoFormatContext, myVideoStreamIndex, 0, AVSEEK_FLAG_ANY);
}

double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )
double get_fps(AVFormatContext* aContext, int aStream)
{
	double eps_zero = 0.000025;
	double fps = r2d(aContext->streams[aStream]->r_frame_rate);

#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(52, 111, 0)
	if (fps < eps_zero)
	{
		fps = r2d(aContext->streams[aStream]->avg_frame_rate);
	}
#endif

	if (fps < eps_zero)
	{
		fps = 1.0 / r2d(aContext->streams[aStream]->codec->time_base);
	}

	return fps;
}



void Tga2D::CVideoPlayer::RestartStream()
{
	av_seek_frame(myVideoFormatContext, myVideoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
}

double Tga2D::CVideoPlayer::GetFps()
{
	if (!myVideoFormatContext)
	{
		return 0;
	}
	return get_fps(myVideoFormatContext, myVideoStreamIndex);
}



bool File_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	bool isGood = infile.good();
	infile.close();
	return isGood;
}


EVideoError CVideoPlayer::Init(const char* aPath, bool aPlayAudio)
{
	if (!File_exist(aPath))
	{
		return EVideoError_FileNotFound;
	}
	myFileName = std::string(aPath);

	av_register_all();
	
	myReturnResult = avformat_open_input(&myVideoFormatContext,
		myFileName.c_str(),
		NULL,
		NULL);



	if (myReturnResult >= 0)
	{
		myReturnResult = avformat_find_stream_info(myVideoFormatContext, NULL);

		if (myReturnResult >= 0)
		{
			for (unsigned int i = 0; i < myVideoFormatContext->nb_streams; i++)
			{
				if (myVideoFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					myVideoStreamIndex = i;

					myVideoCodecContext = myVideoFormatContext->streams[myVideoStreamIndex]->codec;
					if (myVideoCodecContext)
					{  
						myVideoCodec = avcodec_find_decoder(myVideoCodecContext->codec_id);
					}

				}
				else if (aPlayAudio && myVideoFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && !myAudioStream)
				{
					myAudioStream = myVideoFormatContext->streams[i];
					
				}
			}
		}
	}
	else
	{
		return EVideoError_WrongFormat;
	}

	if (myAudioStream)
	{
		myAudioCodecContext = myAudioStream->codec;
		myAudioCodecContext->codec = avcodec_find_decoder(myAudioCodecContext->codec_id);
		if (myAudioCodecContext->codec == NULL)
		{
			std::cout << "Couldn't find a proper decoder for audio" << std::endl;
			return EVideoError_WrongFormat;
		}
		else if (avcodec_open2(myAudioCodecContext, myAudioCodecContext->codec, NULL) != 0)
		{
			std::cout << "Couldn't open the context with the decoder for the audio" << std::endl;
			return EVideoError_WrongFormat;
		}

		int64_t outChannelLayout = AV_CH_LAYOUT_STEREO;
		AVSampleFormat outSampleFormat = AV_SAMPLE_FMT_S16; // Packed audio, non-planar (this is the most common format, and probably what you want; also, WAV needs it)
		int outSampleRate = 44100;
		myWavData.sampleRate = outSampleRate;
		myWavData.sampleSize = av_get_bytes_per_sample(outSampleFormat);
		myWavData.channels = (short)av_get_channel_layout_nb_channels(outChannelLayout);


		mySwsAudioContext = swr_alloc_set_opts(NULL,
			outChannelLayout,
			outSampleFormat,
			outSampleRate,
			av_get_default_channel_layout(myAudioCodecContext->channels),
			myAudioCodecContext->sample_fmt,
			myAudioCodecContext->sample_rate,
			0,
			NULL);

		if (mySwsAudioContext == NULL)
		{
			std::cout << "Couldn't create the SwrContext" << std::endl;
			return EVideoError_WrongFormat;
		}


		if (swr_init(mySwsAudioContext) != 0)
		{
			std::cout << "Couldn't initialize the SwrContext" << std::endl;
			return EVideoError_WrongFormat;

		}
		myAVAudioFrame = avcodec_alloc_frame();
	}

	if (myVideoCodec && myVideoCodecContext)
	{
		myReturnResult = avcodec_open2(myVideoCodecContext, myVideoCodec, NULL);

		if (myReturnResult >= 0)
		{		
			myAVVideoFrame = avcodec_alloc_frame();
			myAVVideoFrameBGR = avcodec_alloc_frame();

			AVPixelFormat format = AV_PIX_FMT_RGBA;

			myNumberOfBytes = avpicture_get_size(format,
				myVideoCodecContext->width,
				myVideoCodecContext->height);

			myUIBuffer = (uint8_t *)av_malloc(myNumberOfBytes * sizeof(uint8_t));

			

			avpicture_fill((AVPicture *)myAVVideoFrameBGR,
				myUIBuffer,
				format,
				myVideoCodecContext->width,
				myVideoCodecContext->height);
			
		

			mySwsVideoContext = sws_getContext(
				myVideoCodecContext->width,
				myVideoCodecContext->height,
				myVideoCodecContext->pix_fmt,
				myVideoCodecContext->width,
				myVideoCodecContext->height,
				format,
				SWS_BILINEAR,
				NULL,
				NULL,
				NULL);

		}
	}
	else
	{
		return EVideoError_WrongFormat;
	}

	return EVideoError_NoError;
}

int m_nBytes = 0;
bool isInited = false;
void CVideoPlayer::PlayAudioStream(const WavData& wav)

{
	static WAVEHDR WaveInHdr;
	static HWAVEOUT hWaveOut;
	if (!isInited)
	{
		int byteRate = wav.channels * wav.sampleRate * wav.sampleSize;

		short blockAlign = wav.channels * (short)wav.sampleSize;

		short bitsPerSample = (short)wav.sampleSize * 8;
		WAVEFORMATEX pFormat;
		pFormat.wFormatTag = WAVE_FORMAT_PCM; // simple, uncompressed format
		pFormat.nChannels = wav.channels; // 1=mono, 2=stereo
		pFormat.nSamplesPerSec = 44100;
		pFormat.nAvgBytesPerSec = byteRate; // = nSamplesPerSec * n.Channels * wBitsPerSample/8
		pFormat.nBlockAlign = blockAlign; // = n.Channels * wBitsPerSample/8
		pFormat.wBitsPerSample = bitsPerSample; // 16 for high quality, 8 for telephone-grade
		pFormat.cbSize = 0;

		myAudioOutput = new CWaveOut(&pFormat, 16, 4096);

		isInited = true;

	
	}
	for (int i=0; i< 1; i++)
	{
		myAudioOutput->Write((PBYTE)&wav.data[0], (int)wav.mySize);
	}
	
}



void processFrame(const AVFrame* frame, SwrContext* swrContext, WavData& wav)
{
	std::vector<unsigned char> buffer(wav.channels * wav.sampleRate * wav.sampleSize);
	unsigned char* pointers[SWR_CH_MAX] = { NULL };

	pointers[0] = &buffer[0];

	int numSamplesOut = swr_convert(swrContext,
		pointers,
		wav.sampleRate,
		(const unsigned char**)frame->extended_data,
		frame->nb_samples);

	if (numSamplesOut < 0)
	{
		// failed
		return;
	}

	if (numSamplesOut == 0)
	{
		// it consumed all the samples and is storing them in an internal buffer
		return;
	}

	wav.mySize = numSamplesOut * wav.sampleSize * wav.channels;
	SAFE_ARRAYDELETE(wav.data);
	wav.data = new unsigned char[numSamplesOut * wav.sampleSize * wav.channels];
	memcpy(wav.data, &buffer[0], numSamplesOut * wav.sampleSize * wav.channels);
}



int CVideoPlayer::GrabNextFrame()
{
	bool valid = false;
	int readFrame = 0;
	int safeValue = 0;
	const int maxSafeVal = 10000;

	while (!valid || safeValue > maxSafeVal)
	{
		safeValue++;
		readFrame = av_read_frame(myVideoFormatContext, &myAVPacket);
		if (readFrame >= 0)
		{
			if (myAVPacket.stream_index == myVideoStreamIndex)
			{
				myDecodedBytes = avcodec_decode_video2(
					myVideoCodecContext,
					myAVVideoFrame,
					&myGotFrame,
					&myAVPacket);

				


				if (myGotFrame)
				{
					valid = true;
				}
			}
			else if (myAudioStream && myAVPacket.stream_index == myAudioStream->index)
			{
				// Create a copy of the packet that we can modify and manipulate
				AVPacket decodingPacket = myAVPacket;
				while (decodingPacket.size > 0)
				{
					// Try to decode the packet into a frame
					int frameFinished = 0;
					int result = avcodec_decode_audio4(myAudioCodecContext, myAVAudioFrame, &frameFinished, &decodingPacket);

					if (result < 0 || frameFinished == 0)
					{
						break;
					}

					processFrame(myAVAudioFrame, mySwsAudioContext, myWavData);

					decodingPacket.size -= result;
					decodingPacket.data += result;

				}

				int numSamplesOut = 0;

				do
				{
					std::vector<unsigned char> buffer(myWavData.channels * myWavData.sampleRate * myWavData.sampleSize);

					unsigned char* pointers[SWR_CH_MAX] = { NULL };
					pointers[0] = &buffer[0];

					numSamplesOut = swr_convert(mySwsAudioContext,
						pointers,
						myWavData.sampleRate,
						NULL,
						0);

					if (numSamplesOut > 0)
					{
						// Copy the converted data from our temporary buffer to our Wav structure
						for (int i = 0; i < numSamplesOut * myWavData.sampleSize * myWavData.channels; ++i)
						{
							//myWavData.data.push_back(buffer[i]);
						}
						//myWavData.data = 
					}
				} while (numSamplesOut > 0);

				PlayAudioStream(myWavData);
				
			}
		}
		
		else
		{
			valid = true;
		}
		av_free_packet(&myAVPacket);
	}

	if (safeValue >= maxSafeVal)
	{
		ERROR_PRINT("Video error: Could not find a valid video frame!");
	}

	return readFrame;
}

bool CVideoPlayer::Update(unsigned int*& aBuffer, unsigned int aSizeX, unsigned int aSizeY)
{
	if (!myGotFrame)
	{
		return false;
	}
	if (mySwsVideoContext)
	{
		myReturnResult = sws_scale(
			mySwsVideoContext,
			myAVVideoFrame->data,
			myAVVideoFrame->linesize,
			0,
			myVideoCodecContext->height,
			myAVVideoFrameBGR->data,
			myAVVideoFrameBGR->linesize);

		if (myReturnResult > 0)
		{
			for (unsigned int i = (unsigned int)myVideoCodecContext->height; i< aSizeX; i++)
			{
				for (unsigned int j= (unsigned int)myVideoCodecContext->width;j< aSizeY; j++)
				{
					aBuffer[j * aSizeX + i] = 0x0000000;
				}
				
			}


			int rgbIndex = 0;
			for (int i = 0; i < myVideoCodecContext->height; i++)
			{
				for (int j = 0; j < myVideoCodecContext->width; j++)
				{
					{
						uint8_t* data = myAVVideoFrameBGR->data[0];
						aBuffer[i * aSizeX + j] = int((data[rgbIndex + 3]) << 24 |
							(data[rgbIndex + 2]) << 16 |
							(data[rgbIndex + 1]) << 8 |
							(data[rgbIndex]));

						rgbIndex += 4;
					}			
				}
			}
		}

	}





	myFrameCount++;
	return true;
}

void Tga2D::CVideoPlayer::Updateound()
{

}
#endif