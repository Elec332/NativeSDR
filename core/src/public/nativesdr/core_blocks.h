//
// Created by Elec332 on 25/02/2022.
//

#ifndef NATIVESDR_CORE_BLOCKS_H
#define NATIVESDR_CORE_BLOCKS_H

#include <nativesdr/core_export.h>
#include <nativesdr/pipeline/block/block.h>

CORE_EXPORT pipeline::block_ptr createFFTBlock();

CORE_EXPORT pipeline::block_ptr createVFOBlock();

CORE_EXPORT pipeline::block_ptr createResamplingBlock();

CORE_EXPORT pipeline::block_ptr createAMDemodulationBlock();

CORE_EXPORT pipeline::block_ptr createDCBlockerBlock();

#endif //NATIVESDR_CORE_BLOCKS_H
