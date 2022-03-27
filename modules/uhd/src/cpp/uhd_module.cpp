//
// Created by Elec332 on 03/03/2022.
//

#include <nativesdr/module/SDRModule.h>
#include <nativesdr/uhd_block.h>
#include <nativesdr/uhd_loader.h>
#include <uhd/device.hpp>

class UHDModule : public ModuleInstance, public USRPDeviceCache {

    uhd_devices devs;

    const uhd_devices& getDevices() override {
        return devs;
    }

    void refresh() override {
        uhd::device_addr_t hint;
        uhd::device_addrs_t devices = uhd::device::find(hint);
        devs.clear();
        for (const auto& d : devices) {
            devs.emplace_back(d.to_string());
        }
//        for (const auto& devs : devices) {
//            std::cout << devs.to_string() << std::endl;
//            uhd::device::sptr dev = uhd::device::make(devs);
////            std::cout << "Device type: " << dev->get_device_type() << std::endl;
////            auto tree = dev->get_tree();
////            printTree(tree, "", "/");
//        }
//        std::cout << "-----" << std::endl;

    }


//    void printTree(uhd::property_tree::sptr& tree, std::string indent, std::string lister) {
//        for (const std::string& name2 : tree->list(lister)) {
//            std::cout << indent << name2 << std::endl;
//            printTree(tree, indent + "   ", lister + "/" + name2);
//        }
//    }

    void init(pipeline::node_manager* nodeManager, const SDRCoreContext* context) override {
        if (isUHDLoaded()) {
            refresh();
            nodeManager->registerSourceBlockType("UHD Source", [&]() {
                return createUHDSource(this);
            });
        }
    }

};


bool initModule() {
    return isUHDLoaded();
}

const char* getModuleName() {
    return "UHD Module";
}

void onShutdownModule() {
}

ModuleInstance* createModuleContainer() {
    return new UHDModule();
}

void destroyModuleContainer(ModuleInstance* instance) {
    delete (UHDModule*) instance;
}