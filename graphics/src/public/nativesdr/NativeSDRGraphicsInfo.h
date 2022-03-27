//
// Created by Elec332 on 18/03/2022.
//

#ifndef NATIVESDR_NATIVESDRGRAPHICSINFO_H
#define NATIVESDR_NATIVESDRGRAPHICSINFO_H

#include <nativesdr/graphics_export.h>

class GRAPHICS_EXPORT NativeGraphicsInfo {

public:

    [[nodiscard]] virtual std::string getAPI() const = 0;

    [[nodiscard]] virtual int getMajorVersion() const = 0;

    [[nodiscard]] virtual int getMinorVersion() const = 0;

    [[nodiscard]] virtual const std::set<std::string> getExtensions() const = 0;

    [[nodiscard]] virtual bool hasExtension(const std::string& name) const = 0;

    [[nodiscard]] virtual const std::set<std::string> getShaderVersions() const = 0;

    [[nodiscard]] virtual bool hasShaderVersion(const std::string& version) const = 0;

};


class GRAPHICS_EXPORT SubContext {

public:

    virtual void runFrame(const std::function<void()>& func) = 0;

};

#endif //NATIVESDR_NATIVESDRGRAPHICSINFO_H
