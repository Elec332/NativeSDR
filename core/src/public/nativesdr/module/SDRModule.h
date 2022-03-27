//
// Created by Elec332 on 10/07/2021.
//

#ifndef NATIVESDR_SDRMODULE_H
#define NATIVESDR_SDRMODULE_H

#include <module/module.h>
#include <string>
#include <nativesdr/pipeline/block/node_manager.h>
#include <nativesdr/core_context.h>

class ModuleInstance {

public:

    virtual void init(pipeline::node_manager* nodeManager, const SDRCoreContext* context) = 0;

};

#endif //NATIVESDR_SDRMODULE_H
