//
// Created by aogun on 2021/1/4.
//

#include "atom_obj.h"
#include "mm_log.h"

atom_obj::atom_obj(const char *name, uint32_t head_size, uint64_t size, uint64_t offset, uint8_t version,
                   uint32_t flags) {
    if (name) m_name = name;
    m_size = size;
    m_offset = offset;
}

void atom_obj::add_atom(std::shared_ptr<atom_obj> obj) {
    m_children.push_back(obj);
}

void atom_obj::add_fields(std::shared_ptr<atom_fields> field) {
    m_fields.push_back(field);
}

const char *atom_obj::get_digest() {
    if (m_digest.empty()) {
        char sz[128];
        snprintf(sz, 128, "%s [size:%d, offset:0x%x]", m_name.c_str(), m_size, m_offset);
        m_digest = sz;
    }
    return m_digest.c_str();
}

std::vector<std::string> *atom_obj::get_page_names() {
    if (!m_pages.empty() || m_fields.empty()){
        return &m_pages;
    }
    int page_num = ceil(m_fields.size() * 1.0 / MAX_FIELD_NUM_IN_PAGE);
    m_pages.resize(page_num);
    char sz[64];
    for (int i = 0; i < page_num - 1; i ++) {
        snprintf(sz, 64, "%d-%d", i * MAX_FIELD_NUM_IN_PAGE, (i + 1) * MAX_FIELD_NUM_IN_PAGE - 1);
        m_pages[i] = sz;
    }
    snprintf(sz, 64, "%d-%d", (page_num - 1) * MAX_FIELD_NUM_IN_PAGE, m_fields.size() - 1);
    m_pages[page_num - 1] = sz;

    return &m_pages;
}

const char *atom_obj::get_current_page_name() {
    if (m_page_index >= m_pages.size()) {
        MM_LOG_ERROR("page index too large");
        return nullptr;
    }
    return m_pages[m_page_index].c_str();
}

void atom_obj::set_page_index(uint32_t index) {
    if (index >= m_pages.size()) {
        MM_LOG_ERROR("page index %u reach the maximum size %u", index, m_pages.size());
        return;
    }
    m_page_index = index;
}

void atom_obj::get_field_index(uint32_t page_index, uint32_t &begin, uint32_t &end) {

    if (page_index >= m_pages.size()) {
        MM_LOG_ERROR("page index %u reach the maximum size %u", page_index, m_pages.size());
        return;
    }
    begin = page_index * MAX_FIELD_NUM_IN_PAGE;
    if (page_index < m_pages.size() - 1) {
        end = (page_index + 1) * MAX_FIELD_NUM_IN_PAGE - 1;
    } else {
        end = m_fields.size() - 1;
    }
}

void atom_obj::get_current_field_index(uint32_t &begin, uint32_t &end) {
    get_field_index(m_page_index, begin, end);
}

atom_obj::~atom_obj() {
//    MM_LOG_INFO("destruct atom %s", m_name.c_str());
}

atom_fields::atom_fields(const char *name, const char *value) {
    if (name) m_name = name;
    m_value = value;
}

atom_fields::atom_fields(const char *name, float value) {
    if (name) m_name = name;
    char sz[64];
    snprintf(sz, 64, "%0.8f", value);
    m_value = sz;
}

atom_fields::atom_fields(const char *name, uint64_t value) {
    if (name) m_name = name;
    char sz[64];
    snprintf(sz, 64, "%llu", value);
    m_value = sz;

}

atom_fields::atom_fields(const char *name, const uint8_t *value, uint32_t size) {
    if (name) m_name = name;

    char sz[128];
    char *p = sz;
    int left = 126;
    auto len = snprintf(sz, left, "[");
    left -= len;
    p += len;
    for (int i = 0; i < size; i ++) {
        len = snprintf(p, left, " %02x", value[i]);
        left -= len;
        p += len;
        if (left == 0) {
            break;
        }
    }
    *p++ = ']';
    *p = 0;
    m_value = sz;

}
