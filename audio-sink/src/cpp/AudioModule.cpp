//
// Created by Elec332 on 09/12/2021.
//

#include <module/SDRModule.h>
#include <AudioModule.h>
#include <portaudio.h>

class Module : public ModuleInstance {

    [[nodiscard]] std::string getName() const override {
        return "AudioModule";
    }

    void init(pipeline::node_manager* nodeManager) override {
        nodeManager->registerBlockType("Audio Sink", createAudioSinkBlock);
    }

};

void initModule() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        throw std::runtime_error(Pa_GetErrorText(err));
    }
    printf("PortAudio version: 0x%08X / '%s'\n", Pa_GetVersion(), Pa_GetVersionText());
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new Module();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (Module*) instance;
}
