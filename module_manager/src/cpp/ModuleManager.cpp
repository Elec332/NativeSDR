//
// Created by Elec332 on 10/07/2021.
//

#include <filesystem>
#include <module/ModuleManager.h>
#include <set>
#include <iostream>

class ModuleContainerImpl : public ModuleContainer {

public:

    explicit ModuleContainerImpl(libloader::library& lib) : library(std::move(lib)) {
        init_h = (bool (*)()) library.getSymbol("initModule");
        shutdown_h = (void (*)()) library.getSymbol("onShutdownModule");
        create_h = (ModuleInstance* (*)()) library.getSymbol("createModuleContainer");
        destroy_h = (void (*)(ModuleInstance*)) library.getSymbol("destroyModuleContainer");
        char* (* nameFunc)() = (char* (*)()) library.getSymbol("getModuleName");
        if (nameFunc != nullptr) {
            name = nameFunc();
        } else {
            init_h = nullptr;
            name = "Invalid";
        }
    }

    ~ModuleContainerImpl() {
        if (delShut) {
            this->shutdownModule();
        }
    }

    void shutdownOnDestruct() override {
        delShut = true;
    }

    bool initModule() override {
        return init_h();
    }

    std::string getModuleName() override {
        return name;
    }

    void shutdownModule() override {
        shutdown_h();
    }

    ModuleInstance* createModuleContainer() override {
        return create_h();
    }

    void destroyModuleContainer(ModuleInstance* instance) override {
        destroy_h(instance);
    }

    bool isValid() {
        return library.isLoaded() && init_h != nullptr && shutdown_h != nullptr && create_h != nullptr && destroy_h != nullptr;
    }

    bool delShut = false;
    std::string name;
    libloader::library library;

private:

    bool (* init_h)();

    void (* shutdown_h)();

    ModuleInstance* (* create_h)();

    void (* destroy_h)(ModuleInstance*);

};

ModulePointer getModule(const std::string& location) {
    if (!std::filesystem::exists(location) || !std::filesystem::is_regular_file(location)) {
        return nullptr;
    }
    libloader::library ref(location);
    return getModule(ref);
}

ModulePointer getModule(libloader::library& lib) {
    if (!lib.isLoaded()) {
        return nullptr;
    }
    std::shared_ptr<ModuleContainerImpl> ret = std::make_shared<ModuleContainerImpl>(lib);
    if (!ret->isValid()) {
        lib = std::move(ret->library); //Move lib ownership back
        ret.reset();
        return nullptr;
    }
    return ret;
}

inline bool runOnLib(bool& failed, libloader::library& lib, std::set<std::string>& moduleNames, const std::function<void(std::string duplicate)>& failCallback, std::list<ModulePointer>& moduleList) {
    if (failed) {
        return true;
    }
    ModulePointer p = getModule(lib);
    if (!p) {
        return false;
    }
    std::string name = p->getModuleName();
    if (!moduleNames.insert(name).second) {
        if (failCallback) {
            failCallback(name);
        } else {
            std::cout << "Duplicate module found: " << name << std::endl;
        }
        failed = true;
        return true;
    }
    moduleList.push_front(p);
    if (p->initModule()) {
        p->shutdownOnDestruct();
        return true;
    } else {
        p->shutdownModule();
        lib = std::move(((ModuleContainerImpl*) p.get())->library); //Move lib ownership back
        moduleList.pop_front();
        return false;
    }
}

bool getModules(std::list<libloader::library>& libs, std::list<ModulePointer>& moduleList, const std::function<void(std::string duplicate)>& failCallback) {
    std::set<std::string> moduleNames;
    bool failed = false;
    for (const auto& m : moduleList) {
        std::string name = m->getModuleName();
        if (!moduleNames.insert(name).second) {
            if (failCallback) {
                failCallback(name);
            } else {
                std::cout << "Duplicate module found: " << name << std::endl;
            }
            moduleList.clear();
            libs.clear();
            return false;
        }
    }
    libs.remove_if([&](libloader::library& lib) {
        return runOnLib(failed, lib, moduleNames, failCallback, moduleList);
    });
    if (failed) {
        moduleList.clear();
        libs.clear();
        return false;
    }
    return true;
}

bool loadModules(std::list<libloader::library>& libs, std::list<ModulePointer>& moduleList, const std::filesystem::path& path, const std::function<void(std::string)>& failCallback) {
    std::set<std::string> moduleNames;
    bool failed = false;
    libloader::loadFolder(libs, [&](libloader::library& lib) {
        return runOnLib(failed, lib, moduleNames, failCallback, moduleList);
    }, path);
    if (failed) {
        moduleList.clear();
        libs.clear();
        return false;
    }
    return true;
}
