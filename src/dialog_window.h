//
// Created by aogun on 2021/1/20.
//

#ifndef MP4VIEWER_DIALOG_WINDOW_H
#define MP4VIEWER_DIALOG_WINDOW_H

#include <string>
#include <thread>
#include <mutex>

class dialog_window {
public:
    dialog_window();
    void open_dialog();
    bool is_selected() { return m_selected; }
    void clear() { m_selected = false; m_path.clear(); }
    const char * get_path() { return m_path.c_str(); }

private:
    void open();
    std::string m_path;
    volatile bool m_selected = false;
    volatile bool m_opened = false;
    std::shared_ptr<std::thread> m_thread;
    std::recursive_mutex m_mutex;
};


#endif //MP4VIEWER_DIALOG_WINDOW_H
