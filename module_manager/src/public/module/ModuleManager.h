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

    virtual void shutdownOnDestruct() = 0;

    virtual bool initModule() = 0;

    virtual std::string getModuleName() = 0;

    virtual void shutdownModule() = 0;

    virtual ModuleInstance* createModuleContainer() = 0;

    virtual void destroyModuleContainer(ModuleInstance* instance) = 0;

};

typedef std::shared_ptr<ModuleContainer> ModulePointer;

ModulePointer getModule(const std::string& location);

ModulePointer getModule(libloader::library& lib);

bool getModules(std::list<libloader::library>& libs, std::list<ModulePointer>& moduleList, const std::function<void(std::string duplicate)>& failCallback);

bool loadModules(std::list<libloader::library>& libs, std::list<ModulePointer>& moduleList, const std::filesystem::path& path, const std::function<void(std::string duplicate)>& failCallback);

#endif //NATIVESDR_MODULEMANAGER_H
