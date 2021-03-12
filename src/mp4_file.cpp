//
// Created by aogun on 2021/1/18.
//

#include "mp4_file.h"
#include <filesystem>
#include "mp4_decoder.h"

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
    if (m_decoder) {
        m_decoder->destroy();
        m_decoder.reset();
    }
}

void mp4_file::select_atom(const std::shared_ptr<atom_obj>& obj) {
    m_selected = obj;
    m_offset = obj->get_offset();
    m_size = obj->get_size();
}

void mp4_file::select_data(int64_t offset, uint32_t size) {
    MM_LOG_INFO("select data offset %lli, size %u", offset, size);
    m_offset = offset;
    m_size = size;
}

std::shared_ptr<mp4_video_dec_task> mp4_file::show(std::shared_ptr<atom_obj> atom,
                                                   uint32_t index,
                                                   int64_t offset, uint32_t size) {
    std::shared_ptr<mp4_video_dec_task> task = std::make_shared<mp4_video_dec_task>();
    auto codec = atom->codec_info();
    if (!codec) {
        task->m_error = string_format("atom %s have no codec info", atom->get_name());
        task->m_state = DECODE_STATE_FAILED;
        MM_LOG_ERROR(task->m_error.c_str());
        return task;
    }
    if (!m_decoder) {
        m_decoder = create_mp4_decoder(m_name.c_str(), codec);
        if (!m_decoder) {
            task->m_error = string_format("create decoder failed");
            task->m_state = DECODE_STATE_FAILED;
            MM_LOG_ERROR(task->m_error.c_str());
            return task;
        }
    }
    auto v = std::make_shared<std::vector<frame_position_t>>();
    uint32_t key_index = atom->get_sync_value(index);
    if (key_index > index) {
        task->m_error = string_format("key index %u larger than sample index:%u, "
                                      "please check it",
                                      key_index, index);
        task->m_state = DECODE_STATE_FAILED;
        MM_LOG_ERROR(task->m_error.c_str());
        return task;
    }
    MM_LOG_INFO("sample index:%u, key index:%u", index, key_index);
    if (key_index == index) {
        frame_position_t pos(offset, size);
        v->push_back(pos);
    } else {
        for (auto i = key_index; i <= index; i ++) {
            frame_position_t pos(atom->get_field_offset(i),
                                 (uint32_t)(*atom->get_field_value_int(i)));

            v->push_back(pos);
            MM_LOG_INFO("push sample index:%u, offset:%lli, size:%u",
                        i, pos.offset, pos.size);
        }
    }

    task->input_frames = std::move(v);
    task->m_info = string_format("index:%u iframe:%u offset:%lli size %u",
                                 index, key_index, offset, size);
    m_decoder->decode(task);
    return task;
}
