#pragma once
#include "..\engine_defines.h"

#ifdef USE_VIDEO
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "ffmpeg-2.0/libavcodec/avcodec.h"
#include "ffmpeg-2.0/libavdevice/avdevice.h"
#include "ffmpeg-2.0/libavfilter/avfilter.h"
#include "ffmpeg-2.0/libavformat/avformat.h"
#include "ffmpeg-2.0/libavformat/avio.h"
#include "ffmpeg-2.0/libavutil/avutil.h"
#include "ffmpeg-2.0/libpostproc/postprocess.h"
#include "ffmpeg-2.0/libswresample/swresample.h"
#include "ffmpeg-2.0/libswscale/swscale.h"
#include <wtypes.h>
#ifdef __cplusplus
} // end extern "C".
#endif // __cplusplus

#include <tga2d/videoplayer/video_audio.h>
enum EVideoError
{
	EVideoError_NoError = 0,
	EVideoError_WrongFormat,
	EVideoError_FileNotFound,
};

namespace Tga2D
{

	struct WavData
	{
		short channels;
		int sampleRate;
		int sampleSize; // in bytes
		unsigned char* data; // packed PCM sample data
		int mySize = 0;
	};

	class CVideoPlayer
	{
	public:
		CVideoPlayer();
		~CVideoPlayer();
		EVideoError Init(const char* aPath, bool aPlayAudio);
		bool DoFirstFrame();
		bool Update(unsigned int*& aBuffer, unsigned int aSizeX = 0, unsigned int aSizeY = 0);
		void Updateound();

		void Stop();
		int GrabNextFrame();
		void RestartStream();

		double GetFps();

		void PlayAudioStream(const WavData& wav);

		AVCodec*            myVideoCodec;

		AVCodecContext*     myVideoCodecContext;
		AVCodecContext*		myAudioCodecContext;
		SwsContext*         mySwsVideoContext;
		SwrContext*			mySwsAudioContext;
		AVFormatContext*    myVideoFormatContext;
		
		AVFrame*            myAVVideoFrame;
		AVFrame*            myAVVideoFrameBGR;
		AVFrame*			myAVAudioFrame;
		AVPacket            myAVPacket;
		std::string         myFileName;
		uint8_t*            myUIBuffer;
		int                 myReturnResult;
		int                 myFrameCount = 0;
		int                 myGotFrame;
		int                 myDecodedBytes;
		int                 myVideoStreamIndex;
		int                 myNumberOfBytes;
		AVStream*			myAudioStream = NULL;
		WavData				myWavData;
		CWaveOut*			myAudioOutput;
	};
}
#endif