// src/mod/ModLoader.cpp
#include "ModLoader.h"
#include "../Game.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

ModLoader& ModLoader::getInstance() {
    static ModLoader instance;
    return instance;
}

ModLoader::ModLoader()
    : modsDir("mods"), listFile("mods/mod_list.txt"),
      versionFile("game_version.txt"), gameVersion("1.0.0"), initialized(false) {
    loadGameVersion();
}

ModLoader::~ModLoader() {
    unloadAll();
}

void ModLoader::loadGameVersion() {
    std::ifstream file(versionFile);
    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)) {
            while (!line.empty() && (line.back() == ' ' || line.back() == '\t' || line.back() == '\r')) {
                line.pop_back();
            }
            while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
                line.erase(0, 1);
            }
            if (!line.empty()) {
                gameVersion = line;
                std::cout << "游戏版本: " << gameVersion << " (从 " << versionFile << ")" << std::endl;
            }
        }
        file.close();
    } else {
        saveGameVersion();
        std::cout << "创建游戏版本文件: " << versionFile << std::endl;
    }
}

void ModLoader::saveGameVersion() {
    std::ofstream file(versionFile);
    if (file.is_open()) {
        file << gameVersion << "\n";
        file.close();
        std::cout << "保存游戏版本: " << gameVersion << " -> " << versionFile << std::endl;
    } else {
        std::cerr << "保存游戏版本失败: " << versionFile << std::endl;
    }
}

void ModLoader::init() {
    if (initialized) return;

    if (gameVersion.empty()) {
        loadGameVersion();
    }

    if (!fs::exists(modsDir)) {
        fs::create_directories(modsDir);
        std::cout << "创建 mods 目录" << std::endl;
    }

    if (!fs::exists(listFile)) {
        std::ofstream file(listFile);
        if (file.is_open()) {
            file << "# Mod list - one mod DLL/SO file per line\n";
            file << "# Add # before filename to disable\n";
            file << "# Example:\n";
            file << "# MyMod.dll\n";
            file << "# #DisabledMod.dll\n";
            file.close();
            std::cout << "创建模组列表文件: " << listFile << std::endl;
        }
    }

    loadEnabledList();
    scanMods();
    initialized = true;
    std::cout << "模组系统初始化完成 (游戏版本: " << gameVersion << ")" << std::endl;
}

void ModLoader::loadEnabledList() {
    enabledMods.clear();
    std::ifstream file(listFile);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);

        if (line.empty() || line[0] == '#') continue;
        enabledMods.push_back(line);
    }
    file.close();
}

void ModLoader::saveEnabledList() {
    std::ofstream file(listFile);
    if (!file.is_open()) return;

    file << "# Mod list - one mod DLL/SO file per line\n";
    file << "# Add # before filename to disable\n\n";

    for (const auto& mod : mods) {
        if (mod.disabled) {
            file << "# " << mod.filePath << "\n";
        } else {
            file << mod.filePath << "\n";
        }
    }
    file.close();
}

void ModLoader::scanMods() {
    mods.clear();

    for (const auto& entry : fs::directory_iterator(modsDir)) {
        if (!entry.is_regular_file()) continue;

        std::string path = entry.path().string();
        std::string filename = entry.path().filename().string();

#ifdef _WIN32
        if (filename.find(".dll") == std::string::npos) continue;
#else
        if (filename.find(".so") == std::string::npos) continue;
#endif

        bool isEnabled = std::find(enabledMods.begin(), enabledMods.end(), filename) != enabledMods.end();

        if (!isEnabled) {
            ModInfo info(filename, "?", "?", filename);
            info.disabled = true;
            mods.push_back(info);
            continue;
        }

        loadMod(path);
    }

    std::cout << "发现 " << mods.size() << " 个模组" << std::endl;
}

