//
// Created by aogun on 2021/1/4.
//

#ifndef MP4VIEWER_ATOM_WINDOW_H
#define MP4VIEWER_ATOM_WINDOW_H

#include "atom_obj.h"
#include "my_inspect.h"
#include "mp4_manager.h"

#define WINDOW_ATOM_DEFAULT_SIZE   400

class atom_window {
public:
    explicit atom_window(mp4_manager * manager);

    void set_top(uint32_t top);
    virtual void draw();
    void show_atom(std::shared_ptr<atom_obj> atom, uint32_t flags);

private:
    mp4_manager * m_manager = nullptr;
    uint32_t m_top = 0;

    int64_t m_selected_atom = -1;
};


#endif //MP4VIEWER_ATOM_WINDOW_H
