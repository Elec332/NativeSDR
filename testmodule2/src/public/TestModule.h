//
// Created by Elec332 on 06/11/2021.
//

#ifndef NATIVESDR_TESTMODULE_H
#define NATIVESDR_TESTMODULE_H

#include <pipeline/block/block_base_impl.h>

pipeline::block_ptr createTestBlock();

pipeline::block_ptr createStreamOutBlock();

pipeline::block_ptr createStreamInBlock();

pipeline::block_ptr createStreamFileBlock();

pipeline::block_ptr createFFTBlock();

pipeline::block_ptr createOffsetBlock();

pipeline::block_ptr createResampleBlock();

pipeline::block_ptr createAMBlock();

#endif //NATIVESDR_TESTMODULE_H
