//
// Created by Elec332 on 10/07/2021.
//

#include <core.h>
#include <NativeSDRGraphics.h>
#include <filesystem>
#include <module/ModuleManager.h>
#include <module/SDRModule.h>
#include <iostream>

static void handleModule(const ModulePointer& p) {
    NativeGraphics::setupGraphics();
    std::cout << "MOD" << std::endl;
    ModuleInstance* i = p->createModuleContainer();
    i->test();
    p->destroyModuleContainer(i);
    NativeGraphics::destroy();
}

int startCore(int argc, char* argv[]) {
    std::filesystem::path exec(argv[0]);
    exec = exec.parent_path();

    std::list<libloader::library> libs = libloader::loadFolder(exec / "modules");
    std::list<ModulePointer> modules = getModules(libs);

    //////////////////////////////////////////////////////////////////////

    for(const auto &p : modules) {
        handleModule(p);
        std::cout << "Exiting module" << std::endl;
    }

    return 0;
}

