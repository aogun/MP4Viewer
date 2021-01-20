//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_MP4_MANAGER_H
#define MP4VIEWER_MP4_MANAGER_H

#include "mp4_file.h"
#include <map>

class mp4_manager {
public:
    mp4_manager();

    std::shared_ptr<mp4_file> open(const char * path);
    bool close(const char * path);
    bool close_current();
    void switch_next();
    std::shared_ptr<mp4_file> current() { return m_current; }
    bool change(const char * path);

    std::map<std::string, std::shared_ptr<mp4_file>> * files() { return &m_files; }

    bool m_open_atom_window = true;
    bool m_open_field_window = true;
    bool m_open_mem_window = true;
private:
    void change_current(std::shared_ptr<mp4_file> next);

    std::map<std::string, std::shared_ptr<mp4_file>> m_files;
    std::shared_ptr<mp4_file> m_current;

};


#endif //MP4VIEWER_MP4_MANAGER_H
