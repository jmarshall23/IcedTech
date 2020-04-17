/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "engine_precompiled.h"
#pragma hdrstop

#include "tr_local.h"

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

extern "C"
{

	//#ifdef WIN32
	#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
	#endif
	//#include <inttypes.h>
	//#endif

	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

/*
=======================================

For Darklight Arena we are using FFMPEG to load H264 movies.

We are using code from 
https://github.com/RobertBeckebans/RBDOOM-3-BFG/blob/master/neo/renderer/Cinematic.cpp

=======================================
*/
class idCinematicLocal : public idCinematic {
public:
							idCinematicLocal();
	virtual					~idCinematicLocal();

	virtual bool			InitFromFile( const char *qpath, bool looping );
	virtual cinData_t		ImageForTime( int milliseconds );
	virtual int				AnimationLength();
	virtual void			Close();
	virtual void			ResetTime(int time);
private:
	int						video_stream_index;
	AVFormatContext* fmt_ctx;
	AVFrame* frame;
	AVFrame* frame2;
	AVCodec* dec;
	AVCodecContext* dec_ctx;
	SwsContext* img_convert_ctx;
	bool					hasFrame;
	long					framePos;

	bool					looping;

	int						startTime;
	int						CIN_WIDTH;
	int						CIN_HEIGHT;

	int						animationLength;
	int						frameRate;

	idImage*				img;

	byte					*image;

	cinStatus_t				status;

	cinData_t				ImageForTimeFFMPEG(int milliseconds);
	bool					InitFromFFMPEGFile(const char* qpath, bool looping);
	void					FFMPEGReset();
};

/*
========================
idCinematicLocal::idCinematicLocal
========================
*/
idCinematicLocal::idCinematicLocal() {
	//	fmt_ctx = avformat_alloc_context();
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
	frame = av_frame_alloc();
	frame2 = av_frame_alloc();
#else
	frame = avcodec_alloc_frame();
	frame2 = avcodec_alloc_frame();
#endif // LIBAVCODEC_VERSION_INT
	dec_ctx = NULL;
	fmt_ctx = NULL;
	video_stream_index = -1;
	img_convert_ctx = NULL;
	hasFrame = false;
	looping = false;
	startTime = 0;
	CIN_WIDTH = 0;
	CIN_HEIGHT = 0;
	animationLength = 0;
	frameRate = 0;
	image = NULL;
	status = FMV_EOF;
	img = NULL;
}

/*
========================
idCinematicLocal::~idCinematicLocal
========================
*/
idCinematicLocal::~idCinematicLocal() {
#if defined(_WIN32) || defined(_WIN64)
	av_frame_free(&frame);
	av_frame_free(&frame2);
#else
	av_freep(&frame);
	av_freep(&frame2);
#endif

	if (fmt_ctx)
	{
		avformat_free_context(fmt_ctx);
	}

	if (img_convert_ctx)
	{
		sws_freeContext(img_convert_ctx);
	}

	if(image) {
		Mem_Free(image);
		image = NULL;
	}

	if(img) {
		img->PurgeImage();
	}
}

/*
========================
idCinematicLocal::InitFromFile
========================
*/
bool idCinematicLocal::InitFromFile(const char* qpath, bool looping) {
	int ret;

	this->looping = looping;
	startTime = 0;

	idStr fileFullPath = qpath;
	fileFullPath.StripFilename();
	fileFullPath.StripFileExtension();

	idStr fullpath;
	idFile* testFile = fileSystem->OpenFileRead(qpath);
	if (testFile)
	{
		fullpath = testFile->GetFullPath();
		fileSystem->CloseFile(testFile);
	}

	if ((ret = avformat_open_input(&fmt_ctx, fullpath, NULL, NULL)) < 0)
	{
		common->Warning("idCinematic: Cannot open FFMPEG video file: '%s', %d\n", qpath, looping);
		return false;
	}
	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
	{
		common->Warning("idCinematic: Cannot find stream info: '%s', %d\n", qpath, looping);
		return false;
	}
	/* select the video stream */
	ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
	if (ret < 0)
	{
		common->Warning("idCinematic: Cannot find a video stream in: '%s', %d\n", qpath, looping);
		return false;
	}
	video_stream_index = ret;
	dec_ctx = fmt_ctx->streams[video_stream_index]->codec;
	/* init the video decoder */
	if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0)
	{
		common->Warning("idCinematic: Cannot open video decoder for: '%s', %d\n", qpath, looping);
		return false;
	}

	CIN_WIDTH = dec_ctx->width;
	CIN_HEIGHT = dec_ctx->height;
	/** Calculate Duration in seconds
	  * This is the fundamental unit of time (in seconds) in terms
	  * of which frame timestamps are represented. For fixed-fps content,
	  * timebase should be 1/framerate and timestamp increments should be
	  * identically 1.
	  * - encoding: MUST be set by user.
	  * - decoding: Set by libavcodec.
	  */
	AVRational avr = dec_ctx->time_base;
	/**
	 * For some codecs, the time base is closer to the field rate than the frame rate.
	 * Most notably, H.264 and MPEG-2 specify time_base as half of frame duration
	 * if no telecine is used ...
	 *
	 * Set to time_base ticks per frame. Default 1, e.g., H.264/MPEG-2 set it to 2.
	 */
	int ticksPerFrame = dec_ctx->ticks_per_frame;
	float durationSec = static_cast<double>(fmt_ctx->streams[video_stream_index]->duration)* static_cast<double>(ticksPerFrame) / static_cast<double>(avr.den);
	animationLength = durationSec * 1000;
	frameRate = av_q2d(fmt_ctx->streams[video_stream_index]->avg_frame_rate);
	hasFrame = false;
	framePos = -1;
	common->Printf("Loaded FFMPEG file: '%s', looping=%d%dx%d, %f FPS, %f sec\n", qpath, looping, CIN_WIDTH, CIN_HEIGHT, frameRate, durationSec);
	image = (byte*)Mem_Alloc(CIN_WIDTH * CIN_HEIGHT * 4 * 2);
	avpicture_fill((AVPicture*)frame2, image, AV_PIX_FMT_BGR32, CIN_WIDTH, CIN_HEIGHT);
	if (img_convert_ctx)
	{
		sws_freeContext(img_convert_ctx);
	}
	img_convert_ctx = sws_getContext(dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt, CIN_WIDTH, CIN_HEIGHT, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);
	status = FMV_PLAY;

	idImageOpts opts;
	opts.format = FMT_RGBA8;
	opts.colorFormat = CFM_DEFAULT;
	opts.numLevels = 1;
	opts.textureType = TT_2D;
	opts.isPersistant = false;
	opts.width = dec_ctx->width;
	opts.height = dec_ctx->height;
	opts.numMSAASamples = 0;
	img = renderSystem->CreateImage(va("_%s_cinematic", fileFullPath.c_str()), &opts, TF_LINEAR);

	startTime = 0;
	ImageForTime(0);
	status = (looping) ? FMV_PLAY : FMV_IDLE;

	//startTime = Sys_Milliseconds();

	return true;
}

