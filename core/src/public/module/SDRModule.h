//
// Created by Elec332 on 10/07/2021.
//

#ifndef NATIVESDR_SDRMODULE_H
#define NATIVESDR_SDRMODULE_H

#include <module/module.h>
#include <string>
#include <pipeline/block/node_manager.h>

class ModuleInstance {

public:

    [[nodiscard]] virtual std::string getName() const = 0;

    virtual void init(pipeline::node_manager* nodeManager) = 0;

};

#endif //NATIVESDR_SDRMODULE_H
