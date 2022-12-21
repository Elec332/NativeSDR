//
// Created by Elec332 on 08/11/2022.
//

#ifndef NATIVESDR_USB_HANDLER_H
#define NATIVESDR_USB_HANDLER_H

#include <functional>
#include <mutex>
#include <list>
#include <memory>

class USBHandler {

public:

    virtual int init() = 0;

    void registerUSBChangeListener(std::function<void()> callback) {
        callback();
        std::unique_lock<std::mutex> raii(callbackLock);
        usbCallbacks.emplace_back(std::move(callback));
    }

    void reload() {
        std::unique_lock<std::mutex> raii(callbackLock);
        for (const auto& cb : usbCallbacks) {
            cb();
        }
    }

    virtual int deInit() = 0;

    void cleanup() {
        std::unique_lock<std::mutex> raii(callbackLock);
        usbCallbacks.clear();
    }

    virtual ~USBHandler() = default;

private:

    std::list<std::function<void()>> usbCallbacks;
    std::mutex callbackLock;

};

std::shared_ptr<USBHandler> createUSBHandler();

#endif //NATIVESDR_USB_HANDLER_H
