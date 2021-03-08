//
// Created by aogun on 2021/1/4.
//

#include "atom_window.h"
#include "imgui.h"

atom_window::atom_window(mp4_manager * manager) {
    m_manager = manager;
}

void atom_window::draw() {
    if (!m_manager) {
        MM_LOG_ERROR("no manager");
        return;
    }
    if (!m_manager->m_open_atom_window) {
        return;
    }
    auto file = m_manager->current();
    do {
        if (!file) {
            break;
        }
        auto inspect = file->get_inspect();
        if (!inspect) {
            break;
        }
        if (!inspect->atoms()->empty()) {
//            if (ImGui::Button("collapse all")) {
//                MM_LOG_INFO("collapse all");
//                ImGui::SetNextTreeNodeOpen(false);
//            }
//            ImGui::SameLine();
//            if (ImGui::Button("expand all")) {
//                MM_LOG_INFO("expand all");
//                ImGui::SetNextTreeNodeOpen(true);
//            }
        }
        auto &io = ImGui::GetIO();
        auto size = io.DisplaySize;
        auto pos = ImGui::GetMainViewport()->WorkPos;
        pos.y -= m_top;
        ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.);
        ImGui::SetNextWindowSize(ImVec2(400, size.y - m_top), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, size.y - m_top), ImVec2(size.x, size.y - m_top));
        ImGui::Begin("Box view", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar(2);
        uint32_t flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (ImGui::TreeNodeEx(file->get_name(), flags)) {
            for (const auto& atom : *inspect->atoms()) {
                show_atom(atom, flags);
            }
            ImGui::TreePop();
        }
        ImGui::End();
    } while (false);
}

void atom_window::set_top(uint32_t top) {
    m_top = top;
}

void atom_window::show_atom(const std::shared_ptr<atom_obj>& atom, uint32_t flags) {
    uint32_t old_flags = flags;
    if (!atom->has_atoms()) flags |= ImGuiTreeNodeFlags_Leaf;
    auto offset = m_manager->current()->offset();
    if (offset >= 0 && offset == atom->get_offset())
        flags |= ImGuiTreeNodeFlags_Selected;
    if (ImGui::TreeNodeEx(atom->get_digest(), flags)) {
        if (ImGui::IsItemClicked()) {
            MM_LOG_INFO("set atom %s as selected node, offset:%llu", atom->get_name(), atom->get_offset());
            m_selected_atom = atom->get_offset();
            m_manager->current()->select_atom(atom);
        }
        if (atom->has_atoms()) {
            for (const auto & child : *atom->atoms())
                show_atom(child, old_flags);
        }
        ImGui::TreePop();
    }
}
