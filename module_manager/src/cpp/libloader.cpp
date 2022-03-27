//
// Created by Elec332 on 18/10/2021.
//

#include <filesystem>
#include <utility>
#include <module/libloader.h>

#ifdef _WIN32

#include <Windows.h>
#include <TlHelp32.h>

#else
#include <dlfcn.h>
#endif

libloader::library::library(const std::string& path) : location(path) {
    if (path[0] == '^') {
        location = location.substr(1);
        return;
    }
#ifdef _WIN32
    this->handle = LoadLibrary(path.c_str());
#else
    this->handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
}

void libloader::library::close() {
    if (!handle) {
        return;
    }
#ifdef _WIN32
    FreeLibrary((HMODULE) handle);
#else
    dlclose(this->handle);
#endif
    handle = nullptr;
}

void* libloader::library::getSymbol(const std::string& name) const {
#ifdef _WIN32
    return (void*) GetProcAddress((HMODULE) this->handle, name.c_str());
#else
    return dlsym(this->handle, name.c_str());
#endif
}

bool libloader::library::isLoaded() const {
    return this->handle != nullptr;
}

libloader::library::~library() {
    close();
}

bool libloader::library::hasSymbol(const std::string& name) const {
    return getSymbol(name);
}

std::string libloader::library::getLocation() const {
    return location;
}

libloader::library::library(libloader::library&& other) noexcept {
    location = other.location;
    handle = other.handle;
    other.handle = nullptr;
    other.location = "";
}

libloader::library& libloader::library::operator=(libloader::library&& other) noexcept {
    if (this != &other) {
        close();
        location = other.location;
        handle = other.handle;
        other.handle = nullptr;
        other.location = "";
    }
    return *this;
}

void libloader::loadFolder(std::list<libloader::library>& list, const std::string& path) {
    std::filesystem::path fp = path;
    loadFolder(list, fp);
}

std::list<libloader::library> libloader::loadFolder(const std::string& path) {
    std::filesystem::path fp = path;
    return loadFolder(fp);
}

void libloader::loadFolder(std::list<libloader::library>& list, const std::function<bool(libloader::library&)>& consumer, const std::string& path) {
    std::filesystem::path fp = path;
    loadFolder(list, consumer, fp);
}

std::list<libloader::library> libloader::loadFolder(const std::filesystem::path& path) {
    std::list<libloader::library> libs;
    loadFolder(libs, path);
    return libs;
}

void libloader::loadFolder(std::list<libloader::library>& libs, const std::filesystem::path& path) {
    loadFolder(libs, nullptr, path);
}

void libloader::loadFolder(std::list<libloader::library>& libs, const std::function<bool(libloader::library&)>& consumer, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        return;
    }
    std::list<std::string> files;
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::exists(dir_entry) || !std::filesystem::is_regular_file(dir_entry)) {
            continue;
        }
        std::string name = dir_entry.path().string();
        for (const auto& l : libs) {
            if (l.getLocation() == name) {
                goto dirLoop;
            }
        }
        if (!libs.emplace_front(name).isLoaded()) {
            libs.pop_front();
            files.emplace_front(name);
        } else if (consumer && consumer(libs.front())) {
            libs.pop_front();
        }
        dirLoop:;
    }
    size_t length = 0;
    while (length != libs.size()) {
        length = libs.size();
        files.remove_if([&](const std::string& file) {
            if (!libs.emplace_front(file).isLoaded()) {
                libs.pop_front();
                return false;
            } else if (consumer && consumer(libs.front())) {
                libs.pop_front();
            }
            return true;
        });
    }
}

std::set<std::string> libloader::getLoadedLibraries() {
    std::set<std::string> ret;
#ifdef WIN32
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 module_entry;
        module_entry.dwSize = sizeof(module_entry);
        if (Module32First(snapshot, &module_entry)) {
            do {
                ret.insert(module_entry.szExePath);
                module_entry.dwSize = sizeof(module_entry);
            } while (Module32Next(snapshot, &module_entry));
        }
        CloseHandle(snapshot);
    }
#endif
    return ret;
}

libloader::library libloader::createFakeLibrary(const std::string& path) {
    return libloader::library("^" + path);
}