bool ModLoader::loadMod(const std::string& filePath) {
    fs::path path(filePath);
    std::string filename = path.filename().string();

#ifdef _WIN32
    HMODULE handle = LoadLibraryA(filePath.c_str());
    if (!handle) {
        ModInfo info(filename, "?", "?", filename);
        info.error = "加载失败: " + std::to_string(GetLastError());
        mods.push_back(info);
        std::cerr << "错误 " << filename << " 加载失败" << std::endl;
        return false;
    }

    auto createFunc = (CreateModFunc)GetProcAddress(handle, "createMod");
    auto destroyFunc = (DestroyModFunc)GetProcAddress(handle, "destroyMod");

    if (!createFunc || !destroyFunc) {
        FreeLibrary(handle);
        ModInfo info(filename, "?", "?", filename);
        info.error = "缺少 createMod/destroyMod 导出函数";
        mods.push_back(info);
        std::cerr << "错误 " << filename << " 缺少导出函数" << std::endl;
        return false;
    }
#else
    void* handle = dlopen(filePath.c_str(), RTLD_LAZY);
    if (!handle) {
        ModInfo info(filename, "?", "?", filename);
        info.error = "加载失败: " + std::string(dlerror());
        mods.push_back(info);
        std::cerr << "错误 " << filename << " 加载失败" << std::endl;
        return false;
    }

    auto createFunc = (CreateModFunc)dlsym(handle, "createMod");
    auto destroyFunc = (DestroyModFunc)dlsym(handle, "destroyMod");

    if (!createFunc || !destroyFunc) {
        dlclose(handle);
        ModInfo info(filename, "?", "?", filename);
        info.error = "缺少 createMod/destroyMod 导出函数";
        mods.push_back(info);
        std::cerr << "错误 " << filename << " 缺少导出函数" << std::endl;
        return false;
    }
#endif

    IMod* instance = createFunc();
    if (!instance) {
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        ModInfo info(filename, "?", "?", filename);
        info.error = "创建模组实例失败";
        mods.push_back(info);
        return false;
    }

    std::string modName = instance->getModName();
    std::string modVersion = instance->getModVersion();
    std::string requiredVersion = instance->getGameVersion();

    if (!isCompatible(requiredVersion)) {
        instance->onUnload();
        destroyFunc(instance);
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        ModInfo info(modName, modVersion, requiredVersion, filename);
        info.error = "需要游戏 v" + requiredVersion + "，当前 v" + gameVersion;
        mods.push_back(info);
        std::cerr << "错误 " << modName << ": " << info.error << std::endl;
        return false;
    }

    ModInfo info(modName, modVersion, requiredVersion, filename);
    info.loaded = true;
    mods.push_back(info);

    LoadedModule mod;
    mod.handle = handle;
    mod.instance = instance;
    mod.createFunc = createFunc;
    mod.destroyFunc = destroyFunc;
    mod.filePath = filename;
    loadedModules.push_back(mod);
    loadedMods.push_back(instance);

    std::cout << "加载模组: " << modName << " v" << modVersion << " (需要游戏 " << requiredVersion << ")" << std::endl;
    return true;
}

void ModLoader::unloadMod(int index) {
    if (index < 0 || index >= (int)loadedModules.size()) return;

    auto& mod = loadedModules[index];
    if (mod.instance) {
        mod.instance->onUnload();
        mod.destroyFunc(mod.instance);
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)mod.handle);
#else
    dlclose(mod.handle);
#endif

    loadedModules.erase(loadedModules.begin() + index);
    loadedMods.erase(loadedMods.begin() + index);
}

void ModLoader::unloadAll() {
    for (int i = (int)loadedModules.size() - 1; i >= 0; --i) {
        unloadMod(i);
    }
    loadedMods.clear();
    loadedModules.clear();
}

void ModLoader::onGameLoad(Game* game) {
    for (auto* mod : loadedMods) {
        try {
            mod->onLoad(game);
            std::cout << "模组初始化完成: " << mod->getModName() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "模组 " << mod->getModName() << " 初始化失败: " << e.what() << std::endl;
        }
    }
}

void ModLoader::onGameUpdate(float dt) {
    for (auto* mod : loadedMods) {
        try {
            mod->onUpdate(dt);
        } catch (const std::exception&) {
        }
    }
}

void ModLoader::toggleMod(int index) {
    if (index < 0 || index >= (int)mods.size()) return;

    auto& info = mods[index];
    info.disabled = !info.disabled;

    if (info.loaded) {
        for (int i = 0; i < (int)loadedModules.size(); ++i) {
            if (loadedModules[i].filePath == info.filePath) {
                unloadMod(i);
                break;
            }
        }
        info.loaded = false;
    }

    saveEnabledList();

    std::cout << "模组切换: " << info.name << " -> "
              << (info.disabled ? "已禁用" : "已启用") << " (重启生效)" << std::endl;
}

void ModLoader::reloadAll() {
    unloadAll();
    mods.clear();
    loadEnabledList();
    scanMods();
}

bool ModLoader::isCompatible(const std::string& required) {
    if (required == "*") return true;
    if (required == gameVersion) return true;

    try {
        std::string req = required;
        std::string ver = gameVersion;

        if (req[0] == 'v' || req[0] == 'V') req = req.substr(1);
        if (ver[0] == 'v' || ver[0] == 'V') ver = ver.substr(1);

        auto split = [](const std::string& s) {
            std::vector<int> parts;
            std::string current;
            for (char c : s) {
                if (c == '.') {
                    if (!current.empty()) {
                        parts.push_back(std::stoi(current));
                        current.clear();
                    }
                } else if (isdigit(c)) {
                    current += c;
                }
            }
            if (!current.empty()) {
                parts.push_back(std::stoi(current));
            }
            return parts;
        };

        std::vector<int> reqParts = split(req);
        std::vector<int> verParts = split(ver);

        size_t minLen = std::min(reqParts.size(), verParts.size());
        for (size_t i = 0; i < minLen; ++i) {
            if (reqParts[i] != verParts[i]) {
                return reqParts[i] < verParts[i];
            }
        }

        return reqParts.size() <= verParts.size();

    } catch (const std::exception&) {
        return required == gameVersion || required == "*";
    }
}