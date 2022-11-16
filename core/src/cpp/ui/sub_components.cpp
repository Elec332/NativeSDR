//
// Created by Elec332 on 19/11/2021.
//

#include <utility>

#include <subinit.h>

class SubDrawer : public pipeline::block {

public:

    SubDrawer(std::string name, ImColor color) : pipeline::block(std::move(name), color) {
        drawFunc = [&](size_t random) {
            draw(random);
        };
        out = &drawFunc;
        addOutput("Renderer", utils::uiType(), out, true);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
    }

    virtual void draw(size_t random) = 0;

private:

    utils::drawFunc drawFunc;
    utils::drawFunc* out = nullptr;

};

class HSBlock : public SubDrawer {

public:

    HSBlock() : SubDrawer("Horizontal Split Sink", ImColor(0, 0, 255)) {
        addInput("Left", utils::uiType(), left);
        addInput("Right", utils::uiType(), right);
    }

    void draw(size_t random) override {
        ImVec2 totalSize = ImGui::GetWindowSize();
        float divider = 4;
        totalSize.x -= divider;
        auto leftPaneWidth = totalSize.x * size;
        auto rightPaneWidth = totalSize.x * (1.0f - size);

        ImGui::Splitter(true, divider, &leftPaneWidth, &rightPaneWidth, 50, 50);
        ImGui::BeginChild("Left", {leftPaneWidth, totalSize.y});
//        ImGui::DebugGreenBox();
        if (left) {
            (*left)(random);
        }
        ImGui::EndChild();
        ImGui::SameLine(0, divider);
        ImGui::BeginChild("Right", {rightPaneWidth, totalSize.y});
//        ImGui::DebugGreenBox();
        if (right) {
            (*right)(random + 512);
        }
        ImGui::EndChild();
        size = std::min(leftPaneWidth / totalSize.x, 0.95f);
    }

    void readJson(const nlohmann::json& json) override {
        size = json["split"].get<float>();
    }

    void toJson(nlohmann::json& json) const override {
        json["split"] = size;
    }

private:

    float size = 0.5;

    utils::drawFunc* left = nullptr;
    utils::drawFunc* right = nullptr;

};

class VSBlock : public SubDrawer {

public:

    VSBlock() : SubDrawer("Vertical Split Sink", ImColor(0, 0, 255)) {
        addInput("Up", utils::uiType(), left);
        addInput("Down", utils::uiType(), right);
    }

    void draw(size_t random) override {
        ImVec2 totalSize = ImGui::GetWindowSize();
        float divider = 4;
        totalSize.y -= divider;
        auto upPaneWidth = std::ceil(totalSize.y * size);
        auto downPaneWidth = std::ceil(totalSize.y * (1.0f - size));

        ImGui::Splitter(false, divider, &upPaneWidth, &downPaneWidth, 100, 100);
        if (upPaneWidth + downPaneWidth < totalSize.y) {
            upPaneWidth++;
        }
        if (upPaneWidth + downPaneWidth > totalSize.y) {
            downPaneWidth--;
        }

        ImGui::BeginChild("Up", {totalSize.x, upPaneWidth});
//        ImGui::DebugGreenBox();
        if (left) {
            (*left)(random);
        }
        ImGui::EndChild();

        ImGui::BeginChild("Down", {totalSize.x, downPaneWidth});
//        ImGui::DebugGreenBox();
        if (right) {
            (*right)(random + 512);
        }
        ImGui::EndChild();

        size = std::min(upPaneWidth / totalSize.y, 0.95f);
    }

    void readJson(const nlohmann::json& json) override {
        size = json["split"].get<float>();
    }

    void toJson(nlohmann::json& json) const override {
        json["split"] = size;
    }

private:

    float size = 0.5;

    utils::drawFunc* left = nullptr;
    utils::drawFunc* right = nullptr;

};

class NumberBlock : public pipeline::block {

public:

    NumberBlock() : pipeline::block("Number", ImColor(0, 0, 255)) {
        val = 100000;
        cb = addOutput("Out", utils::numberType(), valRef, true);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
        ImGui::TextUnformatted("Value: ");
        ImGui::PushItemWidth(100);
        if (ImGui::InputText("##Value", &value)) {
            int nv = std::stoi(value);
            if (nv != val) {
                val = nv;
                cb(0);
            }
        }
        ImGui::PopItemWidth();
        value = std::to_string(val);
    }

    void readJson(const nlohmann::json& json) override {
        val = json["value"].get<int>();
    }

    void toJson(nlohmann::json& json) const override {
        json["value"] = val;
    }

private:

    pipeline::connection_callback cb;
    std::string value;
    int val = 0;
    int* valRef = &val;

};

pipeline::block_ptr createHSBlock() {
    return std::make_shared<HSBlock>();
}

pipeline::block_ptr createVSBlock() {
    return std::make_shared<VSBlock>();
}

pipeline::block_ptr createNumberBlock() {
    return std::make_shared<NumberBlock>();
}

void register_ui_components(pipeline::node_manager* nodeManager, const std::shared_ptr<SDRMainWindow>& window) {
    nodeManager->registerBlockType("UI", sdr_ui::createUIBlock);
    nodeManager->registerBlockType("Frequency Chooser", [window]() {
        return window->createFrequencyBlock();
    });

    nodeManager->registerBlockType("Horizontal Splitter", createHSBlock);
    nodeManager->registerBlockType("Vertical Splitter", createVSBlock);
    nodeManager->registerBlockType("Number", createNumberBlock);
}
