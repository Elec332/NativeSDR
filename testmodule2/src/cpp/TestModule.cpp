//
// Created by Elec332 on 10/07/2021.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/TestModule.h>

class TestBlockSI : public pipeline::block {

public:

    TestBlockSI() : pipeline::block("TestBlock", ImColor(255, 0, 0)) {
        data = (uint8_t*) malloc(2);
        data[0] = data[1] = 0;
        drawFunc = [&](size_t) {
            std::string str = "Hatseflats In:  " + std::to_string(data[0]) + " " + std::to_string(data[1]);
            ImGui::TextUnformatted(str.c_str());
        };
        drawFuncRef = &drawFunc;
        addInput("Din", utils::dataStreamType(), stream, [](int flags) {
            std::cout << "Change " << flags << std::endl;
        });
        addOutput("Renderer", utils::uiType(), drawFuncRef, true);
    }

    ~TestBlockSI() {
        free(data);
    }

    void start() override {
        stopped = false;
        thread = std::thread([&]() {
            while (true) {
                if (stopped) {
                    return;
                }
                if (stream) {
                    stream->read([&](const uint8_t* dat, int len) {
                        memcpy(data, dat, len);
                    });
                } else {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        });
    }

    void stop() override {
        if (stream) {
            stream->stop();
        }
        std::cout << "STOP IN" << std::endl;
        stopped = true;
        thread.join();
    }

    void drawMiddle() override {
        ImGui::Spring(1, 0);
        std::string str = "Hatseflats In:  " + std::to_string(data[0]) + " " + std::to_string(data[1]);
        ImGui::TextUnformatted(str.c_str());
        str = std::to_string((size_t) this);
        ImGui::TextUnformatted(str.c_str());
    }

private:

    std::thread thread;
    bool stopped = false;
    uint8_t* data;
    pipeline::datastream<uint8_t>* stream = nullptr;
    utils::drawFunc drawFunc;
    utils::drawFunc* drawFuncRef;

};

class TestBlockSO : public pipeline::block {

public:

    TestBlockSO() : pipeline::block("TestBlock", ImColor(255, 0, 0)) {
        addOutput("Dout", utils::dataStreamType(), stream, true);
        stream = pipeline::createStream<uint8_t>();
        data = (uint8_t*) malloc(2);
        srand(time(nullptr));
    }

    ~TestBlockSO() {
        pipeline::deleteStream(stream);
        free(data);
    }

    void start() override {
        stream->start();
        stopped = false;
        thread = std::thread([&]() {
            while (true) {
                if (stopped) {
                    return;
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
                if (stopped) {
                    return;
                }
                data[0] = rand() % 100;
                data[1] = rand() % 100;
                stream->write([&](uint8_t* dat) {
                    memcpy(dat, data, 2);
                    return 2;
                });
            }
        });
    }

    void stop() override {
        stream->stop();
        stopped = true;
        std::cout << "STOP" << std::endl;
        thread.join();
    }

    void drawMiddle() override {
        ImGui::Spring(1, 0);
        std::string str = "Hatseflats out:  " + std::to_string(data[0]) + " " + std::to_string(data[1]);
        ImGui::TextUnformatted(str.c_str());
    }

private:

    std::thread thread;
    bool stopped = false;
    uint8_t* data;
    pipeline::datastream<uint8_t>* stream;

};

class TestBlock : public pipeline::block {

public:

    TestBlock() : pipeline::block("TestBlock", ImColor(255, 0, 0)) {
        txt.resize(10);
        addInput("Din", utils::stringType(), inTxt, [](int flags) {
            std::cout << "Change " << flags << std::endl;
        });
        o1 = addOutput("Dout", utils::stringType(), outTxt, true);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
        ImGui::PushItemWidth(200);
        if (ImGui::InputText("", &txt)) {
            o1(1);
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

    void init(pipeline::node_manager* nodeManager) override {
        nodeManager->registerBlockType("Test Block 1", createTestBlock);
        nodeManager->registerBlockType("Test Block Out", createStreamOutBlock);
        nodeManager->registerBlockType("Test Block In", createStreamInBlock);
        nodeManager->registerBlockType("File Stream Block Out", createStreamFileBlock);
        nodeManager->registerBlockType("FFT Block", createFFTBlock);
        nodeManager->registerBlockType("VFO Block", createOffsetBlock);
        nodeManager->registerBlockType("Resample Block", createResampleBlock);
        nodeManager->registerBlockType("AM Block", createAMBlock);
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

pipeline::block_ptr createStreamInBlock() {
    return std::make_shared<TestBlockSI>();
}

pipeline::block_ptr createStreamOutBlock() {
    return std::make_shared<TestBlockSO>();
}


