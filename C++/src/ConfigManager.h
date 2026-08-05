#pragma once
#include <string>
#include <map>
#include <SFML/Window/Keyboard.hpp>
#include "json.hpp"

using json = nlohmann::json;

class ConfigManager {
public:
    static ConfigManager& getInstance();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void loadAll();
    void saveSettings();
    void saveConfig();

    // ===== Settings (settings.json) =====
    int getWindowWidth() const;
    int getWindowHeight() const;
    void setWindowSize(int w, int h);
    float getVolume() const;
    void setVolume(float v);
    bool isMuted() const;
    void setMuted(bool m);
    std::string getKey(int player, const std::string& action) const;
    void setKey(int player, const std::string& action, const std::string& key);

    // ===== Config (config.ini) =====
    int getFPS() const;
    int getTankSize() const;
    int getTankSpeed() const;
    int getPlayerLives() const;
    float getShotCooldown() const;
    int getBulletSize() const;
    float getBulletSpeed() const;
    int getMaxBullets() const;
    int getEnemyCount() const;
    float getEnemySpawnInterval() const;
    float getAIDirectionChange() const;
    float getAIShootChance() const;
    float getPowerUpSpawnInterval() const;
    int getMaxPowerups() const;

    void resetToDefault();

    // 辅助：字符串转 sf::Keyboard::Key
    static sf::Keyboard::Key stringToKey(const std::string& str);
    static std::string keyToString(sf::Keyboard::Key key);

private:
    ConfigManager();
    ~ConfigManager() = default;

    void loadSettings();
    void loadConfig();
    void createDefaultSettings();
    void createDefaultConfig();

    json settings;
    std::map<std::string, std::string> config;
    std::string settingsPath;
    std::string configPath;
};