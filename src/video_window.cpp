//
// Created by aogun on 2021/3/10.
//

#include "video_window.h"
const char * time_code_str(uint32_t time_code) {
    static char sz[64];
    if (time_code == -1) return "N/A";

    auto sec = time_code >> 8;
    auto hour = sec / 3600;
    auto minute = (sec - (hour * 3600)) / 60;
    sec = sec % 60;
    auto frame = time_code & 0xFF;
    snprintf(sz, sizeof(sz) - 1,
             "%02d:%02d:%02d %03d", hour, minute, sec, frame);
    return sz;
}
video_window::video_window(mp4_manager *manager) {
    m_manager = manager;

}

void video_window::set_top(uint32_t top) {
    m_top = top;

}

void video_window::clear() {
    m_info = "Please choose frame in stsz";
    m_texture.reset();

}

void video_window::draw() {
    if (!m_manager) {
        MM_LOG_ERROR("no manager");
        return;
    }
    if (!m_manager->m_open_video_window) {
        return;
    }
    auto task = m_manager->get_video_task();
    if (task != m_task) {
        m_needs_flush = true;
        if (!task) {
            m_info = "Please choose frame in stsz";
        } else {
            m_info = "processing";
        }
        m_task = task;
        m_texture.reset();
//        if (m_task) {
//            m_title = string_format("Video Window [%s] ###Video Window", m_task->m_info.c_str());
//            MM_LOG_INFO("video title:%s", m_title.c_str());
//        } else {
//            m_title = "Video Window###Video Window";
//        }
    }
    if (m_needs_flush) {
        do {
            if (!m_task) {
                m_info = "please select video frame";
                m_needs_flush = false;
                break;
            }
            if (m_task->m_state == DECODE_STATE_FAILED) {
                m_info = m_task->m_error;
                m_needs_flush = false;
                break;
            }
            if (m_task->m_state == DECODE_STATE_FINISHED) {
                void * texture = nullptr;
                auto frame = m_task->output_frame;
                if (!frame) {
                    m_info = "no frame output";
                    m_needs_flush = false;
                    break;
                }
                if (!load_texture_from_memory_ex(frame->data->value, &texture,
                                                 frame->width, frame->height,
                                                 frame->format)) {
                    m_info = "load frame failed";
                    MM_LOG_ERROR(m_info.c_str());
                    m_needs_flush = false;
                    break;
                }
                m_texture = std::make_shared<mp4_texture>(texture, frame->width, frame->height);
                m_texture->time_code = frame->time_code;
                m_needs_flush = false;
                m_info = "OK";
            }
        } while (false);
    }
    auto &io = ImGui::GetIO();
    auto file = m_manager->current();
    auto size = io.DisplaySize;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.);
    auto pos = ImGui::GetMainViewport()->WorkPos;
    pos.x += 100;
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(size.x, size.y));

    ImGui::Begin(m_title.c_str(), &m_manager->m_open_video_window,
                 ImGuiWindowFlags_NoCollapse);

    if (m_texture && m_texture->texture && m_texture->height > 0) {
        auto min = ImGui::GetWindowContentRegionMin();
        auto max = ImGui::GetWindowContentRegionMax();
        auto width = max.x - min.x;
        auto height = max.y - min.y;
        if (!m_task->m_info.empty()) {
            ImGui::Text("Video width:%d, height:%d", m_texture->width,
                        m_texture->height);
            if (m_texture->time_code >= 0) {
                ImGui::SameLine();
                ImGui::Text(" time code:%s", time_code_str(m_texture->time_code));
            }
            ImGui::SameLine();
            ImGui::Text(" Debug info:%s", m_task->m_info.c_str());
            height -= ImGui::GetTextLineHeight();
        }
        auto img_size = ImVec2(width, height);
        if (height > 0) {
            auto window_ratio = width * 1.0 / height;
            auto image_ratio = m_texture->width * 1.0 / m_texture->height;
            if (window_ratio > image_ratio) {
                img_size.x = image_ratio * img_size.y;
            } else {
                img_size.y = img_size.x / image_ratio;
            }
            ImGui::Image(m_texture->texture, img_size);
        }
    } else {
        ImGui::PushFont(m_font);
        ImGui::Text("%s", m_info.c_str());
        ImGui::PopFont();
    }

    ImGui::End();
    ImGui::PopStyleVar(2);
}
