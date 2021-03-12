//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_MAIN_WINDOW_H
#define MP4VIEWER_MAIN_WINDOW_H

#include "imgui.h"
#include "mm_log.h"
#include "mp4_manager.h"
#include "atom_window.h"
#include "field_window.h"
#include "mem_window.h"
#include "dialog_window.h"
#include "video_window.h"

class main_window {
public:
    explicit main_window(mp4_manager* manager);

    bool init();
    void draw();
    void before_draw();

    bool is_running();

    void add_font(const char * name, const char * path);

    bool needs_rebuild_font();

    void enable_debug() { m_debug = true; }
private:
    void ShowMenuFile();
    bool change_font(const char * name);
    void add_big_font();

    bool m_open_log_window = false;

    bool m_running = true;
    mp4_manager * m_manager = nullptr;
    atom_window m_atom_window;
    field_window m_field_window;
    mem_window m_mem_window;
    dialog_window m_dialog_window;
    video_window m_video_window;
    std::map<std::string, ImFont *> m_fonts;
    std::map<std::string, std::string> m_font_path;
    std::string m_current_font;
    std::string m_changed_font;

    ImFont * m_big_font = nullptr;

    bool m_render_fps = false;
    ImFont * m_fps_font = nullptr;

    volatile bool m_needs_rebuild_font = false;
    bool m_debug = false;
};


#endif //MP4VIEWER_MAIN_WINDOW_H
