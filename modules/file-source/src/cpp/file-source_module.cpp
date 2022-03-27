//
// Created by Elec332 on 25/02/2022.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/file-source_block.h>

class FileSourceModule : public ModuleInstance {

    void init(pipeline::node_manager* nodeManager, const SDRCoreContext* context) override {
        nodeManager->registerBlockType("RTL-SDR Source", createFileSource);
    }

};


bool initModule() {
    return true;
}

const char* getModuleName() {
    return "File Source Module";
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new FileSourceModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (FileSourceModule*) instance;
}