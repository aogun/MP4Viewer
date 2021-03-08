//
// Created by aogun on 2021/3/8.
//
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

using namespace ImGui;

bool ImageButtonEx(const char *name, ImTextureID user_texture_id,
                 const ImVec2& size)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    const ImVec2& uv0 = ImVec2(0, 0);
    const ImVec2& uv1 = ImVec2(1,1);
    int frame_padding = -1;
    const ImVec4& bg_col = ImVec4(0,0,0,0);
    const ImVec4& tint_col = ImVec4(1,1,1,1);

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    const ImGuiID id = window->GetID(name);

    const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : g.Style.FramePadding;
    return ImageButtonEx(id, user_texture_id, size, uv0, uv1, padding, bg_col, tint_col);
}