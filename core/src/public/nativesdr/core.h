//
// Created by Elec332 on 10/07/2021.
//

#ifndef NATIVESDR_CORE_H
#define NATIVESDR_CORE_H

#include <nativesdr/core_export.h>

#define LL_EXPORT CORE_EXPORT

#include <filesystem>
#include <module/libloader.h>
#include <thread>
#include <string>

CORE_EXPORT int startCore(int argc, char* argv[]);

CORE_EXPORT std::shared_ptr<libloader::library> loadLibrary(const std::string& path);

CORE_EXPORT std::shared_ptr<libloader::library> loadLibrary(const std::filesystem::path& path);

CORE_EXPORT int testerrr();

#endif //NATIVESDR_CORE_H
