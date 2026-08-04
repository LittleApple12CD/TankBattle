#include "SoundManager.h"
#include "Utils.h"
#include <iostream>
#include <filesystem>

SoundManager::SoundManager() {
    volume = SOUND_VOLUME;
    muted = SOUND_MUTED;
    enabled = SOUND_ENABLED;
}

SoundManager::~SoundManager() {}

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::loadSounds() {
    if (loaded) return;
    
    for (int i = 0; i < SOUND_COUNT; ++i) {
        std::string path = SOUND_PATH + SOUND_FILES[i];
        loadSound(SOUND_NAMES[i], path);
    }
    
    loaded = true;
    std::cout << "🎵 声音系统初始化成功" << std::endl;
}

void SoundManager::loadSound(const std::string& name, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "⚠️ 音效文件不存在: " << filename << std::endl;
        return;
    }
    
    // 存储 buffer
    buffers[name] = std::move(buffer);
    
    // SFML 3.0: Sound 必须用 buffer 构造（无默认构造函数）
    auto sound = std::make_unique<sf::Sound>(buffers[name]);
    sound->setVolume(muted ? 0 : volume * 100);
    
    sounds[name] = std::move(sound);
}

void SoundManager::play(const std::string& name) {
    if (!loaded || muted || !enabled) {
        return;
    }
    
    auto it = sounds.find(name);
    if (it != sounds.end() && it->second) {
        it->second->stop();
        it->second->play();
    }
}

void SoundManager::setVolume(float volume) {
    this->volume = std::max(0.0f, std::min(1.0f, volume));
    SOUND_VOLUME = this->volume;
    float vol = muted ? 0 : this->volume * 100;
    for (auto& [name, sound] : sounds) {
        if (sound) {
            sound->setVolume(vol);
        }
    }
}

void SoundManager::toggleMute() {
    muted = !muted;
    SOUND_MUTED = muted;
    float vol = muted ? 0 : volume * 100;
    for (auto& [name, sound] : sounds) {
        if (sound) {
            sound->setVolume(vol);
        }
    }
}

void SoundManager::setEnabled(bool enabled) {
    this->enabled = enabled;
    SOUND_ENABLED = enabled;
    if (!enabled) {
        for (auto& [name, sound] : sounds) {
            if (sound) {
                sound->stop();
            }
        }
    }
}