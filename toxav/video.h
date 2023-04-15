/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright © 2016-2018 The TokTok team.
 * Copyright © 2013-2015 Tox project.
 */
#ifndef C_TOXCORE_TOXAV_VIDEO_H
#define C_TOXCORE_TOXAV_VIDEO_H

#include "toxav.h"


#include "../toxcore/logger.h"
#include "../toxcore/util.h"

#include "bwcontroller.h"
#include "ring_buffer.h"
#include "rtp.h"

// for VPX ----------
#include <vpx/vpx_decoder.h>
#include <vpx/vpx_encoder.h>
#include <vpx/vpx_image.h>
#include <vpx/vp8cx.h>
#include <vpx/vp8dx.h>
// for VPX ----------

#ifdef __cplusplus
extern "C" {
#endif
// for H264 ----------
#include <x264.h>
#include <libavcodec/avcodec.h>
#include <libavutil/common.h>
// for H264 ----------
#ifdef __cplusplus
}
#endif

// TODO: don't hardcode this, let the application choose it
// VPX Info: Time to spend encoding, in microseconds (it's a *soft* deadline)
#define WANTED_MAX_ENCODER_FPS (40)
#define MAX_ENCODE_TIME_US (1000000 / WANTED_MAX_ENCODER_FPS) // to allow x fps
/*
VPX_DL_REALTIME       (1)       deadline parameter analogous to VPx REALTIME mode.
VPX_DL_GOOD_QUALITY   (1000000) deadline parameter analogous to VPx GOOD QUALITY mode.
VPX_DL_BEST_QUALITY   (0)       deadline parameter analogous to VPx BEST QUALITY mode.
*/

#define AV_BUFFERING_MS_MIN 2
#define AV_BUFFERING_MS_MAX 800

#ifdef HW_CODEC_CONFIG_RPI3_TBW_TV
// more buffering for TV usecase
#undef MIN_AV_BUFFERING_MS
#undef AV_BUFFERING_DELTA_MS
#define MIN_AV_BUFFERING_MS 90
#define AV_BUFFERING_DELTA_MS 5
#endif

#define GUESS_REMOTE_ENCODER_DELAY_MS 100 // guess how long the remote end took to encode 1 video frame in ms

typedef enum PACKET_TOXAV_COMM_CHANNEL_FUNCTION {
    PACKET_TOXAV_COMM_CHANNEL_REQUEST_KEYFRAME = 0,
    PACKET_TOXAV_COMM_CHANNEL_HAVE_H264_VIDEO = 1,
    PACKET_TOXAV_COMM_CHANNEL_DUMMY_NTP_REQUEST = 3,
    PACKET_TOXAV_COMM_CHANNEL_DUMMY_NTP_ANSWER = 4,
} PACKET_TOXAV_COMM_CHANNEL_FUNCTION;


// Zoff --
// -- VP8 codec ----------------
#define VIDEO_CODEC_DECODER_INTERFACE_VP8 (vpx_codec_vp8_dx())
#define VIDEO_CODEC_ENCODER_INTERFACE_VP8 (vpx_codec_vp8_cx())
// -- VP9 codec ----------------
#define VIDEO_CODEC_DECODER_INTERFACE_VP9 (vpx_codec_vp9_dx())
#define VIDEO_CODEC_ENCODER_INTERFACE_VP9 (vpx_codec_vp9_cx())
// Zoff --

#define VIDEO_CODEC_DECODER_MAX_WIDTH  (800) // (16384) // thats just some initial dummy value
#define VIDEO_CODEC_DECODER_MAX_HEIGHT (600) // (16384) // so don't worry

