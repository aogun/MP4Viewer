//
// Created by aogun on 2021/3/9.
//

#ifndef MP4VIEWER_MP4_AVDEC_H
#define MP4VIEWER_MP4_AVDEC_H

#include <string>
#include <thread>
#include <mutex>
#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#endif
#include "mp4_decoder.h"


class mp4_avdec : public mp4_decoder {
public:
    mp4_avdec() {}
    bool create(const char *path, std::shared_ptr<mp4_codec_info> codec) override;

    bool decode(std::shared_ptr<mp4_video_dec_task> task) override;

    decode_state_t get_state() override;

    std::string get_error() override;

    std::shared_ptr<video_frame> get_decoded_frame() override;

    void destroy() override;

    void stop() override;

private:
    void decode_loop();
    std::shared_ptr<video_frame> create_frame(AVFrame *frame);
    std::shared_ptr<video_frame> create_rgb_frame(AVFrame *frame);

    std::string m_path;
    std::string m_error;
    std::shared_ptr<mp4_codec_info> m_codec;


    decode_state_t m_state = DECODE_STATE_IDLE;

    FILE * m_file = nullptr;
    volatile bool m_running = true;
    std::shared_ptr<std::thread> m_thread;
    std::shared_ptr<mp4_video_dec_task> m_current_task;

    std::mutex m_mutex;
    std::condition_variable m_cond;

    SwsContext * m_sws_context = nullptr;
    float m_framerate = 0;
};


#endif //MP4VIEWER_MP4_AVDEC_H
