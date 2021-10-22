# pragma once
# include <imgui/imgui.h>

#include <nativesdr_graphics_export.h>

namespace ax {
namespace Drawing {

enum class IconType: ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };

NATIVESDR_GRAPHICS_EXPORT void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);

} // namespace Drawing
} // namespace ax