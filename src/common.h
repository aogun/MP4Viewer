//
// Created by aogun on 2021/2/18.
//

#ifndef MP4VIEWER_COMMON_H
#define MP4VIEWER_COMMON_H

#include <cstdint>
#define GST_MAKE_FOURCC(a,b,c,d) \
  ( (uint32_t)(d) | ((uint32_t) (c)) << 8  | ((uint32_t) (b)) << 16 | ((uint32_t) (a)) << 24 )

void display_about();

typedef enum {
    MP4_CODEC_UNKNOWN,
    MP4_CODEC_H264,
    MP4_CODEC_HEVC,
} mp4_codec_type_t;

#define ATOM_TYPE_TRAK      GST_MAKE_FOURCC('t', 'r', 'a', 'k')
#define ATOM_TYPE_STSZ      GST_MAKE_FOURCC('s', 't', 's', 'z')
#define ATOM_TYPE_STSC      GST_MAKE_FOURCC('s', 't', 's', 'c')
#define ATOM_TYPE_STCO      GST_MAKE_FOURCC('s', 't', 'c', 'o')
#define ATOM_TYPE_STSS      GST_MAKE_FOURCC('s', 't', 's', 's')
#define ATOM_TYPE_CO64      GST_MAKE_FOURCC('c', 'o', '6', '4')
#define ATOM_TYPE_AVCC      GST_MAKE_FOURCC('a', 'v', 'c', 'C')
#define ATOM_TYPE_HVCC      GST_MAKE_FOURCC('h', 'v', 'c', 'C')

#endif //MP4VIEWER_COMMON_H
