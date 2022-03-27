//
// Created by Elec332 on 04/11/2021.
//

#include <ui/main_window.h>
#include <nativesdr/core_context.h>

void main_window::runCallBack() {
    oldFrequency = frequency;
    if (callback) {
        callback(1);
    }
}

void main_window::checkFrequency() {
    if (frequency != oldFrequency) {
        uint32_t temp = frequency;
        for (uint8_t& i : drawer) {
            i = temp % 10;
            temp -= i;
            temp /= 10;
        }
        runCallBack();
    }
}

void main_window::drawFreqChooser() {
    checkFrequency();
    ImGui::PushFont(big);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    uint8_t fm1 = FREQUENCY_NUMBERS - 1;
    ImVec2 sizeZero = ImGui::CalcTextSize("0");
    ImVec2 mousePos = ImGui::GetMousePos();
    float half = sizeZero.y / 2;
    bool hovered = false;
    bool invalid = true;
    void (* renderTxt)(const char*, ...) = ImGui::TextDisabled;
    bool leftMouse = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    for (int i = fm1; i >= 0; --i) {
        if (i != fm1 && (i + 1) % 3 == 0) {
            renderTxt(".");
            ImGui::SameLine();
        }
        ImVec2 min = ImGui::GetCursorPos();
        min.y += 3;
        ImVec2 max = {min.x + sizeZero.x, min.y + sizeZero.y};
        if (invalid) {
            invalid = drawer[i] == 0;
            if (!invalid) {
                renderTxt = ImGui::Text;
            }
        }
        if (!hovered && isInArea(mousePos, min, max)) {
            int subI = i;
            if (mousePos.y > min.y + half) { //Down
                drawList->AddRectFilled({min.x, min.y + half}, max, IM_COL32(0, 255, 255, 128));
                if (leftMouse && !invalid) {
                    while (subI < FREQUENCY_NUMBERS) {
                        auto& val = drawer[subI];
                        if (val == 0) {
                            val = 9;
                            subI++;
                        } else {
                            val = val - 1;
                            break;
                        }
                    }
                }
            } else { //Up
                drawList->AddRectFilled(min, {max.x, max.y - half}, IM_COL32(255, 0, 0, 128));
                if (leftMouse) {
                    while (subI < FREQUENCY_NUMBERS) {
                        auto& val = drawer[subI];
                        if (val == 9) {
                            val = 0;
                            subI++;
                        } else {
                            val = val + 1;
                            break;
                        }
                    }
                }
            }
            if (leftMouse) {
                frequency = 0;
                for (int j = 0; j < FREQUENCY_NUMBERS; ++j) {
                    frequency += (uint32_t) (drawer[j] * std::pow(10, j));
                }
                runCallBack();
            }
            hovered = true;
        }
        renderTxt("%d", drawer[i]);
        ImGui::SameLine();
    }
    ImGui::Text("");
    ImGui::PopFont();
}

void main_window::drawTopRow(pipeline::schematic* nm) {
    if (ImGui::Button(showEditor ? "Show Interface" : "Show Editor", ImVec2(100, 30))) {
        showEditor = !showEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button(nm->isStarted() ? "Stop" : "Start", ImVec2(100, 30))) {
        if (nm->isStarted()) {
            nm->stop();
        } else {
            nm->start();
        }
    }
    ImGui::SameLine();
    drawFreqChooser();
}

void main_window::drawWindow(pipeline::schematic** nm) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Content", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::PushID("top_row");
    drawTopRow(*nm);
    (*nm)->forEachBlock([&](const pipeline::block_data& w) {
        w->getBlock()->drawDialogs();
    });
    ImGui::PopID();

    ImGui::BeginChild("Main");
    if (showEditor) {
        editor.draw(*nm);
    } else {
        sdr.draw(*nm, context);
    }
    ImGui::EndChild();

    ImGui::End();
}

void drawmain(pipeline::schematic** nm, main_window* window) {
    window->drawWindow(nm);
}

std::shared_ptr<SubContext> main_window::getSubContext() {
    return ctx;
}

class FreqBlock : public pipeline::block {

public:

    explicit FreqBlock(uint64_t*& freq, main_window* window) : pipeline::block("Frequency Chooser", ImColor(255, 255, 0)) {
        window->callback = addOutput("Frequency", utils::frequencyType(), freq, true);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
    }

};


void main_window::init(const std::string& rootDir) {
    mainFB = std::make_shared<FreqBlock>(freqRef, this);
    context.setupGraphics();
    big = ImGui::AddDefaultFont(32);
    ctx = context.createChildContext();
    sdr.init();
    editor.init(rootDir);
}

void main_window::deInit() {
    sdr.deinit();
    editor.deinit();
    context.destroy();
}

void main_window::start(pipeline::schematic** nodes) {
    context.startMainWindow(drawmain, nodes, this);
}

pipeline::block_ptr main_window::createFrequencyBlock() {
    return mainFB;
}

NativeGraphics* main_window::getBackend() {
    return &context;
}
