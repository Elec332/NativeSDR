//
// Created by Elec332 on 09/12/2021.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/portaudio_block.h>
#include <portaudio.h>

class PortAudioModule : public ModuleInstance {

    void init(pipeline::node_manager* nodeManager, SDRCoreContext* context) override {
        nodeManager->registerBlockType("Audio Sink", createAudioSinkBlock);
    }

};

bool initModule() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << "Failed to load PortAudio: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    printf("PortAudio version: 0x%08X / '%s'\n", Pa_GetVersion(), Pa_GetVersionText());
    return true;
}

const char* getModuleName() {
    return "PortAudio Module";
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new PortAudioModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (PortAudioModule*) instance;
}