#define VPX_MAX_ENCODER_THREADS (3)
#define VPX_MAX_DECODER_THREADS (1)
#define VIDEO__VP9E_SET_TILE_COLUMNS (1)
#define VIDEO__VP9E_SET_TILE_ROWS (1)
#define VIDEO__VP9_KF_MAX_DIST (60)
#define VIDEO__VP8_DECODER_ERROR_CONCEALMENT 0
#define VIDEO__VP8_DECODER_POST_PROCESSING_ENABLED 0 // 0, 1, 2, 3 # 0->none, 3->maximum
// #define VIDEO_CODEC_ENCODER_USE_FRAGMENTS 1
#define VIDEO_CODEC_FRAGMENT_NUMS (5)
// #define VIDEO_CODEC_FRAGMENT_VPX_NUMS VP8_ONE_TOKENPARTITION
#define VIDEO_CODEC_FRAGMENT_VPX_NUMS VP8_FOUR_TOKENPARTITION
// #define VIDEO_CODEC_FRAGMENT_VPX_NUMS VP8_EIGHT_TOKENPARTITION
#define VIDEO_MAX_FRAGMENT_BUFFER_COUNT (100)
#define TOXAV_ENCODER_VP8_RC_MAX_QUANTIZER_NORMAL 50
#define TOXAV_ENCODER_VP8_RC_MAX_QUANTIZER_HIGH 43
#define TOXAV_ENCODER_VP8_RC_MIN_QUANTIZER_NORMAL 2
#define TOXAV_ENCODER_VP8_RC_MIN_QUANTIZER_HIGH 0
#define TOXAV_ENCODER_VP_RC_RESIZE_UP_THRESH 60
#define TOXAV_ENCODER_VP_RC_RESIZE_DOWN_THRESH 30

// #define VIDEO_PTS_TIMESTAMPS 1

#define VIDEO_SEND_X_KEYFRAMES_FIRST (1) // force the first n frames to be keyframes!
#define VPX_MAX_DIST_START (100)


#ifdef VIDEO_CODEC_ENCODER_USE_FRAGMENTS
#define VIDEO_RINGBUFFER_BUFFER_ELEMENTS (8 * VIDEO_CODEC_FRAGMENT_NUMS) // this buffer has normally max. ~2 entry
#define VIDEO_RINGBUFFER_FILL_THRESHOLD (2 * VIDEO_CODEC_FRAGMENT_NUMS) // start decoding at lower quality
#define VIDEO_RINGBUFFER_DROP_THRESHOLD (5 * VIDEO_CODEC_FRAGMENT_NUMS) // start dropping incoming frames (except index frames)
#else
// -------------------------------------
//  can buffer ~ (VIDEO_RINGBUFFER_BUFFER_ELEMENTS * 40ms@25fps) --> can hold this much video data in ms for audio-to-video delay
#define VIDEO_RINGBUFFER_BUFFER_ELEMENTS (142) // this buffer has normally max. ~2 entry
// -------------------------------------
#define VIDEO_RINGBUFFER_FILL_THRESHOLD (2) // start decoding at lower quality
#define VIDEO_RINGBUFFER_DROP_THRESHOLD (5) // start dropping incoming frames (except index frames)
#endif

#define VIDEO_MIN_REQUEST_KEYFRAME_INTERVAL_MS_FOR_NF 5000 // x sec. between KEYFRAME requests
#define VIDEO_MIN_REQUEST_KEYFRAME_INTERVAL_MS_FOR_KF 1000 // y sec. between KEYFRAME requests

#define VIDEO_DECODER_SOFT_DEADLINE_AUTOTUNE 1
// #define VIDEO_DECODER_AUTOSWITCH_CODEC 1 // sometimes this does not work correctly
#define VIDEO_DECODER_MINFPS_AUTOTUNE (10)
#define VIDEO_DECODER_LEEWAY_IN_MS_AUTOTUNE (5)

// #define VIDEO_ENCODER_SOFT_DEADLINE_AUTOTUNE 1
#define VIDEO_ENCODER_MINFPS_AUTOTUNE (15)
#define VIDEO_ENCODER_LEEWAY_IN_MS_AUTOTUNE (10)

#define VPX_DECODER_USED TOXAV_ENCODER_CODEC_USED_VP8 // this will switch automatically

