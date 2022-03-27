//
// Created by Elec332 on 25/02/2022.
//

#include <nativesdr/core_blocks.h>
#include "subinit.h"

void register_sdr_components(pipeline::node_manager* nodeManager) {
    nodeManager->registerBlockType("FFT Screen", createFFTBlock);
    nodeManager->registerBlockType("VFO Block", createVFOBlock);
    nodeManager->registerBlockType("Resampling Block", createResamplingBlock);
    nodeManager->registerBlockType("AM Demodulation Block", createAMDemodulationBlock);
    nodeManager->registerBlockType("DC Blocker", createDCBlockerBlock);
}