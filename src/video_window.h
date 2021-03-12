//
// Created by aogun on 2021/3/10.
//

#ifndef MP4VIEWER_VIDEO_WINDOW_H
#define MP4VIEWER_VIDEO_WINDOW_H

#include "atom_obj.h"
#include "my_inspect.h"
#include "mp4_manager.h"
#include "imgui.h"
#include "res.h"

class video_window {
public:
    explicit video_window(mp4_manager * manager);

    void set_top(uint32_t top);
    virtual void draw();

    void set_font(ImFont * font) { m_font = font; }

    void clear();
private:
    mp4_manager * m_manager = nullptr;
    uint32_t m_top = 0;

    bool m_needs_flush = false;
    std::string m_info = "Please open file";
    std::string m_title = "Video Window";
    std::shared_ptr<video_frame> m_video_frame;

    ImFont * m_font = nullptr;
    std::shared_ptr<mp4_video_dec_task> m_task;

    std::shared_ptr<mp4_texture> m_texture;
};


#endif //MP4VIEWER_VIDEO_WINDOW_H