#define VIDEO_DECODER_SOFT_DEADLINE_AUTOTUNE_ENTRIES 20
#define VIDEO_ENCODER_SOFT_DEADLINE_AUTOTUNE_ENTRIES 20
#define VIDEO_INCOMING_FRAMES_GAP_MS_ENTRIES 20
#define VIDEO_DECODER_CAUSED_DELAY_MS_ENTRIES 20
#define VIDEO_BUF_MS_ENTRIES 20
#define VIDEO_BUF_MS_ENTRIES_LONG 200

#include <pthread.h>

struct TSBuffer;

#ifndef TOXAV_DEFINED
#define TOXAV_DEFINED
#undef ToxAV
typedef struct ToxAV ToxAV;
#endif /* TOXAV_DEFINED */

struct OMXContext;

typedef struct VCSession_s {
    /* encoding */
    vpx_codec_ctx_t encoder[1];
    uint32_t frame_counter;
    x264_t *h264_encoder;
    x264_picture_t h264_in_pic;
    x264_picture_t h264_out_pic;
    int h264_enc_width;
    int h264_enc_height;
    uint32_t h264_enc_bitrate;

// ------ ffmpeg encoder ------
    AVCodecContext *h264_encoder2;
    AVPacket *h264_out_pic2;
// ------ ffmpeg encoder ------

    char *encoder_codec_used_name;
    int x264_software_encoder_used;

    /* decoding */
    vpx_codec_ctx_t decoder[1];
    AVCodecContext *h264_decoder;
    struct TSBuffer *vbuf_raw; /* Un-decoded data */

    uint32_t tsb_range_ms;
    uint64_t linfts; /* Last received frame time stamp */
    uint32_t lcfd; /* Last calculated frame duration for incoming video payload */

    uint8_t show_own_video;
    uint64_t last_decoded_frame_ts;
    uint64_t last_encoded_frame_ts;
    uint8_t  flag_end_video_fragment;
    int32_t  last_seen_fragment_num;
    int32_t  last_seen_fragment_seqnum;
    uint16_t count_old_video_frames_seen;
    uint32_t last_requested_keyframe_ts;
    uint32_t last_sent_keyframe_ts;
    uint32_t decoder_soft_deadline[VIDEO_DECODER_SOFT_DEADLINE_AUTOTUNE_ENTRIES];
    uint8_t  decoder_soft_deadline_index;
    uint32_t encoder_soft_deadline[VIDEO_ENCODER_SOFT_DEADLINE_AUTOTUNE_ENTRIES];
    uint8_t  encoder_soft_deadline_index;
    uint32_t incoming_video_frames_gap_ms[VIDEO_INCOMING_FRAMES_GAP_MS_ENTRIES];
    uint8_t  incoming_video_frames_gap_ms_index;
    uint32_t incoming_video_frames_gap_last_ts;
    uint32_t incoming_video_frames_gap_ms_mean_value;

    uint32_t video_decoder_caused_delay_ms_array[VIDEO_DECODER_CAUSED_DELAY_MS_ENTRIES];
    uint8_t video_decoder_caused_delay_ms_array_index;
    uint32_t video_decoder_caused_delay_ms_mean_value;

    uint32_t video_buf_ms_array[VIDEO_BUF_MS_ENTRIES];
    uint8_t video_buf_ms_array_index;
    int32_t video_buf_ms_mean_value;

    uint32_t video_buf_ms_array_long[VIDEO_BUF_MS_ENTRIES_LONG];
    uint8_t video_buf_ms_array_index_long;
    int32_t video_buf_ms_mean_value_long;

    uint8_t send_keyframe_request_received;
    uint8_t h264_video_capabilities_received;

    int64_t timestamp_difference_to_sender__for_video;
    int64_t timestamp_difference_adjustment;
    uint32_t rountrip_time_ms;
    int32_t has_rountrip_time_ms;
    int32_t pinned_to_rountrip_time_ms;
    int32_t video_play_delay;
    int32_t video_play_delay_real;
    uint32_t video_frame_buffer_entries;
    uint64_t last_incoming_frame_ts;
    uint64_t last_parsed_h264_sps_ts;
    uint32_t parsed_h264_sps_profile_i;
    uint32_t parsed_h264_sps_level_i;
    uint32_t video_incoming_frame_orientation;
    uint32_t video_received_first_frame;

    uint32_t dummy_ntp_local_start;
    uint32_t dummy_ntp_local_end;
    uint32_t dummy_ntp_remote_start;
    uint32_t dummy_ntp_remote_end;

    // options ---
    int32_t video_encoder_cpu_used;
    int32_t video_encoder_cpu_used_prev;
    int32_t video_encoder_vp8_quality;
    int32_t video_encoder_vp8_quality_prev;
    int32_t video_rc_max_quantizer;
    int32_t video_rc_max_quantizer_prev;
    int32_t video_rc_min_quantizer;
    int32_t video_rc_min_quantizer_prev;
    int32_t video_keyframe_method;
    int32_t video_keyframe_method_prev;
    uint8_t video_bitrate_autoset;
    int32_t video_max_bitrate;
    int32_t video_min_bitrate;
    int32_t video_encoder_coded_used;
    int32_t video_encoder_coded_used_hw_accel;
    int32_t video_encoder_coded_used_prev;
    int32_t video_decoder_error_concealment;
    int32_t video_decoder_error_concealment_prev;
    int32_t video_decoder_codec_used;
    int32_t startup_video_timespan;
    uint8_t encoder_frame_has_record_timestamp;
    int32_t video_decoder_buffer_ms;
    int32_t video_decoder_add_delay_ms;
    int32_t client_video_capture_delay_ms;
    int32_t video_decoder_caused_delay_ms;
    int32_t remote_client_video_capture_delay_ms;
    int32_t video_encoder_frame_orientation_angle;
    int32_t global_decode_first_frame_delayed_by;
    uint64_t global_decode_first_frame_delayed_ms;
    int32_t global_decode_first_frame_got;
    // options ---

    void *vpx_frames_buf_list[VIDEO_MAX_FRAGMENT_BUFFER_COUNT];
    uint16_t fragment_buf_counter;

    Logger *log;
    ToxAV *av;
    uint32_t friend_number;
    uint32_t incoming_video_bitrate_last_changed;
    uint32_t incoming_video_bitrate_last_cb_ts;
    uint32_t network_round_trip_time_last_cb_ts;
    int32_t network_round_trip_adjustment;
    uint32_t last_requested_lower_fps_ts;

    /* Video frame receive callback */
    toxav_video_receive_frame_cb *vcb;
    void *vcb_user_data;
    toxav_video_receive_frame_pts_cb *vcb_pts;
    void *vcb_pts_user_data;
    toxav_video_receive_frame_h264_cb *vcb_h264;
    void *vcb_h264_user_data;

    pthread_mutex_t queue_mutex[1];
} VCSession;



VCSession *vc_new(Mono_Time *mono_time, const Logger *log, ToxAV *av, uint32_t friend_number,
                  toxav_video_receive_frame_cb *cb, void *cb_data);
void vc_kill(VCSession *vc);
uint8_t vc_iterate(VCSession *vc, Tox *tox, uint8_t skip_video_flag, uint64_t *a_r_timestamp,
                   uint64_t *a_l_timestamp,
                   uint64_t *v_r_timestamp, uint64_t *v_l_timestamp, BWController *bwc,
                   int64_t *timestamp_difference_adjustment_,
                   int64_t *timestamp_difference_to_sender_,
                   int32_t *video_has_rountrip_time_ms);
int vc_queue_message(Mono_Time *mono_time, void *vcp, struct RTPMessage *msg);
int vc_reconfigure_encoder(Logger *log, VCSession *vc, uint32_t bit_rate, uint16_t width, uint16_t height,
                           int16_t kf_max_dist);
int vc_reconfigure_encoder_bitrate_only(VCSession *vc, uint32_t bit_rate);

#endif // C_TOXCORE_TOXAV_VIDEO_H
