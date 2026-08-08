// src/resource/TextureManager.h
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <memory>

/**
 * 贴图管理器 - 单例模式
 * 管理所有游戏贴图
 */
class TextureManager {
public:
    static TextureManager& getInstance();

    // 禁止拷贝
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    /**
     * 注册贴图
     */
    void registerTexture(const std::string& entityId, std::shared_ptr<sf::Texture> texture);

    /**
     * 注册贴图（从文件加载）
     */
    bool registerTexture(const std::string& entityId, const std::string& filePath);

    /**
     * 获取贴图
     */
    std::shared_ptr<sf::Texture> getTexture(const std::string& entityId) const;

    /**
     * 检查贴图是否存在
     */
    bool hasTexture(const std::string& entityId) const;

    /**
     * 获取实体贴图（别名）
     */
    std::shared_ptr<sf::Texture> getEntityTexture(const std::string& entityId) const;

    /**
     * 清除所有贴图
     */
    void clear();

    /**
     * 获取所有贴图ID
     */
    std::vector<std::string> getTextureIds() const;

private:
    TextureManager() = default;
    ~TextureManager() = default;

    std::map<std::string, std::shared_ptr<sf::Texture>> textures;
};