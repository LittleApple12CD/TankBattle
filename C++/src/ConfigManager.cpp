#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>

// ============================================
// 单例
// ============================================
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager()
    : settingsPath("settings.json"), configPath("config.ini") {
    loadAll();
}

// ============================================
// 加载全部
// ============================================
void ConfigManager::loadAll() {
    loadSettings();
    loadConfig();
}

// ============================================
// Settings (JSON)
// ============================================
void ConfigManager::loadSettings() {
    std::ifstream file(settingsPath);
    if (!file.is_open()) {
        createDefaultSettings();
        return;
    }

    try {
        file >> settings;
        file.close();
    } catch (...) {
        std::cerr << "settings.json 损坏，使用默认设置" << std::endl;
        createDefaultSettings();
    }
}

void ConfigManager::createDefaultSettings() {
    settings = {
        {"window", {{"width", 1600}, {"height", 900}, {"fullscreen", false}}},
        {"audio", {{"volume", 0.8f}, {"muted", false}}},
        {"keys", {
            {"player1", {
                {"up", "Up"},
                {"down", "Down"},
                {"left", "Left"},
                {"right", "Right"},
                {"shoot", "Space"}
            }},
            {"player2", {
                {"up", "W"},
                {"down", "S"},
                {"left", "A"},
                {"right", "D"},
                {"shoot", "J"}
            }}
        }}
    };
    saveSettings();
}

void ConfigManager::saveSettings() {
    std::ofstream file(settingsPath);
    if (file.is_open()) {
        file << settings.dump(4);
        file.close();
    }
}

// ============================================
// Config (INI)
// ============================================
void ConfigManager::loadConfig() {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        createDefaultConfig();
        return;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        // 去掉首尾空格
        while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) line.erase(0, 1);
        while (!line.empty() && (line.back() == ' ' || line.back() == '\t' || line.back() == '\r')) line.pop_back();

        if (line.empty()) continue;
        if (line.front() == ';' || line.front() == '#') continue;

        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.length() - 2);
            continue;
        }

        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            while (!key.empty() && key.back() == ' ') key.pop_back();
            while (!value.empty() && value.front() == ' ') value.erase(0, 1);
            config[section + "." + key] = value;
        }
    }
    file.close();
}

void ConfigManager::createDefaultConfig() {
    config = {
        {"Game.FPS", "60"},
        {"Tank.Size", "36"},
        {"Tank.Speed", "7"},
        {"Tank.PlayerLives", "3"},
        {"Tank.ShotCooldown", "0.5"},
        {"Bullet.Size", "8"},
        {"Bullet.Speed", "520"},
        {"Bullet.MaxBullets", "3"},
        {"Enemy.Count", "4"},
        {"Enemy.SpawnInterval", "4.0"},
        {"Enemy.AIDirectionChange", "2.0"},
        {"Enemy.AIShootChance", "0.4"},
        {"PowerUp.SpawnInterval", "10.0"},
        {"PowerUp.MaxPowerups", "3"}
    };
    saveConfig();
}

void ConfigManager::saveConfig() {
    std::ofstream file(configPath);
    if (!file.is_open()) return;

    file << "; ============================================\n";
    file << ";  坦克大战 - 游戏配置文件\n";
    file << ";  修改后重启游戏生效\n";
    file << "; ============================================\n\n";

    std::map<std::string, std::map<std::string, std::string>> sections;
    for (auto& [key, value] : config) {
        size_t dot = key.find('.');
        if (dot != std::string::npos) {
            sections[key.substr(0, dot)][key.substr(dot + 1)] = value;
        }
    }

    for (auto& [section, values] : sections) {
        file << "[" << section << "]\n";
        for (auto& [key, value] : values) {
            file << key << " = " << value << "\n";
        }
        file << "\n";
    }
    file.close();
}

// ============================================
// Settings Getters
// ============================================
int ConfigManager::getWindowWidth() const {
    return settings["window"]["width"].get<int>();
}
int ConfigManager::getWindowHeight() const {
    return settings["window"]["height"].get<int>();
}
void ConfigManager::setWindowSize(int w, int h) {
    settings["window"]["width"] = w;
    settings["window"]["height"] = h;
    saveSettings();
}
float ConfigManager::getVolume() const {
    return settings["audio"]["volume"].get<float>();
}
void ConfigManager::setVolume(float v) {
    settings["audio"]["volume"] = std::max(0.0f, std::min(1.0f, v));
    saveSettings();
}
bool ConfigManager::isMuted() const {
    return settings["audio"]["muted"].get<bool>();
}
void ConfigManager::setMuted(bool m) {
    settings["audio"]["muted"] = m;
    saveSettings();
}

