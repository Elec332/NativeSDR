//
// Created by Elec332 on 05/11/2021.
//

#ifndef NATIVESDR_BLOCK_DATA_H
#define NATIVESDR_BLOCK_DATA_H

#include <pipeline/block/block.h>

namespace pipeline {

    class block_data_instance {

    public:

        [[nodiscard]] virtual pipeline::block* getBlock() const = 0;

        [[nodiscard]] size_t getIdInt() const {
            return (size_t) getId();
        };

        [[nodiscard]] virtual ax::NodeEditor::NodeId getId() const = 0;

    };

    typedef std::shared_ptr<block_data_instance> block_data;

}

#endif //NATIVESDR_BLOCK_DATA_H
