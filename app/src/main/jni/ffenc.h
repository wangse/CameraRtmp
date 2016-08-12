#ifndef __FFENC__H__
#define __FFENC__H__

#include "ffcommon.h"
#include "ffraw.h"

namespace ff 
{
	int read_media_file(const char *filename, const char *outfile);
	int read_trancode(const char *filename, const char *outfile);

	struct AVEncodeContext
	{
		const char *_fileName;
		int _width;
		int _height;
		AVFormatContext *_ctx;
		AVStream *_video_st;
		AVStream * _audio_st;

		AVCtx _vctx;
		AVCtx _actx;

		int has_audio, has_video, encode_audio, encode_video, isopen;
		int _buffer_size; //ԭ�����ݻ������ߴ���kb
		int _nb_raws; //ԭ������֡����

		mutex_t * write_mutex;
	};

	/**
	 * ��������������
	 * ����������ļ�������Ƶ�ĳߴ�(w,h)
	 * video_codec_id Ҫʹ�õ���Ƶ����id
	 * frameRate ��Ƶ֡�ʣ�����������num=24000,den=1000,����24000,��ĸ1000��֡��24
	 * ʹ�����������Ա���С���������
	 * videoBitRate ��Ƶ������
	 * audio_codec_id Ҫʹ�õ���Ƶ����id
	 * sampleRate ��Ƶ������
	 * audioBitRate ��Ƶ������
	 *
	 */
	AVEncodeContext* ffCreateEncodeContext(
		const char* filename, const char *fmt,
		int w, int h, AVRational frameRate, int videoBitRate, AVCodecID video_codec_id,
		int in_w,int in_h,AVPixelFormat in_fmt,
		int sampleRate, int audioBitRate, AVCodecID audio_codec_id, 
		int in_ch, int in_sampleRate, AVSampleFormat in_sampleFmt,
		AVDictionary * opt_arg);

	/*
	 * ������Ƶÿ��֡�Ĳ�������,��ͨ����
	 */
	int ffGetAudioSamples(AVEncodeContext *pec);

	int ffGetAudioChannels(AVEncodeContext *pec);

	/*
	 * ��ʾ�������ݶ��Ѿ��������
	 */
	void ffFlush(AVEncodeContext *pec);

	/**
	 * �رձ���������
	 */
	void ffCloseEncodeContext(AVEncodeContext *pec);

	/**
	 * ������Ƶ֡������Ƶ֡
	 */
	int ffAddFrame(AVEncodeContext *pec, AVRaw *praw);


	/*
	 * ȡ�����С,��λkb
	 */
	int ffGetBufferSizeKB(AVEncodeContext *pec);

	/*
	 * ȡ��������С
	 */
	int ffGetBufferSize(AVEncodeContext *pec);

	int ffIsWaitingOrStop(AVEncodeContext *pec);
}
#endif