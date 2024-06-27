//
// Created by pengcheng.tan on 2024/6/27.
//
#include "tmediasubtitle.h"


tMediaOptResult tMediaSubtitleContext::setupNewSubtitleStream(AVStream *stream) {
    releaseLastSubtitleStream();
    if (subtitle_pkt == nullptr) {
        subtitle_pkt = av_packet_alloc();
    }
    if (subtitle_frame == nullptr) {
        subtitle_frame = new AVSubtitle;
    }
    releaseLastSubtitleStream();
    if (stream->codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
        LOGE("Wrong stream type: %d", stream->codecpar->codec_type);
        return OptFail;
    }
    subtitle_stream = stream;
    auto codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (codec == nullptr) {
        LOGE("Don't find subtitle codec.");
        return OptFail;
    }
    subtitle_decoder_ctx = avcodec_alloc_context3(codec);
    int ret = avcodec_parameters_to_context(subtitle_decoder_ctx, stream->codecpar);
    if (ret < 0) {
        LOGE("Attach params to ctx fail: %d", ret);
        return OptFail;
    }
    ret = avcodec_open2(subtitle_decoder_ctx, codec, nullptr);
    if (ret < 0) {
        LOGE("Open decoder fail: %d", ret);
        return OptFail;
    }
    return OptSuccess;
}

tMediaDecodeResult tMediaSubtitleContext::decodeSubtitle(AVPacket *pkt) {
    if (pkt != nullptr) {
        if (pkt->stream_index != subtitle_stream->index) {
            LOGE("Wrong subtitle stream index");
            return DecodeFail;
        }
    }
    av_packet_unref(subtitle_pkt);
    if (pkt != nullptr) {
        av_packet_move_ref(subtitle_pkt, pkt);
    }
    int got_frame = 0;
    int ret = avcodec_decode_subtitle2(subtitle_decoder_ctx, subtitle_frame, &got_frame, subtitle_pkt);
    if (ret < 0) {
        LOGE("Decode subtitle fail: %d", ret);
        return DecodeFail;
    }
    if (got_frame) {
        if (subtitle_pkt->data) {
            LOGD("Decode subtitle success and skip next pkt.");
            return DecodeSuccessAndSkipNextPkt;
        } else {
            return DecodeSuccess;
        }
    } else {
        if (pkt->data) {
            LOGE("Decode subtitle fail: %d", ret);
            return DecodeFail;
        } else {
            LOGD("Decode subtitle end.");
            return DecodeEnd;
        }
    }
}

void tMediaSubtitleContext::flushDecoder() {
    if (subtitle_decoder_ctx != nullptr) {
        avcodec_flush_buffers(subtitle_decoder_ctx);
    }
}

void tMediaSubtitleContext::releaseLastSubtitleStream() {
    if (subtitle_decoder_ctx != nullptr) {
        avcodec_free_context(&subtitle_decoder_ctx);
        subtitle_decoder_ctx = nullptr;
    }
    subtitle_stream = nullptr;
}

void tMediaSubtitleContext::release() {
    releaseLastSubtitleStream();
    if (subtitle_pkt != nullptr) {
        av_packet_unref(subtitle_pkt);
        av_packet_free(&subtitle_pkt);
    }
    if (subtitle_frame != nullptr) {
        avsubtitle_free(subtitle_frame);
        subtitle_frame = nullptr;
    }
    free(this);
}
