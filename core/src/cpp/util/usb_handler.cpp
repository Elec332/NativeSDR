//
// Created by Elec332 on 08/11/2022.
//

#include <libusb.h>
#include <iostream>
#include "util/usb_handler.h"

class LibUSBImpl : public USBHandler {

public:

    int init() override {
        if (initialized) {
            std::cout << "LibUSB has already been initialized" << std::endl;
            return -1;
        }
        int regc = libusb_init(nullptr);
        if (regc != LIBUSB_SUCCESS) {
            std::cout << "Failed to initialize LibUSB: " << libusb_error_name(regc) << std::endl;
            return regc;
        }
        initialized = true;
        regc = libusb_hotplug_register_callback(nullptr, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_NO_FLAGS, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, usb_hotplug_callback, this, &cb_handler);
        if (regc != LIBUSB_SUCCESS) {
            std::cout << "Failed to initialize LibUSB callbacks: " << libusb_error_name(regc) << std::endl;
            libusb_exit(nullptr);
            return regc;
        }
        return 0;
    }

    int deInit() override {
        if (cb_handler != 0) {
            libusb_hotplug_deregister_callback(nullptr, cb_handler);
            cb_handler = 0;
        }
        return 0;
    }

    static int usb_hotplug_callback(struct libusb_context* ctx, struct libusb_device* dev, libusb_hotplug_event event, void* user_data) {
        std::cout << "Hotplug debug" << std::endl; //TODO: Remove
        auto instance = (LibUSBImpl*) user_data;
        instance->reload();
        return 0;
    }

    ~LibUSBImpl() override {
        if (initialized) {
            libusb_exit(nullptr);
        }
    }

private:

    bool initialized = false;
    int cb_handler = 0;

};

class NullImpl : public USBHandler {

public:

    int init() override {
        return 0;
    }

    int deInit() override {
        return 0;
    }

};

std::shared_ptr<USBHandler> createUSBHandler() {
    if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        return std::make_shared<LibUSBImpl>();
    }
    return std::make_shared<NullImpl>();
}