/*
======================
idCinematic::FFMPEGReset
======================
*/
void idCinematicLocal::FFMPEGReset()
{
	// RB: don't reset startTime here because that breaks video replays in the PDAs
	//startTime = 0;

	framePos = -1;

	if (av_seek_frame(fmt_ctx, video_stream_index, 0, 0) >= 0)
	{
		status = FMV_LOOPED;
	}
	else
	{
		status = FMV_EOF;
	}
}

/*
======================
idCinematic::ImageForTime
======================
*/
cinData_t idCinematicLocal::ImageForTime(int milliseconds) {
	cinData_t	cinData;

	if (milliseconds <= 0)
	{
		milliseconds = Sys_Milliseconds();
	}

	memset(&cinData, 0, sizeof(cinData));
	if (r_skipDynamicTextures.GetBool() || status == FMV_EOF || status == FMV_IDLE)
	{
		return cinData;
	}

	if (!fmt_ctx)
	{
		// RB: .bik requested but not found
		return cinData;
	}

	if ((!hasFrame) || startTime == -1)
	{
		if (startTime == -1)
		{
			FFMPEGReset();
		}
		startTime = milliseconds;
	}

	long desiredFrame = ((milliseconds - startTime) * frameRate) / 1000;
	if (desiredFrame < 0)
	{
		desiredFrame = 0;
	}

	if (desiredFrame < framePos)
	{
		FFMPEGReset();
	}

	if (hasFrame && desiredFrame == framePos)
	{
		cinData.imageWidth = CIN_WIDTH;
		cinData.imageHeight = CIN_HEIGHT;
		cinData.status = status;
		cinData.image = img;
		return cinData;
	}

	AVPacket packet;
	while (framePos < desiredFrame)
	{
		int frameFinished = 0;

		// Do a single frame by getting packets until we have a full frame
		while (!frameFinished)
		{
			// if we got to the end or failed
			if (av_read_frame(fmt_ctx, &packet) < 0)
			{
				// can't read any more, set to EOF
				status = FMV_EOF;
				if (looping)
				{
					desiredFrame = 0;
					FFMPEGReset();
					framePos = -1;
					startTime = milliseconds;
					if (av_read_frame(fmt_ctx, &packet) < 0)
					{
						status = FMV_IDLE;
						return cinData;
					}
					status = FMV_PLAY;
				}
				else
				{
					status = FMV_IDLE;
					return cinData;
				}
			}
			// Is this a packet from the video stream?
			if (packet.stream_index == video_stream_index)
			{
				// Decode video frame
				avcodec_decode_video2(dec_ctx, frame, &frameFinished, &packet);
			}
			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
		}

		framePos++;
	}

	// We have reached the desired frame
	// Convert the image from its native format to RGB
	sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, dec_ctx->height, frame2->data, frame2->linesize);
	cinData.imageWidth = CIN_WIDTH;
	cinData.imageHeight = CIN_HEIGHT;
	cinData.status = status;
	img->UploadScratch(image, CIN_WIDTH, CIN_HEIGHT);
	hasFrame = true;
	cinData.image = img;

	return cinData;
}

