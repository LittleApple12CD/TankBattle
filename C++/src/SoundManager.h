#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

/**
 * 声音管理器 - 单例模式
 * v1.7 音效系统
 */
class SoundManager {
public:
    static SoundManager& getInstance();

    // 禁止拷贝
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSounds();
    void play(const std::string& name);
    void setVolume(float volume);
    void toggleMute();
    bool isMuted() const { return muted; }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool enabled);
    float getVolume() const { return volume; }

private:
    SoundManager();
    ~SoundManager();

    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, std::unique_ptr<sf::Sound>> sounds;
    
    float volume = 0.8f;
    bool muted = false;
    bool enabled = true;
    bool loaded = false;

    void loadSound(const std::string& name, const std::string& filename);
};