std::string ConfigManager::getKey(int player, const std::string& action) const {
    std::string key = "player" + std::to_string(player);
    return settings["keys"][key][action].get<std::string>();
}

void ConfigManager::setKey(int player, const std::string& action, const std::string& key) {
    std::string pkey = "player" + std::to_string(player);
    settings["keys"][pkey][action] = key;
    saveSettings();
}

// ============================================
// Config Getters
// ============================================
int ConfigManager::getFPS() const {
    auto it = config.find("Game.FPS");
    return it != config.end() ? std::stoi(it->second) : 60;
}
int ConfigManager::getTankSize() const {
    auto it = config.find("Tank.Size");
    return it != config.end() ? std::stoi(it->second) : 36;
}
int ConfigManager::getTankSpeed() const {
    auto it = config.find("Tank.Speed");
    return it != config.end() ? std::stoi(it->second) : 7;
}
int ConfigManager::getPlayerLives() const {
    auto it = config.find("Tank.PlayerLives");
    return it != config.end() ? std::stoi(it->second) : 3;
}
float ConfigManager::getShotCooldown() const {
    auto it = config.find("Tank.ShotCooldown");
    return it != config.end() ? std::stof(it->second) : 0.5f;
}
int ConfigManager::getBulletSize() const {
    auto it = config.find("Bullet.Size");
    return it != config.end() ? std::stoi(it->second) : 8;
}
float ConfigManager::getBulletSpeed() const {
    auto it = config.find("Bullet.Speed");
    return it != config.end() ? std::stof(it->second) : 520.0f;
}
int ConfigManager::getMaxBullets() const {
    auto it = config.find("Bullet.MaxBullets");
    return it != config.end() ? std::stoi(it->second) : 3;
}
int ConfigManager::getEnemyCount() const {
    auto it = config.find("Enemy.Count");
    return it != config.end() ? std::stoi(it->second) : 4;
}
float ConfigManager::getEnemySpawnInterval() const {
    auto it = config.find("Enemy.SpawnInterval");
    return it != config.end() ? std::stof(it->second) : 4.0f;
}
float ConfigManager::getAIDirectionChange() const {
    auto it = config.find("Enemy.AIDirectionChange");
    return it != config.end() ? std::stof(it->second) : 2.0f;
}
float ConfigManager::getAIShootChance() const {
    auto it = config.find("Enemy.AIShootChance");
    return it != config.end() ? std::stof(it->second) : 0.4f;
}
float ConfigManager::getPowerUpSpawnInterval() const {
    auto it = config.find("PowerUp.SpawnInterval");
    return it != config.end() ? std::stof(it->second) : 10.0f;
}
int ConfigManager::getMaxPowerups() const {
    auto it = config.find("PowerUp.MaxPowerups");
    return it != config.end() ? std::stoi(it->second) : 3;
}

// ============================================
// 重置默认
// ============================================
void ConfigManager::resetToDefault() {
    createDefaultSettings();
    createDefaultConfig();
    loadAll();
}