/*
======================
idCinematic::AnimationLength
======================
*/
int idCinematicLocal::AnimationLength() {
	return animationLength;
}

/*
======================
idCinematic::Close
======================
*/
void idCinematicLocal::Close() {
	if (image)
	{
		Mem_Free((void*)image);
		image = NULL;
		status = FMV_EOF;
	}

	hasFrame = false;

	if (img_convert_ctx)
	{
		sws_freeContext(img_convert_ctx);
	}

	img_convert_ctx = NULL;

	if (dec_ctx)
	{
		avcodec_close(dec_ctx);
	}

	if (fmt_ctx)
	{
		avformat_close_input(&fmt_ctx);
	}
	status = FMV_EOF;
}

/*
======================
idCinematic::ResetTime
======================
*/
void idCinematicLocal::ResetTime(int time) {
	startTime = time; //originally this was: ( backEnd.viewDef ) ? 1000 * backEnd.viewDef->floatTime : -1;
	status = FMV_PLAY;
}

/*
======================
idCinematic::InitCinematic
======================
*/
void idCinematic::InitCinematic(void) {
	avcodec_register_all();
	av_register_all();
}

/*
======================
idCinematic::ShutdownCinematic
======================
*/
void idCinematic::ShutdownCinematic(void) {

}

/*
======================
idCinematic::Alloc
======================
*/
idCinematic* idCinematic::Alloc() {
	return new idCinematicLocal();
}