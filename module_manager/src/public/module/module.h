//
// Created by Elec332 on 10/07/2021.
//

#ifndef NATIVESDR_MODULE_H
#define NATIVESDR_MODULE_H

#ifdef _WIN32
#define MODULE_MARKER extern "C" __declspec(dllexport)
#else
#define MODULE_MARKER extern "C"
#endif

class ModuleInstance;

MODULE_MARKER bool initModule();

MODULE_MARKER const char* getModuleName();

MODULE_MARKER void onShutdownModule();

MODULE_MARKER ModuleInstance* createModuleContainer();

MODULE_MARKER void destroyModuleContainer(ModuleInstance* instance);

#endif //NATIVESDR_MODULE_H
