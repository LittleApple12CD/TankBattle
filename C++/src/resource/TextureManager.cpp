// src/resource/TextureManager.cpp
#include "TextureManager.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::registerTexture(const std::string& entityId,
                                      std::shared_ptr<sf::Texture> texture) {
    if (texture) {
        textures[entityId] = texture;
    }
}

bool TextureManager::registerTexture(const std::string& entityId,
                                      const std::string& filePath) {
    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile(filePath)) {
        std::cerr << "加载贴图失败: " << filePath << std::endl;
        return false;
    }
    textures[entityId] = texture;
    return true;
}

std::shared_ptr<sf::Texture> TextureManager::getTexture(const std::string& entityId) const {
    auto it = textures.find(entityId);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

bool TextureManager::hasTexture(const std::string& entityId) const {
    return textures.find(entityId) != textures.end();
}

std::shared_ptr<sf::Texture> TextureManager::getEntityTexture(const std::string& entityId) const {
    return getTexture(entityId);
}

void TextureManager::clear() {
    textures.clear();
}

std::vector<std::string> TextureManager::getTextureIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : textures) {
        ids.push_back(pair.first);
    }
    return ids;
}