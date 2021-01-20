//
// Created by aogun on 2021/1/18.
//

#include "my_inspect.h"

static void
AP4_MakePrefixString(unsigned int indent, char* prefix, AP4_Size size)
{
    if (size == 0) return;
    if (indent >= size-1) indent = size-1;
    for (unsigned int i=0; i<indent; i++) {
        prefix[i] = ' ';
    }
    prefix[indent] = '\0';
}

my_inspect::my_inspect()
{
    m_Verbosity = 2;
}


/*----------------------------------------------------------------------
|   my_inspect::~my_inspect
+---------------------------------------------------------------------*/
my_inspect::~my_inspect()
{
//    MM_LOG_INFO("destruct inspect");
}

/*----------------------------------------------------------------------
|   my_inspect::StartAtom
+---------------------------------------------------------------------*/
void
my_inspect::StartAtom(const char* name,
                              AP4_UI08    version,
                              AP4_UI32    flags,
                              AP4_Size    header_size,
                              AP4_UI64    size,
                              AP4_UI64    offset)
{
    auto atom = std::make_shared<atom_obj>(name, header_size, size, offset, version, flags);
    if (m_current) {
        m_current->add_atom(atom);
        atom->set_parent(m_current);
//        MM_LOG_INFO("add %s to %s offset:%llu, size:%llu", name, m_current->get_name(),
//                    offset, size);
        m_current = atom;
    } else {
        m_atoms.push_back(atom);
        m_current = atom;
//        MM_LOG_INFO("add %s to root", name);
    }
}

/*----------------------------------------------------------------------
|   my_inspect::EndAtom
+---------------------------------------------------------------------*/
void
my_inspect::EndAtom()
{
    if (m_current) {
//        MM_LOG_INFO("pop %s", m_current->get_name());
        m_current = m_current->parent().lock();
    } else {
//        MM_LOG_WARN("no current atom");
    }
}

/*----------------------------------------------------------------------
|   my_inspect::StartDescriptor
+---------------------------------------------------------------------*/
void
my_inspect::StartDescriptor(const char* name,
                                    AP4_Size    header_size,
                                    AP4_UI64    size, AP4_UI64 offset)
{
    auto atom = std::make_shared<atom_obj>(name, header_size, size, offset, 0, 0);
    if (m_current) {
        m_current->add_atom(atom);
        atom->set_parent(m_current);
        m_current = atom;
    } else {
        m_atoms.push_back(atom);
        m_current = atom;
    }
}

/*----------------------------------------------------------------------
|   my_inspect::EndDescriptor
+---------------------------------------------------------------------*/
void
my_inspect::EndDescriptor()
{
    if (m_current) {
        m_current = m_current->parent().lock();
    } else {
        MM_LOG_WARN("no current atom");
    }
}

/*----------------------------------------------------------------------
|   my_inspect::AddField
+---------------------------------------------------------------------*/
void
my_inspect::AddField(const char* name, const char* value, FormatHint)
{
    if (m_current) {
        auto field = std::make_shared<atom_fields>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_fields(field);
    } else {
        MM_LOG_WARN("no current atom");
    }
}

/*----------------------------------------------------------------------
|   my_inspect::AddField
+---------------------------------------------------------------------*/
void
my_inspect::AddField(const char* name, AP4_UI64 value, FormatHint hint)
{
    if (m_current) {
        auto field = std::make_shared<atom_fields>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_fields(field);
    } else {
        MM_LOG_WARN("no current atom");
    }
}

/*----------------------------------------------------------------------
|   my_inspect::AddFieldF
+---------------------------------------------------------------------*/
void
my_inspect::AddFieldF(const char* name, float value, FormatHint /*hint*/)
{
    if (m_current) {
        auto field = std::make_shared<atom_fields>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_fields(field);
    } else {
        MM_LOG_WARN("no current atom");
    }
}

/*----------------------------------------------------------------------
|   my_inspect::AddField
+---------------------------------------------------------------------*/
void
my_inspect::AddField(const char*          name,
                             const unsigned char* bytes,
                             AP4_Size             byte_count,
                             FormatHint           /* hint */)
{
    if (m_current) {
        auto field = std::make_shared<atom_fields>(name, bytes, byte_count);
        m_current->add_fields(field);
    } else {
        MM_LOG_WARN("no current atom");
    }
}