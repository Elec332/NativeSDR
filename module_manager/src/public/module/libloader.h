//
// Created by Elec332 on 17/10/2021.
//

#ifndef NATIVESDR2_LIBLOADER_H
#define NATIVESDR2_LIBLOADER_H

#include <string>
#include <functional>
#include <list>

namespace libloader {

    class loading_exception : public std::runtime_error {
    public:
        explicit loading_exception(const char* msg) : std::runtime_error(msg) {
        }
    };

    class library {

    public:

        explicit library(const std::string& path);

        ~library();

        library(const library&) = delete; //No copies allowed

        library& operator=(const library&) = delete; //No copies allowed

        library(library&& other) noexcept;

        library& operator=(library&& other) noexcept;

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

    std::list<libloader::library> loadFolder(const std::string& path);

#if defined(_FILESYSTEM_) || defined(_GLIBCXX_FILESYSTEM)

    std::list<libloader::library> loadFolder(const std::filesystem::path& path);

#endif

}

#endif //NATIVESDR2_LIBLOADER_H
