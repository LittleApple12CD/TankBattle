// src/resource/ResourcePackLoader.cpp
#include "ResourcePackLoader.h"
#include "TextureManager.h"
#include "MapLoader.h"
#include "../Game.h"
#include "../MapGenerator.h"
#include "../Wall.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <zip.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;
using json = nlohmann::json;

ResourcePackLoader& ResourcePackLoader::getInstance() {
    static ResourcePackLoader instance;
    return instance;
}

void ResourcePackLoader::init(Game* game) {
    if (!fs::exists(resourcePacksDir)) {
        fs::create_directories(resourcePacksDir);
        std::cout << "创建 resourcepacks 目录" << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(resourcePacksDir)) {
        if (!entry.is_regular_file() && !entry.is_directory()) continue;

        std::string path = entry.path().string();

        if (entry.is_directory()) {
            loadResourcePack(path, game);
        } else if (entry.path().extension() == ".zip") {
            loadZipPack(path, game);
        }
    }

    std::cout << "加载了 " << loadedPacks.size() << " 个资源包" << std::endl;
}

void ResourcePackLoader::loadResourcePack(const std::string& path, Game* game) {
    fs::path packPath(path);
    std::string packJsonPath = path + "/pack.json";

    if (!fs::exists(packJsonPath)) return;

    std::ifstream file(packJsonPath);
    if (!file.is_open()) return;

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    PackInfo info;
    if (!parsePackJson(content, info)) return;

    info.path = path;
    info.isZip = false;

    std::cout << "加载资源包: " << info.name << " v" << info.version << std::endl;

    std::string imagesDir = path + "/images";
    if (fs::exists(imagesDir)) {
        loadTextures(imagesDir);
    }

    std::string mapsDir = path + "/maps";
    if (fs::exists(mapsDir)) {
        loadMaps(mapsDir, game);
    }

    loadedPacks.push_back(info.name);
    packCache[info.name] = info;
}

// ============================================================
// ZIP 资源包加载（完整实现）
// ============================================================

void ResourcePackLoader::loadZipPack(const std::string& path, Game* game) {
    // 检查文件是否存在
    if (!fs::exists(path)) {
        std::cerr << "ZIP 文件不存在: " << path << std::endl;
        return;
    }

    // 先读取 pack.json 获取信息
    std::string packJsonContent;
    if (!readFileFromZip(path, "pack.json", packJsonContent)) {
        std::cerr << "ZIP 包缺少 pack.json: " << path << std::endl;
        return;
    }

    PackInfo info;
    if (!parsePackJson(packJsonContent, info)) {
        return;
    }

    info.path = path;
    info.isZip = true;

    std::cout << "加载资源包(ZIP): " << info.name << " v" << info.version << std::endl;

    // 加载贴图
    loadTexturesFromZip(path, "images/");

    // 加载地图
    loadMapsFromZip(path, game);

    loadedPacks.push_back(info.name);
    packCache[info.name] = info;
}

// ============================================================
// 从 ZIP 读取文件内容
// ============================================================

bool ResourcePackLoader::readFileFromZip(const std::string& zipPath, 
                                          const std::string& filename, 
                                          std::string& content) {
    int err = 0;
    zip_t* zip = zip_open(zipPath.c_str(), ZIP_RDONLY, &err);
    if (!zip) {
        return false;
    }

    // 查找文件
    zip_stat_t stat;
    if (zip_stat(zip, filename.c_str(), 0, &stat) != 0) {
        zip_close(zip);
        return false;
    }

    // 打开文件
    zip_file_t* file = zip_fopen(zip, filename.c_str(), 0);
    if (!file) {
        zip_close(zip);
        return false;
    }

    // 读取内容
    content.resize(stat.size);
    zip_int64_t read = zip_fread(file, content.data(), stat.size);
    zip_fclose(file);
    zip_close(zip);

    return read == static_cast<zip_int64_t>(stat.size);
}

// ============================================================
// 从 ZIP 加载贴图
// ============================================================

