//
// Created by aogun on 2021/1/19.
//

#include "mem_window.h"

mem_window::mem_window(mp4_manager *manager) {
    m_manager = manager;
    m_editor.HighlightColor = IM_COL32(0, 0, 255, 50);
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

    if (!m_error.empty()) {
        ImGui::SetNextWindowPos(ImVec2(800, m_top), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Hex View", &m_manager->m_open_mem_window, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::TextColored((ImVec4)(ImColor(255, 0, 0)), "%s", m_error.c_str());
        }
        ImGui::End();
    } else {
        if (m_highlight_size > 0) {
            ImGui::SetNextWindowPos(ImVec2(800, m_top), ImGuiCond_FirstUseEver);
            m_editor.DrawWindow("Hex View", m_buffer, m_size, m_buffer_offset, &m_manager->m_open_mem_window);
            if (m_editor.HighlightMin != m_highlight_offset - m_buffer_offset) {
                m_editor.GotoAddrAndHighlight(m_highlight_offset - m_buffer_offset,
                                              m_highlight_offset - m_buffer_offset + m_highlight_size);
                MM_LOG_INFO("change hightlight range to %llu - %llu", m_highlight_offset - m_buffer_offset,
                            m_highlight_offset - m_buffer_offset + m_highlight_size);
            }
        }
    }

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
        }
    } while (false);


    if (reload) {
        if (!m_buffer) {
            m_buffer = new char[MAX_BUFFER_ALLOC_SIZE];
        }
        m_size = 0;
        m_error.clear();

        auto handle = m_current_file->get_handle();
        if (handle) {
            _fseeki64(handle, 0, SEEK_END);
            auto left = _ftelli64(handle);
            m_buffer_offset = offset > 128 ? offset - 128 : 0;
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
