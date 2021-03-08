//
// Created by aogun on 2021/1/18.
//

#include "my_inspect.h"
#include "common.h"

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
    m_Verbosity = 99;
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
        auto field = std::make_shared<atom_field>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_field(field);
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
        auto field = std::make_shared<atom_field>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_field(field);
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
        auto field = std::make_shared<atom_field>(name, value);
//        MM_LOG_INFO("add field %s to %s", name, m_current->get_name());
        m_current->add_field(field);
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
        auto field = std::make_shared<atom_field>(name, bytes, byte_count);
        m_current->add_field(field);
    } else {
        MM_LOG_WARN("no current atom");
    }
}

void my_inspect::AddFieldArray(AP4_Size column_num, AP4_Size row_num,
                               const char * column_names, AP4_UI64 *data) {
    if (m_current) {
        m_current->add_field_array("", row_num, column_num, column_names, (int64_t *)data);
    } else {
        MM_LOG_WARN("no current atom");
    }
}

void my_inspect::Finished() {
    for (auto &atom : m_atoms) {
        process_atom(atom);
    }
}

void my_inspect::process_atom(std::shared_ptr<atom_obj> &atom) {
    MM_LOG_INFO("process atom:%s, type:0x%x", atom->get_name(), atom->get_type());
    if (atom->get_type() == ATOM_TYPE_STSZ) {
        int64_t * offset_array = nullptr;
        MM_LOG_INFO("process stsz atom");
        auto parent = atom->parent();
        do {
            if (parent.owner_before(weak_atom{})) {
                MM_LOG_ERROR("stsz no parent");
                break;
            }
            auto stsd = parent.lock();
            if (!stsd) {
                MM_LOG_ERROR("get stsz parent stsd failed");
                break;
            }
            shared_atom stco_atom, stsc_atom, co64_atom;
            for (const auto & sub_atom: *stsd->atoms()) {
                switch (sub_atom->get_type()) {
                    case ATOM_TYPE_STSC:
                        stsc_atom = sub_atom;
                        break;
                    case ATOM_TYPE_STCO:
                        stco_atom = sub_atom;
                        break;
                    case ATOM_TYPE_CO64:
                        co64_atom = sub_atom;
                        break;
                    default:
                        break;
                }
            }
            if (!stco_atom || (!stsc_atom && !co64_atom)) {
                MM_LOG_ERROR("missing atom");
                break;
            }
            std::shared_ptr<atom_fields> stco;
            auto stsz = atom->field_array();
            if (!stsz || stsz->rows() == 0) {
                MM_LOG_ERROR("no data in stsz atom");
                break;
            }
            if (stco_atom)
                stco = stco_atom->field_array();
            else
                stco = co64_atom->field_array();
            if (!stco || stco->rows() == 0) {
                MM_LOG_ERROR("no data in stsc|co64 atom");
                break;
            }
            auto stsc = stsc_atom->field_array();
            if (!stsc || stsc->rows() == 0) {
                MM_LOG_ERROR("no data in stsc atom");
                break;
            }
            offset_array = new int64_t[stsz->rows()];
            uint32_t chunk_index = 0;
            uint32_t sample_index = 0;
            bool stop = false;
            for (uint32_t i = 0; i < stsc->rows(); i ++) {
                auto data = stsc->get_values(i);
                if (chunk_index + 1 != data[0]) {
                    MM_LOG_ERROR("invalid first_chunk value(%lli), expect:%lli",
                                 data[0], chunk_index);
                    break;
                }
                if (sample_index + 1 != data[1]) {
                    MM_LOG_ERROR("invalid first_sample value(%lli), expect:%lli",
                                 data[1], sample_index + 1);
                    break;
                }
                for (uint32_t j = 0; j < data[2]; j ++) { // chunk
                    if (chunk_index >= stco->rows()) {
                        MM_LOG_WARN("stco|co64 size %lli not enough, current chunk index:%u",
                                    stco->rows(), chunk_index);
                        stop = true;
                        break;
                    }
                    auto chunk_offset = *(stco->get_values(chunk_index));
                    for (uint32_t k = 0; k < data[3]; k ++) {
                        offset_array[sample_index] = chunk_offset;
                        if (sample_index >= stsz->rows()) {
                            MM_LOG_WARN("stsz size %lli not enough, current sample index:%u",
                                        stsz->rows(), sample_index);
                            stop = true;
                            break;
                        }
                        chunk_offset += *(stsz->get_values(sample_index));
                        sample_index ++;
                    }
                    if (stop) break;
                    chunk_index ++;
                }
                if (stop) break;
            }
            if (sample_index >= 1) {
                atom->set_field_offset(offset_array, sample_index);
                offset_array = nullptr;
            }
        } while (false);
        if (offset_array) {
            delete[] offset_array;
            offset_array = nullptr;
        }
    }
    if (atom->has_atoms()) {
        for (auto &sub_atom : *atom->atoms()) {
            process_atom(sub_atom);
        }
    }
}
