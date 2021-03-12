//
// Created by aogun on 2021/3/8.
//

#ifndef MP4VIEWER_MP4_DECODER_H
#define MP4VIEWER_MP4_DECODER_H

#include <memory>
#include <vector>
#include "common.h"

class frame_position_t {
public:
    frame_position_t(int64_t offset, uint32_t size) : offset(offset), size(size) {}

    int64_t offset = -1;
    uint32_t size = 0;
};

typedef enum {
    DECODE_STATE_IDLE = 0,
    DECODE_STATE_PROCESSING,
    DECODE_STATE_FAILED,
    DECODE_STATE_FINISHED,
} decode_state_t;

class video_frame {
public:
    std::shared_ptr<mp4_buffer> data;
    uint8_t * p0 = nullptr;
    uint8_t * p1 = nullptr;
    uint8_t * p2 = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;

    mp4_raw_type_t format = MP4_RAW_UNKNOWN;
};

using frame_pos_vector_t=std::shared_ptr<std::vector<frame_position_t>>;

class mp4_video_dec_task {
public:
    frame_pos_vector_t input_frames;
    std::string m_info;
    volatile bool m_running = true;
    volatile decode_state_t m_state = DECODE_STATE_IDLE;
    std::string m_error;
    std::shared_ptr<video_frame> output_frame;
};

class mp4_decoder {
public:
    virtual bool create(const char * path, std::shared_ptr<mp4_codec_info> codec) = 0;
    virtual bool decode(std::shared_ptr<mp4_video_dec_task> task) = 0;
    virtual decode_state_t get_state() = 0;
    virtual std::string get_error() = 0;
    virtual std::shared_ptr<video_frame> get_decoded_frame() = 0;
    virtual void destroy() = 0;
    virtual void stop() = 0;
};

std::shared_ptr<mp4_decoder> create_mp4_decoder(const char *path,
                                                std::shared_ptr<mp4_codec_info> codec);

#endif //MP4VIEWER_MP4_DECODER_H