// ============================================
// 键位转换辅助函数
// ============================================
sf::Keyboard::Key ConfigManager::stringToKey(const std::string& str) {
    if (str == "Space") return sf::Keyboard::Key::Space;
    if (str == "Up") return sf::Keyboard::Key::Up;
    if (str == "Down") return sf::Keyboard::Key::Down;
    if (str == "Left") return sf::Keyboard::Key::Left;
    if (str == "Right") return sf::Keyboard::Key::Right;
    if (str == "Escape") return sf::Keyboard::Key::Escape;
    if (str == "Enter") return sf::Keyboard::Key::Enter;
    if (str == "LControl") return sf::Keyboard::Key::LControl;
    if (str == "LShift") return sf::Keyboard::Key::LShift;
    if (str == "A") return sf::Keyboard::Key::A;
    if (str == "B") return sf::Keyboard::Key::B;
    if (str == "C") return sf::Keyboard::Key::C;
    if (str == "D") return sf::Keyboard::Key::D;
    if (str == "E") return sf::Keyboard::Key::E;
    if (str == "F") return sf::Keyboard::Key::F;
    if (str == "G") return sf::Keyboard::Key::G;
    if (str == "H") return sf::Keyboard::Key::H;
    if (str == "I") return sf::Keyboard::Key::I;
    if (str == "J") return sf::Keyboard::Key::J;
    if (str == "K") return sf::Keyboard::Key::K;
    if (str == "L") return sf::Keyboard::Key::L;
    if (str == "M") return sf::Keyboard::Key::M;
    if (str == "N") return sf::Keyboard::Key::N;
    if (str == "O") return sf::Keyboard::Key::O;
    if (str == "P") return sf::Keyboard::Key::P;
    if (str == "Q") return sf::Keyboard::Key::Q;
    if (str == "R") return sf::Keyboard::Key::R;
    if (str == "S") return sf::Keyboard::Key::S;
    if (str == "T") return sf::Keyboard::Key::T;
    if (str == "U") return sf::Keyboard::Key::U;
    if (str == "V") return sf::Keyboard::Key::V;
    if (str == "W") return sf::Keyboard::Key::W;
    if (str == "X") return sf::Keyboard::Key::X;
    if (str == "Y") return sf::Keyboard::Key::Y;
    if (str == "Z") return sf::Keyboard::Key::Z;
    if (str == "Num0") return sf::Keyboard::Key::Num0;
    if (str == "Num1") return sf::Keyboard::Key::Num1;
    if (str == "Num2") return sf::Keyboard::Key::Num2;
    if (str == "Num3") return sf::Keyboard::Key::Num3;
    if (str == "Num4") return sf::Keyboard::Key::Num4;
    if (str == "Num5") return sf::Keyboard::Key::Num5;
    if (str == "Num6") return sf::Keyboard::Key::Num6;
    if (str == "Num7") return sf::Keyboard::Key::Num7;
    if (str == "Num8") return sf::Keyboard::Key::Num8;
    if (str == "Num9") return sf::Keyboard::Key::Num9;
    return sf::Keyboard::Key::Unknown;
}

std::string ConfigManager::keyToString(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Space: return "Space";
        case sf::Keyboard::Key::Up: return "Up";
        case sf::Keyboard::Key::Down: return "Down";
        case sf::Keyboard::Key::Left: return "Left";
        case sf::Keyboard::Key::Right: return "Right";
        case sf::Keyboard::Key::Escape: return "Escape";
        case sf::Keyboard::Key::Enter: return "Enter";
        case sf::Keyboard::Key::LControl: return "LControl";
        case sf::Keyboard::Key::LShift: return "LShift";
        case sf::Keyboard::Key::A: return "A";
        case sf::Keyboard::Key::B: return "B";
        case sf::Keyboard::Key::C: return "C";
        case sf::Keyboard::Key::D: return "D";
        case sf::Keyboard::Key::E: return "E";
        case sf::Keyboard::Key::F: return "F";
        case sf::Keyboard::Key::G: return "G";
        case sf::Keyboard::Key::H: return "H";
        case sf::Keyboard::Key::I: return "I";
        case sf::Keyboard::Key::J: return "J";
        case sf::Keyboard::Key::K: return "K";
        case sf::Keyboard::Key::L: return "L";
        case sf::Keyboard::Key::M: return "M";
        case sf::Keyboard::Key::N: return "N";
        case sf::Keyboard::Key::O: return "O";
        case sf::Keyboard::Key::P: return "P";
        case sf::Keyboard::Key::Q: return "Q";
        case sf::Keyboard::Key::R: return "R";
        case sf::Keyboard::Key::S: return "S";
        case sf::Keyboard::Key::T: return "T";
        case sf::Keyboard::Key::U: return "U";
        case sf::Keyboard::Key::V: return "V";
        case sf::Keyboard::Key::W: return "W";
        case sf::Keyboard::Key::X: return "X";
        case sf::Keyboard::Key::Y: return "Y";
        case sf::Keyboard::Key::Z: return "Z";
        case sf::Keyboard::Key::Num0: return "Num0";
        case sf::Keyboard::Key::Num1: return "Num1";
        case sf::Keyboard::Key::Num2: return "Num2";
        case sf::Keyboard::Key::Num3: return "Num3";
        case sf::Keyboard::Key::Num4: return "Num4";
        case sf::Keyboard::Key::Num5: return "Num5";
        case sf::Keyboard::Key::Num6: return "Num6";
        case sf::Keyboard::Key::Num7: return "Num7";
        case sf::Keyboard::Key::Num8: return "Num8";
        case sf::Keyboard::Key::Num9: return "Num9";
        default: return "Unknown";
    }
}