//
// Created by aogun on 2021/1/18.
//

#include "mp4_manager.h"

#include <utility>

mp4_manager::mp4_manager() {}

std::shared_ptr<mp4_file> mp4_manager::open(const char * path) {
    std::shared_ptr<mp4_file> result;
    if (!path) return nullptr;
    if (m_files.size() >= 4) {
        MM_LOG_ERROR("too many files");
        return nullptr;
    }
    auto iter = m_files.find(path);
    if (iter != m_files.end()) {
        MM_LOG_INFO("file %s already opened", path);
        m_current = iter->second;
        return iter->second;
    }
    result = std::make_shared<mp4_file>();
    if (result->open(path)) {
        m_files[path] = result;
        m_current = result;
        return result;
    }
    return nullptr;
}

bool mp4_manager::change(const char *path) {
    auto iter = m_files.find(path);
    if (iter != m_files.end()) {
        change_current(iter->second);
        return true;
    }
    return false;
}

bool mp4_manager::close(const char *path) {
    auto iter = m_files.find(path);
    bool changed = false;
    if (iter != m_files.end()) {
        if (m_current == iter->second) {
            changed = true;
        }
        m_files.erase(iter);
        if (changed) {
            if (m_files.empty()) {
                change_current(nullptr);
            } else {
                change_current(m_files.begin()->second);
            }
        }
        return true;
    }
    return false;
}

bool mp4_manager::close_current() {
    if (!m_current) {
        return true;
    }
    MM_LOG_INFO("close file %s", m_current->get_name());
    return close(m_current->get_name());
}

void mp4_manager::switch_next() {
    if (m_files.empty()) return;
    bool found = false;
    for (const auto &iter : m_files) {
        if (found) {
            change_current(iter.second);
            return;
        }
        if (iter.second == m_current) {
            found = true;
        }
    }
    if (!found) {
        MM_LOG_ERROR("can't find current file(%s) position", m_current->get_short_name());
    }
    change_current(m_files.begin()->second);
}

void mp4_manager::change_current(std::shared_ptr<mp4_file> next) {
    m_current = std::move(next);
}
