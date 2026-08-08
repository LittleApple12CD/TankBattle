// src/mod/ModLoader.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "IMod.h"
#include "ModInfo.h"

class Game;

class ModLoader {
public:
    static ModLoader& getInstance();

    ModLoader(const ModLoader&) = delete;
    ModLoader& operator=(const ModLoader&) = delete;

    void init();
    void onGameLoad(Game* game);
    void onGameUpdate(float dt);
    void unloadAll();

    const std::vector<ModInfo>& getMods() const { return mods; }
    const std::vector<IMod*>& getLoadedMods() const { return loadedMods; }
    void toggleMod(int index);
    void reloadAll();

    // 版本号相关
    std::string getGameVersion() const { return gameVersion; }
    void setGameVersion(const std::string& version) { 
        gameVersion = version; 
        saveGameVersion();
    }
    void loadGameVersion();
    void saveGameVersion();

private:
    ModLoader();
    ~ModLoader();

    void scanMods();
    bool loadMod(const std::string& filePath);
    void unloadMod(int index);
    void loadEnabledList();
    void saveEnabledList();
    bool isCompatible(const std::string& required);

#ifdef _WIN32
    using ModuleHandle = void*;
#else
    using ModuleHandle = void*;
#endif

    struct LoadedModule {
        ModuleHandle handle;
        IMod* instance;
        CreateModFunc createFunc;
        DestroyModFunc destroyFunc;
        std::string filePath;
    };

    std::vector<ModInfo> mods;
    std::vector<IMod*> loadedMods;
    std::vector<LoadedModule> loadedModules;
    std::string modsDir;
    std::string listFile;
    std::string gameVersion;
    std::string versionFile;
    std::vector<std::string> enabledMods;
    bool initialized;
};