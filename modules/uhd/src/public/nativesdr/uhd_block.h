//
// Created by Elec332 on 03/03/2022.
//

#ifndef NATIVESDR_UHD_BLOCK_H
#define NATIVESDR_UHD_BLOCK_H

#include <nativesdr/pipeline/block/block.h>
#include <uhd/types/device_addr.hpp>
#include <nativesdr/uhd_module_export.h>

class USRPDeviceCache;

pipeline::source_block_ptr createUHDSource(USRPDeviceCache* cache);

typedef std::vector<std::string> uhd_devices;

class UHD_MODULE_EXPORT USRPDeviceCache {

public:

    virtual void refresh() = 0;

    virtual const uhd_devices& getDevices() = 0;

};

#endif //NATIVESDR_UHD_BLOCK_H
