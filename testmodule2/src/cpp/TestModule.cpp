//
// Created by Elec332 on 10/07/2021.
//

#define IM_INTERNAL
#include <module/SDRModule.h>
#include <testbase.h>
#include <TestModule.h>

class TestBlock : public pipeline::block {

public:

    TestBlock() : pipeline::block("TestBlock", ImColor(255, 0, 0)) {
        txt.resize(10);
        std::cout << txt.capacity() << std::endl;
        addInput("Din", utils::stringType(), inTxt, []() {
            std::cout << "Change" << std::endl;
        }, true);
        o1 = addOutput("Dout", utils::stringType(), outTxt, true);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
        ImGui::PushItemWidth(200);
        if (ImGui::InputText("", &txt)) {
            if (o1 == nullptr) {
                std::cout << "NULLLLLL" << std::endl;
            }
            o1();
        }
        ImGui::PopItemWidth();
        ImGui::Spring(1, 0);
        std::string str = "Hatseflatsv2 " + (inTxt ? *inTxt : "");
        ImGui::TextUnformatted(str.c_str());
    }

private:

    pipeline::connection_callback o1;
    std::string txt;
    std::string* outTxt = &txt;
    std::string* inTxt = nullptr;

};

class Test : public ModuleInstance {

    [[nodiscard]] std::string getName() const override {
        return "testModule2";
    }

    void init(pipeline::node_manager *nodeManager) override {
        nodeManager->registerBlockType("Test Block 1", createTestBlock);
    }

};


void initModule() {
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new Test();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (Test*) instance;
}

pipeline::block_ptr createTestBlock() {
    return std::make_shared<TestBlock>();
}
