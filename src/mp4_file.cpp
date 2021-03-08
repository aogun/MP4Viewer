//
// Created by aogun on 2021/1/18.
//

#include "mp4_file.h"
#include <filesystem>

mp4_file::mp4_file() = default;

const char *mp4_file::get_name() {
    return m_name.c_str();
}

bool mp4_file::open(const char *path) {
    AP4_ByteStream*         input       = nullptr;
    if (path == nullptr) return false;
    AP4_Result result = AP4_FileByteStream::Create(path, AP4_FileByteStream::STREAM_MODE_READ, input);
    if (AP4_FAILED(result)) {
        MM_LOG_INFO("open file %s failed", path);
        return false;
    }
    m_inspect = std::make_shared<my_inspect>();

    AP4_Atom* atom = nullptr;
    AP4_DefaultAtomFactory atom_factory;
    while (atom_factory.CreateAtomFromStream(*input, atom) == AP4_SUCCESS) {
        // remember the current stream position because the Inspect method
        // may read from the stream (there may be stream references in some
        // of the atoms
        AP4_Position position;
        input->Tell(position);

        // inspect the atom
        atom->Inspect(*m_inspect);

        // restore the previous stream position
        input->Seek(position);

        // destroy the atom
        delete atom;
        atom = nullptr;
    }
    if (input) input->Release();
    m_name = path;
    m_short_name = std::filesystem::path(path).filename().string();
    m_inspect->Finished();
    return true;
}

FILE *mp4_file::get_handle() {
    if (!m_error.empty()) {
        return nullptr;
    }
    if (m_handle) return m_handle;

    m_handle = fopen(m_name.c_str(), "rb");
    MM_LOG_INFO("open file %s", m_name.c_str());
    return m_handle;
}

mp4_file::~mp4_file() {
    if (m_handle) {
        fclose(m_handle);
        m_handle = nullptr;
    }
    MM_LOG_INFO("destruct file %s", m_name.c_str());
}

void mp4_file::select_atom(const std::shared_ptr<atom_obj>& obj) {
    m_selected = obj;
    m_offset = obj->get_offset();
    m_size = obj->get_size();
}

void mp4_file::select_data(uint64_t offset, uint32_t size) {
    MM_LOG_INFO("select data offset %llu, size %u", offset, size);
    m_offset = offset;
    m_size = size;
}
