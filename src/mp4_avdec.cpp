//
// Created by aogun on 2021/3/9.
//

#include "mp4_avdec.h"
#include "mm_log.h"
#include <utility>


static bool m_init = false;
static std::mutex m_init_mutex;
uint32_t process_start_code(uint8_t * p, uint8_t size) {
    uint32_t ret = 0;
    for (int i = 0; i < size; i ++) {
        ret = (ret << 8) + p[i];
        p[i] = 0;
    }
    p[size - 1] = 1;
    return ret;
}

std::shared_ptr<mp4_decoder> create_mp4_decoder(const char *path,
                                                std::shared_ptr<mp4_codec_info> codec) {
    if (!m_init) {
        std::lock_guard<std::mutex> guard(m_init_mutex);
        if (!m_init) {
            av_register_all();
            av_log_set_level(AV_LOG_DEBUG);
            m_init = true;
        }
    }

    std::shared_ptr<mp4_decoder> ret;
    ret = std::make_shared<mp4_avdec>();
    if (!ret->create(path, std::move(codec))) {
        return std::shared_ptr<mp4_decoder>();
    }
    return ret;
}

bool mp4_avdec::create(const char *path, std::shared_ptr<mp4_codec_info> codec) {
    if (!codec) {
        MM_LOG_ERROR("no codec");
        return false;
    }
    if (codec->m_type != MP4_CODEC_H264 && codec->m_type != MP4_CODEC_HEVC) {
        MM_LOG_ERROR("unsupported codec %d", codec->m_type);
        return false;
    }
    m_file = fopen(path, "rb");
    if (!m_file) {
        MM_LOG_ERROR("open file %s failed", path);
        return false;
    }
    m_codec = std::move(codec);
    m_path = path;

    m_thread = std::make_shared<std::thread>(&mp4_avdec::decode_loop, this);
    return true;
}

bool mp4_avdec::decode(std::shared_ptr<mp4_video_dec_task> task) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_current_task) {
        m_current_task->m_running = false;
    }
    m_current_task = std::move(task);
    m_cond.notify_one();
    return true;
}

decode_state_t mp4_avdec::get_state() {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_current_task) return m_current_task->m_state;
    return DECODE_STATE_IDLE;
}

std::string mp4_avdec::get_error() {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_current_task) return m_current_task->m_error;
    return "no task";
}

std::shared_ptr<video_frame> mp4_avdec::get_decoded_frame() {
    return std::shared_ptr<video_frame>();
}

void mp4_avdec::destroy() {
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_running = false;
        if (m_current_task) m_current_task->m_running = false;
    }
    m_cond.notify_one();
    if (m_thread) {
        m_thread->join();
        m_thread.reset();
    }
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

