//
// Created by aogun on 2021/1/18.
//

#include "main_window.h"
#include "common.h"

struct ExampleAppLog
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.
    bool                Start;
    std::vector<std::string> LogBeforeStart;

    ExampleAppLog()
    {
        AutoScroll = true;
        Start = false;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, va_list arg_list)
    {
        if (!Start) {
            static char sz[1024];
            va_list args;
            vsnprintf(sz, 1024, fmt, arg_list);
            LogBeforeStart.emplace_back(sz);
            return;
        }
        if (!LogBeforeStart.empty()) {
            for (const auto &s : LogBeforeStart) {
                Buf.append(s.c_str(), s.c_str() + s.size());
            }
            LineOffsets.push_back(LogBeforeStart.size());
            LogBeforeStart.clear();
        }
        int old_size = Buf.size();
        Buf.appendfv(fmt, arg_list);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!LogBeforeStart.empty()) {
            for (const auto &s : LogBeforeStart) {
                Buf.append(s.c_str(), s.c_str() + s.size());
            }
            LineOffsets.push_back(LogBeforeStart.size());
            LogBeforeStart.clear();
        }
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
};
static ExampleAppLog m_log;
void add_log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    m_log.AddLog(fmt, args);
    va_end(args);
}


main_window::main_window(mp4_manager* manager) :
    m_manager(manager), m_atom_window(manager), m_field_window(manager), m_mem_window(manager),
    m_video_window(manager){
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
    auto &io = ImGui::GetIO();
    auto size = io.DisplaySize;
    ImGui::DockSpaceOverViewport();
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
                ImGui::MenuItem("Log", nullptr, &m_open_log_window);
                ImGui::MenuItem("FPS", nullptr, &m_render_fps);
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
                std::thread(display_about).detach();
            }
            ImGui::EndMenu();
        }
        menu_size = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
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

    m_video_window.set_top(menu_size.y);
    m_video_window.draw();

    if (m_debug)
        ImGui::ShowDemoWindow(&m_debug);

    if (m_open_log_window) {
        ImGui::SetNextWindowSize(ImVec2(size.x, 400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, size.y - 400), ImGuiCond_FirstUseEver);
        m_log.Draw("Log", &m_open_log_window);
    }
    if (m_render_fps) {
        auto pos = ImGui::GetMainViewport()->WorkPos;
        pos.x += (size.x - 80);
        pos.y += 20.0f;
        ImGui::SetNextWindowPos(pos);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("fps", nullptr, ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoScrollbar |
                                     ImGuiWindowFlags_AlwaysAutoResize |
                                     ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_NoFocusOnAppearing |
                                     ImGuiWindowFlags_NoNav);
        ImGui::PopStyleVar(3);
        ImGui::PushFont(m_fps_font);
        ImGui::TextColored((ImVec4) ImColor(0, 0xff, 0, 0xff), "%.1f", ImGui::GetIO().Framerate);
        ImGui::PopFont();
        ImGui::End();
    }
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
//    io.IniFilename = nullptr;

    ImFontConfig config;
    config.SizePixels = 30;
    m_fps_font = io.Fonts->AddFontDefault(&config);
    if (m_fps_font == nullptr) {
        MM_LOG_ERROR("load fps font failed");
        return false;
    }
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
        add_big_font();
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
    add_big_font();
    m_needs_rebuild_font = true;
    return true;
}

void main_window::before_draw() {
    if (!m_log.Start) m_log.Start = true;
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

void main_window::add_big_font() {
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config{};
    config.SizePixels = 24;
    m_big_font = io.Fonts->AddFontDefault(&config);
    m_video_window.set_font(m_big_font);
}

