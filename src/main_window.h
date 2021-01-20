//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_MAIN_WINDOW_H
#define MP4VIEWER_MAIN_WINDOW_H

#include "imgui.h"
#include "mm_log.h"
#include "imfilebrowser.h"
#include "mp4_manager.h"
#include "atom_window.h"
#include "field_window.h"
#include "mem_window.h"
#include "dialog_window.h"

class main_window {
public:
    explicit main_window(mp4_manager* manager, const char * font_path = nullptr);

    bool init();
    void draw();

    bool is_running();

private:
    void ShowMenuFile();

//    ImGui::FileBrowser m_file_dialog;
    bool m_running = true;
    mp4_manager * m_manager = nullptr;
    std::string m_font_path;
    atom_window m_atom_window;
    field_window m_field_window;
    mem_window m_mem_window;
    dialog_window m_dialog_window;
//    MemoryEditor m_mem_edit;
};


#endif //MP4VIEWER_MAIN_WINDOW_H