void mp4_avdec::decode_loop() {
    std::shared_ptr<mp4_video_dec_task> task;
    MM_LOG_INFO("decode loop begin");
    while (m_running) {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (m_current_task) {
                task = m_current_task;
            }
        }
        if (task && task->m_state == DECODE_STATE_IDLE) {
            task->m_state = DECODE_STATE_PROCESSING;
            auto codec_id = m_codec->m_type == MP4_CODEC_H264 ?
                    AV_CODEC_ID_H264 : AV_CODEC_ID_HEVC;
            auto nalu_len = m_codec->m_nalu_length_size;
            AVFrame *last_frame = nullptr;
            AVCodecContext * context = nullptr;
            AVCodec *decoder = nullptr;
            do {
                decoder = avcodec_find_decoder(codec_id);
                if (!decoder) {
                    m_error = "Could not found decoder";
                    MM_LOG_ERROR(m_error.c_str());
                    break;
                }
                context = avcodec_alloc_context3(decoder);
                if (!context) {
                    m_error = "Could not allocate video codec context";
                    MM_LOG_ERROR(m_error.c_str());
                    break;
                }
                context->thread_count = 2;
                if (avcodec_open2(context, decoder, nullptr) < 0) {
                    m_error = "Could not open codec";
                    MM_LOG_ERROR(m_error.c_str());
                    break;
                }
//                if (m_codec && m_codec->m_codec_data) {
//                    AVPacket packet {};
//                    packet.size = m_codec->m_codec_data->size;
//                    packet.data = m_codec->m_codec_data->value;
//                    auto ret = avcodec_send_packet(context, &packet);
//                    if (ret < 0) {
//                        char sz[1024];
//                        av_strerror(ret, sz, 1024);
//                        MM_LOG_ERROR("send codec error: %s", sz);
//                    }
//
//                    AVFrame *frame = nullptr;
//                    frame = av_frame_alloc();
//                    ret = avcodec_receive_frame(context, frame);
//                    if (ret && ret != AVERROR(EAGAIN)) {
//                        char err_buf[1024];
//                        av_strerror(ret, err_buf, 1024);
//                        MM_LOG_INFO("decode error: %s", err_buf);
//                    }
//                }
                int index = 0;
                int count = task->input_frames->size();
                for (const auto pos : *(task->input_frames)) {
                    index ++;
                    if (!task->m_running) {
                        m_error = "cancel by user";
                        MM_LOG_ERROR(m_error.c_str());
                        break;
                    }
                    if (pos.size == 0) {
                        MM_LOG_WARN("invalid input frame size 0");
                        continue;
                    }
                    if (_fseeki64(m_file, pos.offset, SEEK_SET) != 0) {
                        m_error = string_format("invalid frame offset %lli", pos.offset);
                        MM_LOG_ERROR(m_error.c_str());
                        break;
                    }
                    int codec_size = 0;
                    if (m_codec && m_codec->m_codec_data && index == 1) {
                        codec_size = m_codec->m_codec_data->size;
                    }
                    std::unique_ptr<uint8_t[]> buf( new uint8_t[ pos.size + codec_size ] );
                    auto len = fread(buf.get() + codec_size, 1, pos.size, m_file);
                    if (len != pos.size) {
                        m_error = string_format("read frame at %lli size %u return %u",
                                                pos.offset, pos.size, len);
                        MM_LOG_ERROR(m_error.c_str());
                        break;
                    }
                    AVPacket packet {};
                    packet.size = pos.size;
                    packet.data = buf.get();
                    if (codec_size) memcpy(buf.get(), m_codec->m_codec_data->value, codec_size);
                    auto last = buf.get() + pos.size + codec_size - nalu_len;
                    auto ptr = buf.get() + codec_size;
                    while (ptr < last) {
                        auto s = process_start_code(ptr, nalu_len);
                        ptr += (s + nalu_len);
                    }

                    auto ret = avcodec_send_packet(context, &packet);
                    if (ret < 0) {
                        char sz[1024];
                        av_strerror(ret, sz, 1024);
                        m_error = string_format("Error during decoding: %s", sz);
                        MM_LOG_ERROR(m_error.c_str());
                        continue;
                    }
                    if (index == count) {
                        avcodec_send_packet(context, nullptr);
                    }

                    AVFrame *frame = nullptr;
                    do {
                        frame = av_frame_alloc();
                        ret = avcodec_receive_frame(context, frame);
                        if (ret && ret != AVERROR(EAGAIN)) {
                            char err_buf[1024];
                            av_strerror(ret, err_buf, 1024);
                            MM_LOG_INFO("decode error: %s", err_buf);
                            m_error = err_buf;
                            break;
                        }
                        if (ret == AVERROR(EAGAIN)) {
                            MM_LOG_INFO("needs more data");
                            break;
                        }
                        MM_LOG_INFO("decode frame suc.");
                        if (last_frame) {
                            av_frame_free(&last_frame);
                        }
                        last_frame = frame;
                    } while (false);
                }
            } while(false);
            if (!last_frame) {
                task->m_state = DECODE_STATE_FAILED;
                if (m_error.empty()) {
                    m_error = "no output frame";
                    MM_LOG_ERROR(m_error.c_str());
                }
                task->m_error = m_error;
            } else {
                MM_LOG_INFO("process last frame");
                task->output_frame = create_rgb_frame(last_frame);
                task->m_state = DECODE_STATE_FINISHED;

                av_frame_free(&last_frame);
            }
            if (context) {
                avcodec_free_context(&context);
            }
        } else {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock);
        }
    }
    MM_LOG_INFO("decode loop end");
}

void mp4_avdec::stop() {

}

std::shared_ptr<video_frame> mp4_avdec::create_frame(AVFrame *av_frame) {
    auto frame = std::make_shared<video_frame>();
    frame->width = av_frame->width;
    frame->height = av_frame->height;

    AVBufferRef * buf1 = av_frame_get_plane_buffer(av_frame, 0);
    AVBufferRef * buf2 = av_frame_get_plane_buffer(av_frame, 1);
    AVBufferRef * buf3 = av_frame_get_plane_buffer(av_frame, 2);
    auto y_size = av_frame->width * av_frame->height;
    auto size = y_size * 1.5;
    auto data = new uint8_t[size];
    int offset = 0;
    memcpy(data + offset, buf1->data, y_size);
    offset += y_size;
    memcpy(data + offset, buf2->data, y_size / 4);
    offset += y_size / 4;
    memcpy(data + offset, buf3->data, y_size / 4);

    {
        auto f = fopen("e:\\temp\\raw.i420", "wb");
        fwrite(data, 1, size, f);
        fclose(f);
    }
    frame->data = std::make_shared<mp4_buffer>(data, size, false);
    frame->format = MP4_RAW_I420;
    return frame;
}

std::shared_ptr<video_frame> mp4_avdec::create_rgb_frame(AVFrame *av_frame) {
    if (!m_sws_context) {
        m_sws_context = sws_getContext(av_frame->width, av_frame->height, AV_PIX_FMT_YUV420P,
                                       av_frame->width, av_frame->height, AV_PIX_FMT_RGBA,
                                       NULL, NULL, NULL, NULL);
    }
    int line_size[8] = {av_frame->linesize[0] * 4};
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, av_frame->width,
                                             av_frame->height, 1);
    auto data = (uint8_t*) malloc(num_bytes * sizeof(uint8_t));
    uint8_t* bgr_buffer[8] = {data};

    sws_scale(m_sws_context, av_frame->data, av_frame->linesize, 0,
              av_frame->height, bgr_buffer, line_size);

    auto frame = std::make_shared<video_frame>();
    frame->width = av_frame->width;
    frame->height = av_frame->height;

    {
        auto f = fopen("e:\\temp\\raw.rgba", "wb");
        fwrite(data, 1, num_bytes, f);
        fclose(f);
    }
    frame->data = std::make_shared<mp4_buffer>(data, num_bytes, false);
    frame->format = MP4_RAW_R8G8B8A8;
    return frame;
}

