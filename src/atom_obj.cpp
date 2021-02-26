//
// Created by aogun on 2021/1/4.
//

#include "atom_obj.h"
#include "mm_log.h"
#include <cmath>
#include <sstream>

static void split(const std::string& s, char sep, std::vector<std::string> &v){
    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(sep, pos)) != std::string::npos) {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        v.push_back(substring);

        prev_pos = ++pos;
    }

    v.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
}

atom_obj::atom_obj(const char *name, uint32_t head_size, uint64_t size, uint64_t offset, uint8_t version,
                   uint32_t flags) {
    if (name) m_name = name;
    m_size = size;
    m_offset = offset;
}

void atom_obj::add_atom(const std::shared_ptr<atom_obj>& obj) {
    m_children.push_back(obj);
}

void atom_obj::add_field(const std::shared_ptr<atom_field>& field) {
    m_fields.push_back(field);
}

const char *atom_obj::get_digest() {
    if (m_digest.empty()) {
        char sz[128];
        snprintf(sz, 128, "%s [size:%llu, offset:0x%llx]", m_name.c_str(), m_size, m_offset);
        m_digest = sz;
    }
    return m_digest.c_str();
}

std::vector<std::string> *atom_obj::get_page_names() {
    if (!m_pages.empty()){
        return &m_pages;
    }
    if (m_field_array) {
        int page_num = ceil(m_field_array->rows() * 1.0 / MAX_FIELD_NUM_IN_PAGE);
        m_pages.resize(page_num);
        char sz[64];
        for (int i = 0; i < page_num - 1; i ++) {
            snprintf(sz, 64, "%d-%d", i * MAX_FIELD_NUM_IN_PAGE, (i + 1) * MAX_FIELD_NUM_IN_PAGE - 1);
            m_pages[i] = sz;
        }
        snprintf(sz, 64, "%d-%d", (page_num - 1) * MAX_FIELD_NUM_IN_PAGE, m_field_array->rows() - 1);
        m_pages[page_num - 1] = sz;
    } else {
        int page_num = ceil(m_fields.size() * 1.0 / MAX_FIELD_NUM_IN_PAGE);
        m_pages.resize(page_num);
        char sz[64];
        for (int i = 0; i < page_num - 1; i ++) {
            snprintf(sz, 64, "%d-%d", i * MAX_FIELD_NUM_IN_PAGE, (i + 1) * MAX_FIELD_NUM_IN_PAGE - 1);
            m_pages[i] = sz;
        }
        snprintf(sz, 64, "%d-%llu", (page_num - 1) * MAX_FIELD_NUM_IN_PAGE, m_fields.size() - 1);
        m_pages[page_num - 1] = sz;

    }
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
        if (m_field_array) {
            end = m_field_array->rows() - 1;
        } else {
            end = m_fields.size() - 1;
        }
    }
}

void atom_obj::get_current_field_index(uint32_t &begin, uint32_t &end) {
    get_field_index(m_page_index, begin, end);
}

atom_obj::~atom_obj() {
//    MM_LOG_INFO("destruct atom %s", m_name.c_str());
}

void atom_obj::add_field_array(const char * name, int row_num, int column_num,
                               const char *column_name, int64_t *data) {
    m_field_array = std::make_shared<atom_fields>(name, row_num, column_num, column_name, data);
}

bool atom_obj::reach_page_limit() {
    return m_fields.size() > MAX_FIELD_NUM_IN_PAGE ||
    (m_field_array && m_field_array->rows() > MAX_FIELD_NUM_IN_PAGE);
}

const char *atom_obj::get_field_name(uint32_t index) {
    if (m_field_array) {
        return m_field_array->get_name(index);
    } else {
        return m_fields[index]->get_name();
    }
}

const char *atom_obj::get_field_value(uint32_t index) {
    if (m_field_array) {
        return m_field_array->value(index).c_str();
    } else {
        return m_fields[index]->get_name();
    }
}

const std::string &atom_obj::get_field_value_str(uint32_t index) {
    if (m_field_array) {
        return m_field_array->value(index);
    } else {
        return m_fields[index]->value();
    }
}

atom_field::atom_field(const char *name, const char *value) {
    if (name) m_name = name;
    m_value = value;
}

atom_field::atom_field(const char *name, float value) {
    if (name) m_name = name;
    char sz[64];
    snprintf(sz, 64, "%0.8f", value);
    m_value = sz;
}

atom_field::atom_field(const char *name, uint64_t value) {
    if (name) m_name = name;
    char sz[64];
    snprintf(sz, 64, "%llu", value);
    m_value = sz;

}

atom_field::atom_field(const char *name, const uint8_t *value, uint32_t size) {
    if (name) m_name = name;

    char sz[128];
    char *p = sz;
    int left = 126;
    auto len = snprintf(sz, left, "[");
    left -= len;
    p += len;
    for (int i = 0; i < size; i ++) {
        len = snprintf(p, left, i == 0 ? "%02x" : " %02x", value[i]);
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

atom_fields::atom_fields(const char * name, int row_num, int column_num, const char * names, int64_t *data) {
    if (names)
        split(names, ',', m_column_name);
    m_column_num = column_num;
    m_row_num = row_num;
    m_data = data;
    m_name = name;
}

atom_fields::~atom_fields() {
    if (m_data) {
        delete [] m_data;
        m_data = nullptr;
    }
}

const char *atom_fields::get_column_name(int column) {
    if (column >= m_column_num) {
        MM_LOG_ERROR("invalid column index(%d), total column number:%d", column, m_column_num);
        return nullptr;
    }
    return m_column_name[column].c_str();
}

const char *atom_fields::get_name(uint32_t row) {
    static char sz[64];
    auto it = m_name_map.find(row);
    if (it == m_name_map.end()) {
        snprintf(sz, 64, "%s %d", m_name.c_str(), row);
        m_name_map[row] = sz;
        return sz;
    }
    return it->second.c_str();
}

int64_t atom_fields::get_value(int column, int row) {
    if (column >= m_column_num) {
        MM_LOG_ERROR("invalid column index(%d), total column number:%d", column, m_column_num);
        return 0;
    }
    if (row >= m_row_num) {
        MM_LOG_ERROR("invalid row index(%d), total row number:%d", row, m_row_num);
        return 0;
    }
    return m_data[row * m_column_num + column];
}

int64_t *atom_fields::get_values(int row) {
    if (row >= m_row_num) {
        MM_LOG_ERROR("invalid row index(%d), total row number:%d", row, m_row_num);
        return nullptr;
    }

    return &m_data[row * m_column_num];
}

const std::string &atom_fields::value(uint32_t row) {
    auto it = m_value_map.find(row);
    if (it == m_value_map.end()) {
        static std::string sz;
        std::stringstream str;
        auto p = m_data + row * m_column_num;
        if (m_column_num == 1) {
            str << p[0];
        } else {
            for (int i = 0; i < m_column_num; i ++) {
                if (i != 0) str << ",";
                if (m_column_name.size() > i) str << m_column_name[i] << "=" << p[i];
            }
        }
        sz = str.str();
        m_value_map[row] = sz;
        return sz;
    }
    return it->second;
}
