//
// Created by Elec332 on 18/10/2021.
//

#include <filesystem>
#include <module/libloader.h>

#ifdef _WIN32

#include <Windows.h>

#else
#include <dlfcn.h>
#endif

libloader::library::library(const std::string& path) : location(path) {
#ifdef _WIN32
    this->handle = LoadLibrary(path.c_str());
#else
    //todo
#endif
}

void libloader::library::close() {
    if (!handle) {
        return;
    }
#ifdef _WIN32
    FreeLibrary((HMODULE) handle);
#else
    //todo
#endif
    handle = nullptr;
}

#ifdef _WIN32

void* libloader::library::getSymbol(const std::string& name) const {
#ifdef _WIN32
    return (void*) GetProcAddress((HMODULE) this->handle, name.c_str());
#else
    //todo
#endif
}

#else
void *libloader::library::getSymbol(const std::string &name) const {
    //todo
}
#endif

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

void libloader::loadFolder(std::list<libloader::library>& libs, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        return;
    }
    std::list<std::string> files;
    for (auto const& dir_entry: std::filesystem::directory_iterator(path)) {
        if (!std::filesystem::exists(dir_entry) || !std::filesystem::is_regular_file(dir_entry)) {
            continue;
        }
        std::string name = dir_entry.path().string();
        if (!libs.emplace_front(name).isLoaded()) {
            libs.pop_front();
            files.emplace_front(name);
        }
    }
    size_t length = 0;
    while (length != libs.size()) {
        length = libs.size();
        files.remove_if([&](const std::string& file) {
            if (!libs.emplace_front(file).isLoaded()) {
                libs.pop_front();
                return false;
            }
            return true;
        });
    }
}

std::list<libloader::library> libloader::loadFolder(const std::filesystem::path& path) {
    std::list<libloader::library> libs;
    loadFolder(libs, path);
    return libs;
}