void ResourcePackLoader::loadTexturesFromZip(const std::string& zipPath, 
                                              const std::string& prefix) {
    int err = 0;
    zip_t* zip = zip_open(zipPath.c_str(), ZIP_RDONLY, &err);
    if (!zip) {
        std::cerr << "无法打开 ZIP: " << zipPath << std::endl;
        return;
    }

    // 读取 images.json 映射
    std::map<std::string, std::string> mapping;
    std::string mappingContent;
    std::string mappingFile = prefix + "images.json";
    
    if (readFileFromZip(zipPath, mappingFile, mappingContent)) {
        try {
            json j = json::parse(mappingContent);
            if (j.contains("mappings")) {
                for (auto& [key, value] : j["mappings"].items()) {
                    mapping[key] = value.get<std::string>();
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "解析 images.json 失败: " << e.what() << std::endl;
        }
    }

    // 遍历 ZIP 中的 PNG 文件
    zip_int64_t num_entries = zip_get_num_entries(zip, 0);
    auto& tm = TextureManager::getInstance();

    for (zip_int64_t i = 0; i < num_entries; ++i) {
        const char* name = zip_get_name(zip, i, 0);
        if (!name) continue;

        std::string entryName(name);
        
        // 检查是否在 images/ 目录下且是 PNG
        if (entryName.find(prefix) != 0) continue;
        if (entryName.find(".png") == std::string::npos) continue;

        // 获取文件名（不含路径）
        std::string filename = entryName.substr(prefix.length());
        
        // 从 ZIP 读取图片数据
        zip_stat_t stat;
        if (zip_stat(zip, entryName.c_str(), 0, &stat) != 0) continue;

        zip_file_t* file = zip_fopen(zip, entryName.c_str(), 0);
        if (!file) continue;

        // 读取数据到内存
        std::vector<char> data(stat.size);
        zip_fread(file, data.data(), stat.size);
        zip_fclose(file);

        // 从内存加载纹理
        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromMemory(data.data(), data.size())) {
            std::cerr << "加载贴图失败: " << filename << std::endl;
            continue;
        }

        // 注册贴图（去掉 .png 扩展名）
        std::string nameWithoutExt = filename.substr(0, filename.find_last_of('.'));
        tm.registerTexture(nameWithoutExt, texture);

        // 应用映射
        for (const auto& [key, value] : mapping) {
            if (value == filename) {
                tm.registerTexture(key, texture);
            }
        }

        std::cout << "  加载贴图: " << filename << std::endl;
    }

    zip_close(zip);
}

// ============================================================
// 从 ZIP 加载地图
// ============================================================

void ResourcePackLoader::loadMapsFromZip(const std::string& zipPath, Game* game) {
    int err = 0;
    zip_t* zip = zip_open(zipPath.c_str(), ZIP_RDONLY, &err);
    if (!zip) {
        std::cerr << "无法打开 ZIP: " << zipPath << std::endl;
        return;
    }

    auto& walls = game->getWalls();
    zip_int64_t num_entries = zip_get_num_entries(zip, 0);

    for (zip_int64_t i = 0; i < num_entries; ++i) {
        const char* name = zip_get_name(zip, i, 0);
        if (!name) continue;

        std::string entryName(name);
        
        // 检查是否在 maps/ 目录下且是 JSON
        if (entryName.find("maps/") != 0) continue;
        if (entryName.find(".json") == std::string::npos) continue;

        // 读取地图 JSON
        std::string content;
        if (!readFileFromZip(zipPath, entryName, content)) continue;

        // 解析地图
        std::vector<Wall> tempWalls;
        if (MapLoader::parseMapJson(content, tempWalls)) {
            // 合并到游戏墙壁
            for (auto& wall : tempWalls) {
                bool overlap = false;
                for (const auto& existing : walls) {
                    if (existing.isAlive() && 
                        wall.getRect().findIntersection(existing.getRect()).has_value()) {
                        overlap = true;
                        break;
                    }
                }
                if (!overlap) {
                    walls.push_back(wall);
                }
            }
            
            std::string filename = entryName.substr(entryName.find_last_of('/') + 1);
            std::cout << "  加载地图: " << filename << std::endl;
        }
    }

    zip_close(zip);
}

// ============================================================
// 其他现有函数
// ============================================================

bool ResourcePackLoader::parsePackJson(const std::string& jsonStr, PackInfo& info) {
    try {
        json j = json::parse(jsonStr);
        info.name = j.value("name", "");
        info.version = j.value("version", "1.0.0");
        info.type = j.value("type", "ResourcePack");

        if (info.name.empty()) {
            std::cerr << "pack.json 缺少 name 字段" << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "解析 pack.json 失败: " << e.what() << std::endl;
        return false;
    }
}

void ResourcePackLoader::loadTextures(const std::string& imagesDir) {
    std::map<std::string, std::string> mapping;
    std::string mappingFile = imagesDir + "/images.json";

    if (fs::exists(mappingFile)) {
        std::ifstream file(mappingFile);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            file.close();

            try {
                json j = json::parse(content);
                if (j.contains("mappings")) {
                    for (auto& [key, value] : j["mappings"].items()) {
                        mapping[key] = value.get<std::string>();
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "解析 images.json 失败: " << e.what() << std::endl;
            }
        }
    }

    auto& tm = TextureManager::getInstance();

    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".png") continue;

        std::string filename = entry.path().filename().string();
        std::string name = filename.substr(0, filename.find_last_of('.'));

        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(entry.path().string())) {
            std::cerr << "加载贴图失败: " << filename << std::endl;
            continue;
        }

        tm.registerTexture(name, texture);

        for (const auto& [key, value] : mapping) {
            if (value == filename) {
                tm.registerTexture(key, texture);
            }
        }
    }
}

void ResourcePackLoader::loadMaps(const std::string& mapsDir, Game* game) {
    auto& walls = game->getWalls();
    int count = MapLoader::loadAllMaps(mapsDir, walls);
    
    if (count > 0) {
        std::cout << "从资源包加载了 " << count << " 个地图" << std::endl;
    }
}

void ResourcePackLoader::reloadAll(Game* game) {
    loadedPacks.clear();
    packCache.clear();
    TextureManager::getInstance().clear();
    init(game);
}

bool ResourcePackLoader::hasPack(const std::string& name) const {
    return packCache.find(name) != packCache.end();
}