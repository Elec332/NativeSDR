//
// Created by Elec332 on 17/10/2021.
//

#ifndef NATIVESDR2_LIBLOADER_H
#define NATIVESDR2_LIBLOADER_H

#include <string>
#include <functional>
#include <list>
#include <set>
#include <stdexcept>

#if defined(_FILESYSTEM_) || defined(_GLIBCXX_FILESYSTEM)
#define LL_FILESYSTEM
#endif

#ifndef LL_EXPORT
#define LL_EXPORT
#endif

namespace libloader {

    class loading_exception : public std::runtime_error {
    public:
        explicit loading_exception(const char* msg) : std::runtime_error(msg) {
        }
    };

    class LL_EXPORT library {

    public:

#ifdef LL_FILESYSTEM

        explicit library(std::filesystem::path path) : library(path.string()) {
        }

#endif

        explicit library(const std::string& path);

        ~library();

        library(const libloader::library& a) = delete; //No copies allowed

        library& operator=(const libloader::library& a) = delete; //No copies allowed

        library(libloader::library&& other) noexcept;

        library& operator=(libloader::library&& other) noexcept;

        [[nodiscard]] bool isLoaded() const;

        void close();

        [[nodiscard]] bool hasSymbol(const std::string& name) const;

        template<typename T>
        [[nodiscard]] T getObject(const std::string& name) const {
            auto addr = getSymbol(name);
            if (!addr) {
                throw loading_exception("No such symbol!");
            }
            return *reinterpret_cast<T*>(addr);
        }

        template<class T>
        [[nodiscard]] std::function<T> getFunction(const std::string& name) const {
            auto addr = getSymbol(name);
            if (!addr) {
                throw loading_exception("No such symbol!");
            }
            return *reinterpret_cast<std::function<T>*>(addr);
        }

        template<typename T>
        [[nodiscard]] T* getReference(const std::string& name) const {
            return reinterpret_cast<T*>(getSymbol(name));
        }

        [[nodiscard]] void* getSymbol(const std::string& name) const;

        [[nodiscard]] std::string getLocation() const;

    private:

        std::string location;

        void* handle = nullptr;

    };

    LL_EXPORT void loadFolder(std::list<libloader::library>& list, const std::function<bool(libloader::library&)>& consumer, const std::string& path);

    LL_EXPORT void loadFolder(std::list<libloader::library>& list, const std::string& path);

    LL_EXPORT std::list<libloader::library> loadFolder(const std::string& path);

    LL_EXPORT libloader::library createFakeLibrary(const std::string& path);

    LL_EXPORT std::set<std::string> getLoadedLibraries();

#ifdef LL_FILESYSTEM

    LL_EXPORT void loadFolder(std::list<libloader::library>& list, const std::function<bool(libloader::library&)>& consumer, const std::filesystem::path& path);

    LL_EXPORT void loadFolder(std::list<libloader::library>& list, const std::filesystem::path& path);

    LL_EXPORT std::list<libloader::library> loadFolder(const std::filesystem::path& path);

#endif

}

#endif //NATIVESDR2_LIBLOADER_H
