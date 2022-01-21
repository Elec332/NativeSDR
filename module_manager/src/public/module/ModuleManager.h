//
// Created by Elec332 on 10/07/2021.
//

#ifndef NATIVESDR_MODULEMANAGER_H
#define NATIVESDR_MODULEMANAGER_H

#include <memory>
#include <module/module.h>
#include <module/libloader.h>

class ModuleContainer {

public:

    virtual void initModule() = 0;

    virtual void onShutdownModule() = 0;

    virtual ModuleInstance* createModuleContainer() = 0;

    virtual void destroyModuleContainer(ModuleInstance* instance) = 0;

};

typedef std::shared_ptr<ModuleContainer> ModulePointer;

ModulePointer getModule(const std::string& location);

ModulePointer getModule(libloader::library& lib);

std::list<ModulePointer> getModules(std::list<libloader::library>& libs);

#endif //NATIVESDR_MODULEMANAGER_H
