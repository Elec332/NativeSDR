//
// Created by Elec332 on 10/07/2021.
//

#include <filesystem>
#include <module/ModuleManager.h>

class ModuleContainerImpl : public ModuleContainer {

public:

    explicit ModuleContainerImpl(libloader::library &lib) : library(std::move(lib)) {
        init_h = (void (*)()) library.getSymbol("initModule");
        shutdown_h = (void (*)()) library.getSymbol("onShutdownModule");
        create_h = (ModuleInstance *(*)()) library.getSymbol("createModuleContainer");
        destroy_h = (void (*)(ModuleInstance *)) library.getSymbol("destroyModuleContainer");
    }

    void initModule() override {
        init_h();
    }

    void onShutdownModule() override {
        shutdown_h();
    }

    ModuleInstance *createModuleContainer() override {
        return create_h();
    }

    void destroyModuleContainer(ModuleInstance *instance) override {
        destroy_h(instance);
    }

    bool isValid() {
        return library.isLoaded() && init_h != nullptr && shutdown_h != nullptr && create_h != nullptr &&
               destroy_h != nullptr;
    }

    libloader::library library;

private:

    void (*init_h)();

    void (*shutdown_h)();

    ModuleInstance *(*create_h)();

    void (*destroy_h)(ModuleInstance *);

};

ModulePointer getModule(const std::string &location) {
    if (!std::filesystem::exists(location) || !std::filesystem::is_regular_file(location)) {
        return nullptr;
    }
    libloader::library ref(location);
    return getModule(ref);
}

ModulePointer getModule(libloader::library &lib) {
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

std::list<ModulePointer> getModules(std::list<libloader::library> &libs) {
    std::list<ModulePointer> modules;
    libs.remove_if([&](libloader::library &lib) {
        ModulePointer p = getModule(lib);
        if (p) {
            modules.push_front(p);
            return true;
        }
        return false;
    });
    return modules;
}
