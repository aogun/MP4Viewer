//
// Created by aogun on 2021/1/18.
//

#include "main_window.h"


main_window::main_window(mp4_manager* manager, const char * font_path) :
    m_manager(manager), m_atom_window(manager), m_field_window(manager), m_mem_window(manager) {
    if (font_path) m_font_path = font_path;
}


void main_window::ShowMenuFile()
{
    if (ImGui::MenuItem("Open", "Ctrl+O")) {
        MM_LOG_INFO("open file dialog");
//        m_file_dialog.Open();
        m_dialog_window.open_dialog();
    }
    if (m_manager->current()) {
        char sz[128];
        snprintf(sz, 128, "Close - [%s]", m_manager->current()->get_short_name());
        if (ImGui::MenuItem(sz, "Ctrl+W")) {
            MM_LOG_INFO("close file %s", m_manager->current()->get_name());
            m_manager->close_current();
        }
    } else {
        ImGui::MenuItem("Close", "Ctrl+W", false, false);
    }
    if (ImGui::MenuItem("Exit", "Alt+F4")) {
        m_running = false;
    }

}

void main_window::draw() {
    ImVec2 menu_size;
    bool open = true;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Detail window", nullptr, &m_manager->m_open_field_window);
            ImGui::MenuItem("Hex view window", nullptr, &m_manager->m_open_mem_window);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Theme"))
        {
            static int style = 0;
            bool selected;
            style == 0 ? selected = true : selected = false;
            if (ImGui::MenuItem("Light", nullptr, selected)) {
                ImGui::StyleColorsLight();
                style = 0;
            }
            style == 1 ? selected = true : selected = false;
            if (ImGui::MenuItem("Dark", nullptr, selected)) {
                ImGui::StyleColorsDark();
                style = 1;
            }
            style == 2 ? selected = true : selected = false;
            if (ImGui::MenuItem("Classic", nullptr, selected)) {
                ImGui::StyleColorsClassic();
                style = 2;
            }
            ImGui::EndMenu();
        }
        if (m_manager->current()) {
            if (ImGui::BeginMenu("Window"))
            {
                for (const auto &iter : *m_manager->files()) {
                    const auto &file = iter.second;
                    bool selected = false;
                    if (m_manager->current() == file) {
                        selected = true;
                    }
                    if (ImGui::MenuItem(file->get_short_name(), nullptr, selected)) {
                        if (!selected) {
                            m_manager->change(file->get_name());
                        }
                    }
                }
                ImGui::EndMenu();
            }
        } else {
            ImGui::BeginMenu("Window", false);
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("about", nullptr)) {

            }
            ImGui::EndMenu();
        }
        menu_size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
    auto &io = ImGui::GetIO();
    auto size = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, menu_size.y));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y - menu_size.y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("MP4Viewer", &open,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBringToFrontOnFocus |
                 ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PopStyleVar(3);

//    m_file_dialog.Display();
//    if(m_file_dialog.HasSelected())
//    {
//        MM_LOG_INFO("Selected filename %s", m_file_dialog.GetSelected().string().c_str());
//        m_manager->open(m_file_dialog.GetSelected().string().c_str());
//        m_file_dialog.ClearSelected();
//    }
    ImGui::End();

    if (m_dialog_window.is_selected()) {
        m_manager->open(m_dialog_window.get_path());
        m_dialog_window.clear();
    }

    if (io.KeyCtrl) {
        if (ImGui::IsKeyPressed('O')) {
            MM_LOG_INFO("open file dialog");
            io.KeysDown['O'] = false;
            m_dialog_window.open_dialog();
        }
        if (ImGui::IsKeyPressed('W')) {
            MM_LOG_INFO("close current file");
            m_manager->close_current();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
            MM_LOG_INFO("switch to next");
            m_manager->switch_next();
        }
    }

    m_mem_window.set_top(menu_size.y);
    m_mem_window.draw();

    m_atom_window.set_top(menu_size.y);
    m_atom_window.draw();

    m_field_window.set_top(menu_size.y);
    m_field_window.draw();

}

bool main_window::is_running() {
    return m_running;
}

bool main_window::init() {
    ImGuiIO& io = ImGui::GetIO();
    if (!m_font_path.empty()) {
        auto font = io.Fonts->AddFontFromFileTTF(m_font_path.c_str(), 18,
                                     nullptr, io.Fonts->GetGlyphRangesDefault());
        if (font == nullptr) {
            MM_LOG_ERROR("load font %s failed", m_font_path.c_str());
        }
    }
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsLight();
    io.IniFilename = nullptr;
    return false;
}
