//
// Created by Elec332 on 15/11/2022.
//

#ifndef NATIVESDR_CORE_ACTIONS_H
#define NATIVESDR_CORE_ACTIONS_H

class SDRCoreActions {

public:

    virtual void reloadUSBDevices() = 0;

    virtual void saveSchematic() = 0;

    virtual void saveSchematic(const std::filesystem::path& path) = 0;

};

#endif //NATIVESDR_CORE_ACTIONS_H
