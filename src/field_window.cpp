//
// Created by aogun on 2021/1/18.
//

#include "field_window.h"

field_window::field_window(mp4_manager *manager) {
    m_manager = manager;

}

void field_window::set_top(uint32_t top) {
    m_top = top;

}
#define INPUT_TEXT_SPACE 8
#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))
float CalcTextWidth(const char* text, const char* text_end)
{
    ImFont* font = ImGui::GetFont();
    ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1, text, text_end, NULL);

    return text_size.x;
}

const char * warp_text(const char * text, float wrap_width, ImVec2 & size, int &length) {
    const char * s = text;
    static float text_height = ImGui::CalcTextSize("").y;
    float len = 0;
    float word_width = 0;
    size.y = text_height;
    size.x = 0;
    int max_len = 1024;
    static char * sz = new char[max_len];
    char * p = sz;
    while (*s) {
        word_width = CalcTextWidth(s, s + 1);
        if (len + word_width > wrap_width ) {
            size.y += text_height;
            len = 0;
            *p++ = '\n';
            size.x = wrap_width;
        }
        len += word_width;
        *p++ = *s++;
        if (p - sz >= max_len - 1) break;
    }
    len = IM_FLOOR(len + 0.95f);
    if (size.x == 0) size.x = len;
    *p++ = 0;
    length = p - sz;
    return sz;
}

void field_window::draw() {
    if (!m_manager) {
        MM_LOG_ERROR("no manager");
        return;
    }
    if (!m_manager->m_open_field_window) {
        return;
    }
    static float text_height = ImGui::CalcTextSize("").y;
    auto &io = ImGui::GetIO();
    auto file = m_manager->current();
    do {
        if (!file) {
            break;
        }
        auto inspect = file->get_inspect();
        if (!inspect) {
            break;
        }
        auto atom = file->current();
        if (atom) {
            auto size = io.DisplaySize;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.);
            ImGui::SetNextWindowPos(ImVec2(400, m_top), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(400, size.y - m_top), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSizeConstraints(ImVec2(40, size.y - m_top), ImVec2(size.x, size.y - m_top));
            ImGui::Begin("Detail Window", &m_manager->m_open_field_window, ImGuiWindowFlags_NoCollapse |
                                                                   ImGuiWindowFlags_HorizontalScrollbar);

            float x_padding = ImGui::GetStyle().WindowPadding.x;
            float width = ImGui::GetWindowWidth() - x_padding * 2;
            ImGui::PopStyleVar(2);
            ImVec2 text_size;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.f, 0.f, 0.f, 0.f));

            text_size.x = width;
            text_size.y = text_height;
            ImGui::InputTextMultiline("##field-title", (char*)atom->get_name(), strlen(atom->get_name()),
                                      text_size, ImGuiInputTextFlags_ReadOnly);
            int columns_count = 2;

            float name_width = 0;
            float value_width = 0;
            int index = 0;
            char label[16];
            if (atom->fields()->size() <= 100) {
                ImGui::Columns(columns_count, NULL, true);
                for (const auto &field : *atom->fields()) {
                    auto name = field->name();
                    auto x = ImGui::CalcTextSize(name.c_str()).x;;
                    if (x > name_width) name_width = x;
                }
                if (name_width > width / 2) name_width = width / 2;
                value_width = width - name_width - INPUT_TEXT_SPACE * 2 - 10;

                for (const auto &field : *atom->fields()) {
                    ImGui::Separator();
                    sprintf(label, "##%d-name", index);
                    int len = 0;
                    const char * text = warp_text(field->get_name(), name_width,
                                                  text_size, len);
                    text_size.x = name_width + INPUT_TEXT_SPACE + 10;

                    ImGui::InputTextMultiline(label, (char*)text, len,
                                              text_size, ImGuiInputTextFlags_ReadOnly);
//                MM_LOG_INFO("name [%s] size:%0.2fx%0.2f", name.c_str(), text_size.x, text_size.y);INPUT_TEXT_SPACE
                    ImGui::NextColumn();
                    sprintf(label, "##%d-value", index);
                    text = warp_text(field->get_value(), value_width, text_size, len);
                    text_size.x = value_width + INPUT_TEXT_SPACE;
                    ImGui::InputTextMultiline(label, (char*)text, len,
                                              text_size, ImGuiInputTextFlags_ReadOnly);
//                MM_LOG_INFO("value [%s] size:%0.2fx%0.2f", value.c_str(), text_size.x, text_size.y);
                    ImGui::NextColumn();
                    index ++;
                }
            } else {
                name_width = 120;
                value_width = width - 120;
                text_size.y = text_height;
                uint32_t begin = 0;
                uint32_t end = atom->fields()->size();
                if (atom->reach_page_limit()) {
                    auto &pages = *atom->get_page_names();

                    if (ImGui::BeginCombo("choose page", atom->get_current_page_name(), 0))
                    {
                        for (int i = 0; i < pages.size(); i++)
                        {
                            const bool is_selected = (i == atom->get_page_index());
                            if (ImGui::Selectable(pages[i].c_str(), is_selected))
                                atom->set_page_index(i);

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    atom->get_current_field_index(begin, end);
                }
                if (end != 0) {
                    ImGui::Columns(columns_count, NULL, true);
                    const auto &fields = *atom->fields();
                    ImGuiListClipper clipper;
                    clipper.Begin(end - begin, text_height);
                    clipper.Step();

                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                        const auto &field = fields[i + begin];
                        ImGui::Separator();
                        sprintf(label, "##%d-name", i);
                        text_size.x = name_width;
                        auto name = field->name();
                        ImGui::Text("%s", name.c_str());
                        ImGui::NextColumn();
                        auto value = field->value();
                        sprintf(label, "##%d-value", i);
                        text_size.x = value_width;
                        ImGui::InputTextMultiline(label, (char*)value.c_str(), value.size(),
                                                  text_size, ImGuiInputTextFlags_ReadOnly);
                        ImGui::NextColumn();
                    }
                    clipper.End();
                }
            }

            ImGui::SetColumnWidth(0, name_width + INPUT_TEXT_SPACE + 10);
            ImGui::SetColumnWidth(1, value_width + INPUT_TEXT_SPACE);
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::PopStyleVar(1);
            ImGui::PopStyleColor(1);
            ImGui::End();
        }
    } while (false);

}
