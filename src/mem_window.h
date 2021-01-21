//
// Created by aogun on 2021/1/19.
//

#ifndef MP4VIEWER_MEM_WINDOW_H
#define MP4VIEWER_MEM_WINDOW_H

#include "atom_obj.h"
#include "my_inspect.h"
#include "mp4_manager.h"
#include "imgui.h"
#include "imgui_memory_editor.h"

#define MAX_BUFFER_ALLOC_SIZE   32 * 1024 * 1024

typedef struct {
    int32_t begin;
    int32_t end;
} mem_range;

class mem_window {
public:
    explicit mem_window(mp4_manager * manager);

    virtual ~mem_window();

    void set_top(uint32_t top);
    virtual void draw();
    void write_cb(ImU8* mem_data, size_t addr, ImU8 data_input_value);

    void save();

    volatile bool m_modified = false;
    void set_font(ImFont * font) { m_font = font; }
private:
    void check_data();

    mp4_manager * m_manager = nullptr;
    uint32_t m_top = 0;

    MemoryEditor m_editor;
    std::shared_ptr<mp4_file> m_current_file;
    uint64_t m_buffer_offset = 0;

    char * m_buffer = nullptr;
    int m_size = 0;

    uint64_t m_highlight_offset = 0;
    uint32_t m_highlight_size = 0;
    std::string m_error;
    char m_title[128];

    mem_range m_modified_range;
    ImFont * m_font = nullptr;
};


#endif //MP4VIEWER_MEM_WINDOW_H
