//
// Created by aogun on 2021/3/5.
//

#ifndef MP4VIEWER_RES_H
#define MP4VIEWER_RES_H

#include <cstdint>
#include <memory>

typedef enum {
    ID_RES_IMAGE_ZOOM_IN = 1
} raw_image_id_t;

class raw_image {
public:
    raw_image(uint8_t *data, int width, int height);
    ~raw_image();

    int width() { return m_width; }
    int height() { return m_height; }
    uint8_t *data() { return m_data; }

private:
    uint8_t *m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
};

std::shared_ptr<raw_image> get_raw_image(raw_image_id_t id);
void * get_inner_texture(raw_image_id_t id);
void free_texture(void * p);

#endif //MP4VIEWER_RES_H
