//
// Created by aogun on 2021/1/18.
//

#ifndef MP4VIEWER_MP4_FILE_H
#define MP4VIEWER_MP4_FILE_H

#include <string>
#include "Ap4.h"
#include "my_inspect.h"
#include <memory>
#include "mm_log.h"
#include "mp4_decoder.h"

class mp4_file {
public:
    explicit mp4_file();

    virtual ~mp4_file();

    bool open(const char * path);
    const char * get_name();

    std::shared_ptr<my_inspect> get_inspect() { return m_inspect; }
    std::shared_ptr<atom_obj> current() { return m_selected; }
    void select_atom(const std::shared_ptr<atom_obj>& obj);
    void select_data(int64_t offset, uint32_t size);

    const char * get_short_name() { return m_short_name.c_str(); }

    FILE * get_handle();
    const char * get_error() { return m_error.c_str(); }
    int64_t offset() { return m_offset; }

    void get_current(int64_t &offset, uint32_t &size) const { offset = m_offset; size = m_size; }

    std::shared_ptr<mp4_video_dec_task> show(std::shared_ptr<atom_obj> atom, uint32_t index,
                                             int64_t offset, uint32_t size);
private:
    std::string m_name;
    std::string m_short_name;
    std::shared_ptr<my_inspect> m_inspect;
    std::shared_ptr<atom_obj> m_selected;

    FILE * m_handle = nullptr;
    std::string m_error;

    int64_t m_offset = -1;
    uint32_t m_size = 0;

    std::shared_ptr<mp4_decoder> m_decoder;
};


#endif //MP4VIEWER_MP4_FILE_H
