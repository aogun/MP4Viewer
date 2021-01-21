//
// Created by aogun on 2021/1/18.
//

#include "main_window.h"


main_window::main_window(mp4_manager* manager) :
    m_manager(manager), m_atom_window(manager), m_field_window(manager), m_mem_window(manager) {
    m_font_path["default"] = "";
}


void main_window::ShowMenuFile()
{
    if (ImGui::MenuItem("Open", "Ctrl+O")) {
        MM_LOG_INFO("open file dialog");
        m_dialog_window.open_dialog();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S", false, m_mem_window.m_modified)) {
        MM_LOG_INFO("save file");
        m_mem_window.save();
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
            if (ImGui::BeginMenu("windows")) {
                ImGui::MenuItem("Detail", nullptr, &m_manager->m_open_field_window);
                ImGui::MenuItem("Hex view", nullptr, &m_manager->m_open_mem_window);
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
            if (ImGui::BeginMenu("Fonts"))
            {
                for (const auto &iter : m_font_path) {
                    bool selected = iter.first == m_current_font;
                    if (ImGui::MenuItem(iter.first.c_str(), nullptr, selected)) {
                        m_changed_font = iter.first;
                    }
                }
                ImGui::EndMenu();
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
        if (ImGui::IsKeyPressed('S')) {
            if (m_mem_window.m_modified) {
                MM_LOG_INFO("save current file");
                m_mem_window.save();
            }
        }
        if (ImGui::IsKeyPressed('\t')) {
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

    change_font("default");
    m_mem_window.set_font(m_fonts["default"]);
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsLight();
    io.IniFilename = nullptr;
    return false;
}

void main_window::add_font(const char *name, const char *path) {
    m_font_path[name] = path;
}

bool main_window::change_font(const char *name) {
    ImGuiIO& io = ImGui::GetIO();
    auto iter = m_fonts.find(name);
    if (iter != m_fonts.end()) {
        io.FontDefault = iter->second;
        m_current_font = name;
        return true;
    }
    auto path_iter = m_font_path.find(name);
    if (path_iter == m_font_path.end()) {
        MM_LOG_ERROR("font %s not found", name);
        return false;
    }
    if (path_iter->first == "default") {
        auto font = io.Fonts->AddFontDefault();
        if (font == nullptr) {
            MM_LOG_ERROR("load default font failed");
            return false;
        }
        m_fonts[name] = font;
        io.FontDefault = font;
        m_current_font = name;
        return true;
    }
    auto path = path_iter->second;
    if (path.empty()) {
        MM_LOG_ERROR("font %s path empty", name);
        return false;
    }
    MM_LOG_ERROR("try to load font from %s", path.c_str());
    auto font = io.Fonts->AddFontFromFileTTF(path.c_str(), 16,
                                 nullptr, io.Fonts->GetGlyphRangesChineseFull());
    if (font == nullptr) {
        MM_LOG_ERROR("load font %s failed", path.c_str());
        return false;
    }

    m_fonts[name] = font;
    io.FontDefault = font;
    m_current_font = name;
    m_needs_rebuild_font = true;
    return true;
}

void main_window::before_draw() {
    if (!m_changed_font.empty()) {
        change_font(m_changed_font.c_str());
        m_changed_font.clear();
    }
}

bool main_window::needs_rebuild_font() {
    if (m_needs_rebuild_font) {
        m_needs_rebuild_font = false;
        return true;
    }
    return false;
}

