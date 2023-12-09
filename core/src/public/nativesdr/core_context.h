//
// Created by Elec332 on 17/02/2022.
//

#ifndef NATIVESDR_CORE_CONTEXT_H
#define NATIVESDR_CORE_CONTEXT_H

#include <nativesdr/NativeSDRGraphicsInfo.h>
#include <nativesdr/core_export.h>

class SDRCoreContext {

public:

    [[nodiscard]] virtual std::string getSDRRunDirectory() const = 0;

    virtual NativeGraphicsInfo* getGraphicsInfo() = 0;

    virtual std::shared_ptr<SubContext> getGraphicsSubContext() = 0;

    virtual void registerUSBChangeListener(std::function<void()>) = 0;

};


#endif //NATIVESDR_CORE_CONTEXT_H
