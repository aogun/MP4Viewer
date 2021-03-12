//
// Created by aogun on 2021/2/18.
//

#ifndef MP4VIEWER_COMMON_H
#define MP4VIEWER_COMMON_H

#include <cstdint>
#include <string>
#include <stdarg.h>
#include <memory>

#define GST_MAKE_FOURCC(a,b,c,d) \
  ( (uint32_t)(d) | ((uint32_t) (c)) << 8  | ((uint32_t) (b)) << 16 | ((uint32_t) (a)) << 24 )

void display_about();

class mp4_buffer {
public:
    mp4_buffer(uint8_t *value, uint32_t size, bool copy = true);

    virtual ~mp4_buffer();

    uint8_t *value = nullptr;
    uint32_t size = 0;
};
inline std::string string_format(const std::string fmt, ...) {
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
                va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
                va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}
typedef enum {
    MP4_CODEC_UNKNOWN,
    MP4_CODEC_H264,
    MP4_CODEC_HEVC,
} mp4_codec_type_t;

typedef enum {
    MP4_RAW_UNKNOWN,
    MP4_RAW_R8G8B8A8,
    MP4_RAW_BGR,
    MP4_RAW_I420,
} mp4_raw_type_t;

class mp4_codec_info {
public:
    mp4_codec_type_t m_type = MP4_CODEC_UNKNOWN;
    int m_nalu_length_size = 0;
    std::shared_ptr<mp4_buffer> m_codec_data;
};
#define ATOM_TYPE_TRAK      GST_MAKE_FOURCC('t', 'r', 'a', 'k')
#define ATOM_TYPE_STSZ      GST_MAKE_FOURCC('s', 't', 's', 'z')
#define ATOM_TYPE_STSC      GST_MAKE_FOURCC('s', 't', 's', 'c')
#define ATOM_TYPE_STCO      GST_MAKE_FOURCC('s', 't', 'c', 'o')
#define ATOM_TYPE_STSS      GST_MAKE_FOURCC('s', 't', 's', 's')
#define ATOM_TYPE_CO64      GST_MAKE_FOURCC('c', 'o', '6', '4')
#define ATOM_TYPE_AVCC      GST_MAKE_FOURCC('a', 'v', 'c', 'C')
#define ATOM_TYPE_HVCC      GST_MAKE_FOURCC('h', 'v', 'c', 'C')

#endif //MP4VIEWER_COMMON_H
