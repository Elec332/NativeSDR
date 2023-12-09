//
// Created by Elec332 on 10/07/2021.
//

#include <filesystem>
#include <nativesdr/core.h>
#include <nativesdr/module/SDRModule.h>
#include <module/ModuleManager.h>
#include <iostream>
#include <fftw3.h>
#include <map>
#include <ui/main_window.h>
#include "subinit.h"
#include "util/usb_handler.h"
#include "nativesdr/core_actions.h"
#include <nativesdr/core_context.h>

class SDRCore : public SDRCoreContext, public SDRCoreActions {

public:

    int startCoreInstance(int argc, char* argv[]) {
        std::filesystem::path exec(argv[0]);
        exec = exec.parent_path();
        runDir = exec.string();

        std::string extraDir;
        if (argc > 2 && argv[1] == std::string("-md")) {
            extraDir += argv[2];
        }
        init_malloc();

        std::list<libloader::library> libs;
        for (const auto& s : libloader::getLoadedLibraries()) {
            libs.push_front(libloader::createFakeLibrary(s));
        }

        std::list<ModulePointer> modules;
        libloader::loadFolder(libs, exec);

        if (!loadModules(libs, modules, exec / "lib", nullptr)) {
            return -1;
        }
        if (!loadModules(libs, modules, exec / "modules", nullptr)) {
            return -1;
        }
        if (!extraDir.empty() && !loadModules(libs, modules, extraDir, nullptr)) {
            return -1;
        }
//    fftwf_init_threads(); TODO: Check
//    fftwf_plan_with_nthreads(4);
        fftwf_set_timelimit(0.003);
        usbHandler = createUSBHandler();
        int c = usbHandler->init();
        if (c != 0) {
            return c;
        }

        window = createMainWindow();
        window->init(exec.string());
        std::map<ModuleInstance*, ModuleContainer*> dealloc;

        //////////////////////////////////////////////////////////////////////

        pipeline::node_manager* nodeManager = newNodeManager();
        register_ui_components(nodeManager, window);
        register_sdr_components(nodeManager);
        for (const auto& p : modules) {
            ModuleInstance* i = p->createModuleContainer();
            std::cout << "Instantiating module: " << p->getModuleName() << std::endl;
            i->init(nodeManager, this);
            dealloc[i] = p.get();
        }
        std::cout << "Loading config: " << exec / "start.json" << std::endl;
        currentSchematic = pipeline::newSchematic(nodeManager, exec / "start.json");

        window->start(&currentSchematic, this);

        window->deInit();
        usbHandler->deInit();
        usbHandler->cleanup();
        currentSchematic->save();
        pipeline::deleteSchematic(currentSchematic);
        deleteNodeManager(nodeManager);
        for (const auto& p : dealloc) {
            std::cout << "Unloading module: " << p.second->getModuleName() << std::endl;
            p.second->destroyModuleContainer(p.first);
        }
        return 0;
    }

    [[nodiscard]] std::string getSDRRunDirectory() const override {
        return runDir;
    }

    NativeGraphicsInfo* getGraphicsInfo() override {
        return window->getBackend();
    }

    std::shared_ptr<SubContext> getGraphicsSubContext() override {
        return window->getBackend()->createChildContext();
    }

    void registerUSBChangeListener(std::function<void()> callback) override {
        usbHandler->registerUSBChangeListener(std::move(callback));
    }

    void reloadUSBDevices() override {
        usbHandler->reload();
    }

    void saveSchematic() override {
        currentSchematic->save();
    }

    void saveSchematic(const std::filesystem::path &path) override {
        currentSchematic->save(path);
    }

private:

    std::shared_ptr<SDRMainWindow> window;
    std::string runDir;
    std::shared_ptr<USBHandler> usbHandler;
    pipeline::schematic* currentSchematic;

};

int startCore(int argc, char* argv[]) {
    SDRCore core;
    return core.startCoreInstance(argc, argv);
}

int testerrr() {
    std::cout << "Jopoooo" << std::endl;
    return 7;
}

std::shared_ptr<libloader::library> loadLibrary(const std::string& path) {
    return std::make_shared<libloader::library>(path);
}

std::shared_ptr<libloader::library> loadLibrary(const std::filesystem::path& path) {
    return std::make_shared<libloader::library>(path);
}
