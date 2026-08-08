// src/mod/ModInfo.h
#pragma once
#include <string>

struct ModInfo {
    std::string name;
    std::string version;
    std::string gameVersion;
    std::string filePath;
    bool loaded;
    bool disabled;
    std::string error;

    ModInfo() : loaded(false), disabled(false) {}

    ModInfo(const std::string& n, const std::string& v,
            const std::string& gv, const std::string& path)
        : name(n), version(v), gameVersion(gv), filePath(path),
          loaded(false), disabled(false) {}

    bool isLoaded() const { return loaded; }
    bool isDisabled() const { return disabled; }
    bool hasError() const { return !error.empty(); }
};