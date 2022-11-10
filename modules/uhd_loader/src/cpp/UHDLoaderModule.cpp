//
// Created by Elec332 on 03/03/2022.
//

#include <nativesdr/core.h>
#include <nativesdr/module/SDRModule.h>
#include <nativesdr/uhd_loader.h>

class UHDLoaderModule : public ModuleInstance {

    void init(pipeline::node_manager* nodeManager, SDRCoreContext* context) override {
    }

};

bool isUHDPresent = false;
std::shared_ptr<libloader::library> uhdLib = nullptr;

bool initModule() {
#ifdef WIN32
    std::filesystem::path path = std::getenv("UHD_PKG_PATH");
    if (is_directory(path) && !isUHDPresent && !uhdLib) {
        std::filesystem::path p2 = path / "bin" / "uhd.dll";
        if (is_directory(path / "share" / "uhd" / "images") && is_regular_file(p2)) {
            uhdLib = loadLibrary(p2);
        }
    }
#endif
    if (uhdLib->isLoaded()) {
        char args[1] = "";
        auto alloc = (int (*)(void**)) uhdLib->getSymbol("uhd_string_vector_make");
        auto dealloc = (int (*)(void**)) uhdLib->getSymbol("uhd_string_vector_free");
        auto func = (int (*)(char*, void**)) uhdLib->getSymbol("uhd_usrp_find");
        if (!func || !alloc || !dealloc) {
            uhdLib = nullptr;
        } else {
            void* unknown;
            alloc(&unknown);
            func(args, &unknown);
            dealloc(&unknown);
            isUHDPresent = true;
        }
    }
    if (isUHDPresent) {
        std::cout << "UHDLoader loaded UHD libraries successfully!" << std::endl;
    } else {
        std::cout << "UHDLoader failed to load UHD libraries!" << std::endl;
    }
    return isUHDPresent;
}

const char* getModuleName() {
    return "UHD Loader Module";
}

void onShutdownModule() {
    if (uhdLib) {
        uhdLib = nullptr;
        isUHDPresent = false;
    }
}

ModuleInstance* createModuleContainer() {
    return new UHDLoaderModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (UHDLoaderModule*) instance;
}

bool isUHDLoaded() {
    return isUHDPresent;
}