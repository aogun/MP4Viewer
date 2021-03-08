//
// Created by aogun on 2021/1/4.
//

#ifndef MP4VIEWER_ATOM_OBJ_H
#define MP4VIEWER_ATOM_OBJ_H

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "common.h"

#define MAX_FIELD_NUM_IN_PAGE       3000

//typedef enum {
//    FIELD_TYPE_STRING,
//    FIELD_TYPE_INT,
//    FIELD_TYPE_FLOAT,
//    FIELD_TYPE_DATA,
//} atom_field_value_type;

class mp4_buffer {
public:
    mp4_buffer(uint8_t *value, uint32_t size, bool copy = true);

    virtual ~mp4_buffer();

    uint8_t *value = nullptr;
    uint32_t size = 0;
};
class mp4_codec_info {
public:
    mp4_codec_type_t m_type = MP4_CODEC_UNKNOWN;
    std::shared_ptr<mp4_buffer> m_codec_data;
};

class atom_field {
public:
    atom_field(const char * name, const char * value);
    atom_field(const char * name, float value);
    atom_field(const char * name, uint64_t value);
    atom_field(const char * name, const uint8_t *value, uint32_t size);

    const char * get_name() { return m_name.c_str(); }
    const char * get_value() { return m_value.c_str(); }

    const std::string &name() { return m_name; }
    const std::string &value() { return m_value; }
    std::shared_ptr<mp4_buffer> &data() { return m_data; }
private:
    std::string m_name;
    std::string m_value;
    std::shared_ptr<mp4_buffer> m_data;
};

class atom_fields {
public:
    atom_fields(const char * name, uint32_t row_num, uint32_t column_num,
                const char * names, int64_t * data);

    virtual ~atom_fields();

    const char * get_name(uint32_t row);
    const char * get_column_name(uint32_t column);
    int64_t get_value(uint32_t column, uint32_t row);
    int64_t* get_values(uint32_t row);
    const std::string & value(uint32_t row);
    uint32_t rows() { return m_row_num; }
    int64_t * data() { return m_data; }
private:
    std::vector<std::string> m_column_name;
    int m_column_num = 0;
    uint32_t m_row_num = 0;
    int64_t * m_data = nullptr;
    std::string m_name;
    std::map<int, std::string> m_name_map;
    std::map<int, std::string> m_value_map;
};

class atom_obj {
public:
    atom_obj(const char * name, uint32_t head_size, uint64_t size,
             uint64_t offset, uint8_t version, uint32_t flags);

    virtual ~atom_obj();

    void add_atom(const std::shared_ptr<atom_obj>& obj);
    void add_field(const std::shared_ptr<atom_field>& field);
    void add_field_array(const char * name, int row_num, int column_num,
                         const char *column_name, int64_t *data);

    std::weak_ptr<atom_obj> parent() { return m_parent; }
    void set_parent(const std::shared_ptr<atom_obj>& parent) { m_parent = parent; }
    const char * get_name() { return m_name.c_str(); }
    uint32_t get_type() { return m_type; }
    const char * get_digest();

    bool has_fields() { return !m_fields.empty(); }
    bool has_atoms() { return !m_children.empty(); }
    uint64_t get_offset() const { return m_offset; }
    uint64_t get_size() const { return m_size; }

    bool has_sample_offset() { return m_has_sample_offset; }
//    void get_sample_offset(uint32_t index, uint64_t offset, uint32_t sync_index);

    std::vector<std::shared_ptr<atom_obj>> * atoms() { return &m_children; }
    std::vector<std::shared_ptr<atom_field>> * fields() { return &m_fields; }
    std::shared_ptr<atom_fields> field_array() { return m_field_array; }

    void set_field_offset(int64_t * data, uint32_t num);
    uint32_t get_field_offset_num() { return m_field_offset_num; }
    int64_t * get_field_offset_array() { return m_field_offset; }
    int64_t get_field_offset(uint32_t index) { return m_field_offset[index]; }

    const char * get_field_name(uint32_t index);
    const char * get_field_value(uint32_t index);
    int64_t * get_field_value_int(uint32_t index);
    const std::string & get_field_value_str(uint32_t index);

    bool reach_page_limit();
    std::vector<std::string> * get_page_names();
    const char * get_current_page_name();
    void set_page_index(uint32_t index);
    uint32_t get_page_index() { return m_page_index; }
    void get_field_index(uint32_t page_index, uint32_t &begin, uint32_t &end);
    void get_current_field_index(uint32_t &begin, uint32_t &end);

    void set_codec_info(std::shared_ptr<mp4_codec_info> codec);
    std::shared_ptr<mp4_codec_info> codec_info() { return m_codec_info; }
private:
    std::string m_name;
    std::string m_digest;
    std::vector<std::shared_ptr<atom_field>> m_fields;
    std::shared_ptr<atom_fields> m_field_array;
    std::vector<std::shared_ptr<atom_obj>> m_children;
    std::weak_ptr<atom_obj> m_parent;
    uint64_t m_size = 0;
    uint64_t m_offset = 0;

    std::vector<std::string> m_pages;
    uint32_t m_page_index = 0;

    bool m_has_sample_offset = false;
    bool m_playable = false;

    uint32_t m_type = 0;

    int64_t *m_field_offset = nullptr;
    uint32_t m_field_offset_num = 0;
    std::shared_ptr<mp4_codec_info> m_codec_info;
};
using weak_atom = std::weak_ptr<atom_obj>;
using shared_atom = std::shared_ptr<atom_obj>;

#endif //MP4VIEWER_ATOM_OBJ_H
