//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_FIELD_WINDOW_H
#define MP4VIEWER_FIELD_WINDOW_H

#include "atom_obj.h"
#include "my_inspect.h"
#include "mp4_manager.h"
#include "imgui.h"

#define WINDOW_FIELD_DEFAULT_SIZE   400

class field_window {
public:
    explicit field_window(mp4_manager * manager);

    void set_top(uint32_t top);
    virtual void draw();

private:
    mp4_manager * m_manager = nullptr;
    uint32_t m_top = 0;

    int64_t m_selected = -1;
    void * m_zoom_in = nullptr;
};


#endif //MP4VIEWER_FIELD_WINDOW_H
