//
// Created by aogun on 2021/1/4.
//

#ifndef MP4VIEWER_ATOM_OBJ_H
#define MP4VIEWER_ATOM_OBJ_H

#include <vector>
#include <string>
#include <memory>

#define MAX_FIELD_NUM_IN_PAGE       3000

//typedef enum {
//    FIELD_TYPE_STRING,
//    FIELD_TYPE_INT,
//    FIELD_TYPE_FLOAT,
//    FIELD_TYPE_DATA,
//} atom_field_value_type;

class atom_fields {
public:
    atom_fields(const char * name, const char * value);
    atom_fields(const char * name, float value);
    atom_fields(const char * name, uint64_t value);
    atom_fields(const char * name, const uint8_t *value, uint32_t size);

    const char * get_name() { return m_name.c_str(); }
    const char * get_value() { return m_value.c_str(); }

    const std::string &name() { return m_name; }
    const std::string &value() { return m_value; }
private:
    std::string m_name;
    std::string m_value;
};

class atom_obj {
public:
    atom_obj(const char * name, uint32_t head_size, uint64_t size, uint64_t offset, uint8_t version, uint32_t flags);

    virtual ~atom_obj();

    void add_atom(std::shared_ptr<atom_obj> obj);
    void add_fields(std::shared_ptr<atom_fields> field);

    std::weak_ptr<atom_obj> parent() { return m_parent; }
    void set_parent(std::shared_ptr<atom_obj> parent) { m_parent = parent; }
    const char * get_name() { return m_name.c_str(); }
    const char * get_digest();

    bool has_fields() { return !m_fields.empty(); }
    bool has_atoms() { return !m_children.empty(); }
    uint64_t get_offset() const { return m_offset; }
    uint64_t get_size() const { return m_size; }

    std::vector<std::shared_ptr<atom_obj>> * atoms() { return &m_children; }
    std::vector<std::shared_ptr<atom_fields>> * fields() { return &m_fields; }

    bool reach_page_limit() { return m_fields.size() > MAX_FIELD_NUM_IN_PAGE; }
    std::vector<std::string> * get_page_names();
    const char * get_current_page_name();
    void set_page_index(uint32_t index);
    uint32_t get_page_index() { return m_page_index; }
    void get_field_index(uint32_t page_index, uint32_t &begin, uint32_t &end);
    void get_current_field_index(uint32_t &begin, uint32_t &end);

private:
    std::string m_name;
    std::string m_digest;
    std::vector<std::shared_ptr<atom_fields>> m_fields;
    std::vector<std::shared_ptr<atom_obj>> m_children;
    std::weak_ptr<atom_obj> m_parent;
    uint64_t m_size = 0;
    uint64_t m_offset = 0;

    std::vector<std::string> m_pages;
    uint32_t m_page_index = 0;

};


#endif //MP4VIEWER_ATOM_OBJ_H
