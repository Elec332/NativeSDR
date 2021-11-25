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
#include <ui/main_window.h>
#include "subinit.h"

int startCore(int argc, char* argv[]) {
    std::filesystem::path exec(argv[0]);
    exec = exec.parent_path();

    init_malloc();
    fftwf_init_threads();
    fftwf_plan_with_nthreads(4);
    fftwf_set_timelimit(0.003);
    main_window::init();
    sdr_ui::init();
    editor_ui::init();

    std::list<libloader::library> libs = libloader::loadFolder(exec / "modules");
    std::list<ModulePointer> modules = getModules(libs);
    std::map<ModuleInstance*, ModuleContainer*> dealloc;

    //////////////////////////////////////////////////////////////////////

    pipeline::node_manager* nodeManager = newNodeManager();
    nodeManager->registerBlockType("UI", sdr_ui::createUIBlock);
    nodeManager->registerBlockType("Frequency Chooser", main_window::createFrequencyBlock);
    register_ui_components(nodeManager);
    for (const auto& p: modules) {
        ModuleInstance* i = p->createModuleContainer();
        std::cout << "Loading module: " << i->getName() << std::endl;
        i->init(nodeManager);
        dealloc[i] = p.get();
    }
    pipeline::schematic* schematic = pipeline::newSchematic(nodeManager, exec / "start.json");

    main_window::start(&schematic);

    sdr_ui::deinit();
    editor_ui::deinit();
    schematic->save();
    pipeline::deleteSchematic(schematic);
    deleteNodeManager(nodeManager);
    for (const auto& p: dealloc) {
        std::cout << "Unloading module: " << p.first->getName() << std::endl;
        p.second->destroyModuleContainer(p.first);
    }
    return 0;
}

void test() {
}

