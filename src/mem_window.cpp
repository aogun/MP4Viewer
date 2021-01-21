//
// Created by aogun on 2021/1/19.
//

#include "mem_window.h"

static mem_window * m_mem_window = nullptr;

void write_cb(ImU8* mem_data, size_t addr, ImU8 data_input_value) {
    if (m_mem_window) {
        m_mem_window->write_cb(mem_data, addr, data_input_value);
    }
}

mem_window::mem_window(mp4_manager *manager) {
    m_manager = manager;
    m_editor.OptGreyOutZeroes = false;
    m_editor.ReadOnly = false;
    m_editor.HighlightColor = IM_COL32(0, 0, 255, 50);
    m_editor.WriteFn = ::write_cb;

    strcpy_s(m_title, "Hex View###");
    m_mem_window = this;
}

void mem_window::set_top(uint32_t top) {
    m_top = top;
    auto &io = ImGui::GetIO();
    auto size = io.DisplaySize;

    m_editor.DefaultHeight = size.y - m_top;
}
void mem_window::draw() {
    if (!m_manager) {
        MM_LOG_ERROR("no manager");
        return;
    }
    if (!m_manager->m_open_mem_window) {
        return;
    }
    check_data();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.);
    if (!m_error.empty()) {
        ImGui::SetNextWindowPos(ImVec2(800, m_top), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(m_title, &m_manager->m_open_mem_window, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::TextColored((ImVec4)(ImColor(255, 0, 0)), "%s", m_error.c_str());
        }
        ImGui::End();
    } else {
        if (m_highlight_size > 0) {

            ImGui::SetNextWindowPos(ImVec2(800, m_top), ImGuiCond_FirstUseEver);
            m_editor.DrawWindow(m_title, m_buffer, m_size, m_buffer_offset, &m_manager->m_open_mem_window,
                                m_font, ImGuiWindowFlags_NoBringToFrontOnFocus);
            if (m_editor.HighlightMin != m_highlight_offset - m_buffer_offset) {
                m_editor.GotoAddrAndHighlight(m_highlight_offset - m_buffer_offset,
                                              m_highlight_offset - m_buffer_offset + m_highlight_size);
                MM_LOG_INFO("change hightlight range to %llu - %llu", m_highlight_offset - m_buffer_offset,
                            m_highlight_offset - m_buffer_offset + m_highlight_size);
            }
        }
    }
    ImGui::PopStyleVar(2);

}

mem_window::~mem_window() {
    if (m_buffer) {
        delete[] m_buffer;
        m_buffer = nullptr;
    }
}

void mem_window::check_data() {

    auto f = m_manager->current();
    if (!f) {
        m_highlight_offset = 0;
        m_highlight_size = 0;
        m_size = 0;
        m_buffer_offset = 0;
        m_current_file.reset();

        strcpy_s(m_title, "Hex View###");
        return;
    }

    uint64_t offset;
    uint32_t size;
    f->get_current(offset, size);
    if (size == 0) {
        m_highlight_offset = 0;
        m_highlight_size = 0;
        m_size = 0;
        m_buffer_offset = 0;
    };
    bool reload = false;
    do {
        if (m_current_file == f) {
            if (m_highlight_offset == offset) {
                break;
            }
            if (!m_error.empty()) {
                reload = true;
                break;
            }
            if (offset + size <= m_buffer_offset + m_size && offset >= m_buffer_offset) {
                m_highlight_offset = offset;
                m_highlight_size = size;
                break;
            }
            // no error, offset changed and out of buffer range, reload buffer
            reload = true;

        } else {
            // file change, reload buffer
            reload = true;
            m_highlight_offset = 0;
            m_highlight_size = 0;
            m_size = 0;
            m_buffer_offset = 0;
            m_current_file = f;

            snprintf(m_title, sizeof(m_title), "Hex View - [%s]###", f->get_short_name());
        }
    } while (false);


    if (reload) {
        if (m_modified) {
            m_modified = false;
            snprintf(m_title, sizeof(m_title), "Hex View - [%s]###", f->get_short_name());
        }
        if (!m_buffer) {
            m_buffer = new char[MAX_BUFFER_ALLOC_SIZE];
        }
        m_size = 0;
        m_error.clear();

        auto handle = m_current_file->get_handle();
        if (handle) {
            _fseeki64(handle, 0, SEEK_END);
            auto left = _ftelli64(handle);
            m_buffer_offset = offset > 2048 ? offset - 2048 : 0;
            left -= m_buffer_offset;
            m_size = left > MAX_BUFFER_ALLOC_SIZE ? MAX_BUFFER_ALLOC_SIZE : left;

            _fseeki64(handle, m_buffer_offset, SEEK_SET);

            m_highlight_offset = offset;
            m_highlight_size = offset + size > m_buffer_offset + m_size ? m_size - (offset - m_buffer_offset) : size;

            int64_t read = fread(m_buffer, 1, m_size, handle);
            if (read != m_size) {
                char sz[1024];
                snprintf(sz, 128, "read file %s from offset %llu size %d return %lli",
                         m_current_file->get_short_name(), m_buffer_offset, m_size, read);
                MM_LOG_ERROR(sz);
                m_error = sz;
                return;
            }

        } else {
            m_error = m_current_file->get_error();
        }
    }

}

void mem_window::write_cb(ImU8 *mem_data, size_t addr, ImU8 data_input_value) {
    if (!m_modified) {
        m_modified_range.begin = m_modified_range.end = -1;
        auto f = m_manager->current();
        if (f)
            snprintf(m_title, sizeof(m_title), "Hex View - [%s] *###", f->get_short_name());
    }
    m_modified = true;
    mem_data[addr] = (ImU8)data_input_value;
    m_buffer[addr] = (ImU8)data_input_value;
    MM_LOG_INFO("edit addr %d to %d", addr, data_input_value);

    if (m_modified_range.begin == -1) {
        m_modified_range.begin = addr;
        m_modified_range.end = addr + 1;
    } else {
        if (addr < m_modified_range.begin) m_modified_range.begin = addr;
        if (m_modified_range.end < addr + 1) m_modified_range.end = addr + 1;
    }
    MM_LOG_INFO("m_modified_range %u to %u, base:%llu", m_modified_range.begin, m_modified_range.end,
                m_buffer_offset);
}

void mem_window::save() {
    if (!m_modified) return;

    auto f = m_manager->current();
    if (!f) {
        MM_LOG_WARN("no current file to save");
        return;
    }
    FILE * handle = fopen(f->get_name(), "rb+");
    if (!handle) {
        MM_LOG_ERROR("open file %s failed", f->get_name());
        return;
    }
    _fseeki64(handle, m_buffer_offset + m_modified_range.begin, SEEK_SET);

    MM_LOG_INFO("save offset from %llu to %llu", m_buffer_offset + m_modified_range.begin,
                m_buffer_offset + m_modified_range.end);
    auto len = fwrite(m_buffer + m_modified_range.begin, 1,
           m_modified_range.end - m_modified_range.begin, handle);
    if (len < m_modified_range.end - m_modified_range.begin) {
        MM_LOG_ERROR("write file failed");
    }
    fclose(handle);
    m_modified = false;
    m_modified_range.begin = m_modified_range.end = -1;
    snprintf(m_title, sizeof(m_title), "Hex View - [%s]###", f->get_short_name());
}
