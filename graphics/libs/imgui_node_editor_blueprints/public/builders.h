//------------------------------------------------------------------------------
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------
# pragma once


//------------------------------------------------------------------------------
# include <imgui_node_editor.h>


//------------------------------------------------------------------------------
namespace ax {
namespace NodeEditor {
namespace Utilities {


//------------------------------------------------------------------------------
struct BlueprintNodeBuilder
{
    IMGUI_API BlueprintNodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);

    IMGUI_API void Begin(NodeId id);
    IMGUI_API void End();

    IMGUI_API void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
    IMGUI_API void EndHeader();

    IMGUI_API void Input(PinId id);
    IMGUI_API void EndInput();

    IMGUI_API void Middle();

    IMGUI_API void Output(PinId id);
    IMGUI_API void EndOutput();


private:
    enum class Stage
    {
        Invalid,
        Begin,
        Header,
        Content,
        Input,
        Output,
        Middle,
        End
    };

    IMGUI_API bool SetStage(Stage stage);

    IMGUI_API void Pin(PinId id, ax::NodeEditor::PinKind kind);
    IMGUI_API void EndPin();

    ImTextureID HeaderTextureId;
    int         HeaderTextureWidth;
    int         HeaderTextureHeight;
    NodeId      CurrentNodeId;
    Stage       CurrentStage;
    ImU32       HeaderColor;
    ImVec2      NodeMin;
    ImVec2      NodeMax;
    ImVec2      HeaderMin;
    ImVec2      HeaderMax;
    ImVec2      ContentMin;
    ImVec2      ContentMax;
    bool        HasHeader;
};



//------------------------------------------------------------------------------
} // namespace Utilities
} // namespace Editor
} // namespace ax