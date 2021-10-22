//
// Created by Elec332 on 21/10/2021.
//

#include "drawtest.h"
#include <NativeSDRGraphics.h>

namespace ne = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

static ne::EditorContext* ctx = nullptr;

void init() {
    ctx = ne::CreateEditor();
}

void deinit() {
    ne::DestroyEditor(ctx);
}

void draw() {
    ne::SetCurrentEditor(ctx);
    ne::Begin("Window Test");
    ne::End();
}
