//
// Created by Elec332 on 04/11/2021.
//

#include <ui/main_window.h>
#include <nativesdr/core_context.h>

void drawmain(pipeline::schematic** nm, SDRMainWindow* window, SDRCoreActions* actions) {
    window->drawWindow(nm, actions);
}

//std::shared_ptr<SubContext> main_window::getSubContext() {
//    return ctx;
//}

class SDRMainWindowImpl : public SDRMainWindow {

public:

    void init(const std::string &rootDir) override {
        mainFB = std::make_shared<FreqBlock>(freqRef, this);
        context.setupGraphics();
        big = ImGui::AddDefaultFont(32);
        ctx = context.createChildContext();
        sdr.init();
        editor.init(rootDir);
    }

    void deInit() override {
        sdr.deinit();
        editor.deinit();
        context.destroy();
    }

    void runCallBack() {
        oldFrequency = frequency;
        if (callback) {
            callback(1);
        }
    }

    void checkFrequency() {
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

    void start(pipeline::schematic **nodes, SDRCoreActions *actions) override {
        context.startMainWindow(drawmain, nodes, this, actions);
    }

    void drawWindow(pipeline::schematic **nm, SDRCoreActions *actions) override {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Content", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        pipeline::schematic* schematic = *nm;
        drawMenuBar(actions);
        ImGui::PushID("top_row");
        drawTopRow(schematic);
        schematic->forEachBlock([&](const pipeline::block_data& w) {
            w->getBlock()->drawDialogs();
        });
        ImGui::PopID();

        ImGui::BeginChild("Main");
        if (showEditor) {
            editor.draw(schematic);
        } else {
            sdr.draw(schematic, context);
        }
        ImGui::EndChild();

        if (ImGui::BeginPopupModal("Load Error", nullptr, ImGuiWindowFlags_NoScrollbar)) {
            ImGui::Text("The saved schematic failed to load, most likely due to a missing module that provided one of the blocks for this schematic.\n"
                        "Press \"Acknowledge\" to continue but lock the schematic, so nothing will change and it will still work when all blocks are present.\n"
                        "Press \"Ignore\" to ignore the errors and remove the missing blocks and continue.");
            if (ImGui::Button("Acknowledge")) {
                brokenAck = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ignore")) {
                brokenAck = true;
                schematic->ignoreBroken();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if ((size_t) schematic != ackRef) {
            ackRef = (size_t) schematic;
            brokenAck = false;
        }
        if (schematic->isBroken() && !brokenAck) {
            ImGui::OpenPopup("Load Error");
        }

        ImGui::End();
    }

    void drawMenuBar(SDRCoreActions* actions) {
        ImGui::PushID("menu_row");
        if (ImGui::BeginMainMenuBar()){
            if (ImGui::BeginMenu("File")){
                if (ImGui::MenuItem("New", nullptr)) {
                    std::cout << "New" << std::endl;
                }
                if (ImGui::MenuItem("Open", nullptr)) {
                    std::cout << "Open" << std::endl;
                }
                if (ImGui::MenuItem("Save", nullptr)) {
                    actions->saveSchematic();
                }
                if (ImGui::MenuItem("Save As", nullptr)) {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKeySA", "Choose File", ".json", fileDir);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools")){
                if (ImGui::MenuItem("Reload USB Devices", nullptr)) {
                    actions->reloadUSBDevices();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
            ImGui::NewLine();
        }
        ImGui::PopID();

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKeySA", ImGuiWindowFlags_NoCollapse, ImVec2(500, 300))) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                fileDir = ImGuiFileDialog::Instance()->GetCurrentPath();
                actions->saveSchematic(ImGuiFileDialog::Instance()->GetFilePathName());
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void drawFreqChooser() {
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

    void drawTopRow(pipeline::schematic* nm) {
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

    NativeGraphics* getBackend() override {
        return &context;
    }

    pipeline::block_ptr createFrequencyBlock() override {
        return mainFB;
    }

    class FreqBlock : public pipeline::block {

    public:

        explicit FreqBlock(uint64_t*& freq, SDRMainWindowImpl* window) : pipeline::block("Frequency Chooser", ImColor(255, 255, 0)) {
            window->callback = addOutput("Frequency", utils::frequencyType(), freq, true);
        }

        void start() override {
        }

        void stop() override {
        }

        void drawMiddle() override {
        }

    };

private:

    bool showEditor = false;
    uint64_t oldFrequency = 0;
//uint64_t frequency = 99109674;
    uint64_t frequency = 95300000;
    uint64_t* freqRef = &frequency;
    uint8_t drawer[FREQUENCY_NUMBERS];
    ImFont* big;

    NativeGraphics context;
    editor_ui editor;
    sdr_ui sdr;

    std::shared_ptr<SubContext> ctx;

    pipeline::block_ptr mainFB;
    std::string fileDir = ".";

    pipeline::connection_callback callback = nullptr;

    bool brokenAck = false;
    size_t ackRef = 0;

};

std::shared_ptr<SDRMainWindow> createMainWindow() {
    return std::make_shared<SDRMainWindowImpl>();
}
