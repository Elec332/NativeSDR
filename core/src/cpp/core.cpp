//
// Created by Elec332 on 10/07/2021.
//

#include <core.h>
#include <filesystem>
#include <module/SDRModule.h>
#include <module/ModuleManager.h>
#include <iostream>
#include <fftw3.h>
#include <map>
#include <utility>
#include <ui/main_window.h>

static void handleModule(const ModulePointer& p) {
    NativeGraphics::setupGraphics();
    std::cout << "MOD" << std::endl;
    ModuleInstance* i = p->createModuleContainer();
    i->test();
    p->destroyModuleContainer(i);
    NativeGraphics::destroy();
    std::cout << "Exiting module" << std::endl;
}

class TestBlock : public pipeline::block {

public:

    TestBlock() : pipeline::block("TestBlock", ImColor(255, 0, 0)) {
        stream = (utils::sampleData*) malloc(sizeof(utils::sampleData));
        addInput("Din", utils::sampleDataType(), stream, true);
        addOutput("Dout", utils::sampleDataType(), stream, true);
    }

    ~TestBlock() {
        free(stream);
    }

    void start() override {
    }

    void stop() override {
    }

    void drawMiddle() override {
    }

private:

    utils::sampleData* stream;

};

pipeline::block_ptr cteare() {
    return std::make_shared<TestBlock>();
}

void test(pipeline::node_manager* nm) {
    nm->registerBlockType("First Block", cteare);
    //nm->addBlock("First Block", 0, 0);
}


int startCore(int argc, char* argv[]) {
    fftwf_init_threads();
    fftwf_plan_with_nthreads(4);
    fftwf_set_timelimit(0.003);

    pipeline::node_manager* nodeManager = newNodeManager();
    pipeline::schematic* schematic = pipeline::newSchematic(nodeManager);
    main_window::init();
    sdr_ui::init();
    editor_ui::init();

    std::filesystem::path exec(argv[0]);
    exec = exec.parent_path();

    std::list<libloader::library> libs = libloader::loadFolder(exec / "modules");
    std::list<ModulePointer> modules = getModules(libs);
    std::map<ModuleInstance*, ModuleContainer*> dealloc;

    //////////////////////////////////////////////////////////////////////


    for (const auto& p: modules) {
        ModuleInstance* i = p->createModuleContainer();
        //handleModule(p);
        std::cout << "Loading module: " << i->getName() << std::endl;
        i->init(nodeManager);
        dealloc[i] = p.get();
    }

    test(nodeManager);

    main_window::start(schematic);

    for (const auto& p: dealloc) {
        std::cout << "Unloading module: " << p.first->getName() << std::endl;
        p.second->destroyModuleContainer(p.first);
    }
    sdr_ui::deinit();
    editor_ui::deinit();
    pipeline::deleteSchematic(schematic);
    deleteNodeManager(nodeManager);
    return 0;
}

void test() {
}

