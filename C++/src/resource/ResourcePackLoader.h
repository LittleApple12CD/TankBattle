// src/resource/ResourcePackLoader.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

class Game;

/**
 * 资源包加载器
 * 支持文件夹和ZIP格式的资源包
 */
class ResourcePackLoader {
public:
    static ResourcePackLoader& getInstance();

    // 禁止拷贝
    ResourcePackLoader(const ResourcePackLoader&) = delete;
    ResourcePackLoader& operator=(const ResourcePackLoader&) = delete;

    /**
     * 初始化资源包系统
     */
    void init(Game* game);

    /**
     * 重新加载所有资源包
     */
    void reloadAll(Game* game);

    /**
     * 获取已加载的资源包列表
     */
    const std::vector<std::string>& getLoadedPacks() const { return loadedPacks; }

    /**
     * 检查资源包是否存在
     */
    bool hasPack(const std::string& name) const;

private:
    ResourcePackLoader() = default;
    ~ResourcePackLoader() = default;

    struct PackInfo {
        std::string name;
        std::string version;
        std::string type;
        std::string path;
        bool isZip;
    };

    void loadResourcePack(const std::string& path, Game* game);
    void loadZipPack(const std::string& path, Game* game);
    bool parsePackJson(const std::string& json, PackInfo& info);
    void loadTextures(const std::string& imagesDir);
    void loadMaps(const std::string& mapsDir, Game* game);
    
    // ✅ ZIP 内部加载辅助函数
    void loadTexturesFromZip(const std::string& zipPath, const std::string& prefix);
    void loadMapsFromZip(const std::string& zipPath, Game* game);
    bool readFileFromZip(const std::string& zipPath, const std::string& filename, std::string& content);

    std::vector<std::string> loadedPacks;
    std::map<std::string, PackInfo> packCache;
    std::string resourcePacksDir = "resourcepacks";
};