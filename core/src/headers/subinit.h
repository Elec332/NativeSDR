//
// Created by Elec332 on 22/10/2021.
//

#ifndef NATIVESDR_SUBINIT_H
#define NATIVESDR_SUBINIT_H

#include <nativesdr/pipeline/block/node_manager.h>
#include "ui/main_window.h"

void init_object_types();

void init_malloc();

void register_ui_components(pipeline::node_manager* nodeManager, const std::shared_ptr<SDRMainWindow>& window);

void register_sdr_components(pipeline::node_manager* nodeManager);

#endif //NATIVESDR_SUBINIT_H
