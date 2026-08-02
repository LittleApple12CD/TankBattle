#include "LevelData.h"

const std::map<std::string, LevelData::EnemyType> LevelData::ENEMY_TYPES = {
    {"normal", EnemyType(1, 1.0, sf::Color(200, 50, 50), "Normal")},
    {"tough", EnemyType(2, 1.0, sf::Color(255, 150, 50), "Tough")},
    {"elite", EnemyType(3, 1.5, sf::Color(200, 100, 255), "Elite")}
};

const std::map<int, LevelData::BossConfig> LevelData::BOSS_TYPES = {
    {5, BossConfig(5, 1.5, 1.5, 2, 1.0)},
    {10, BossConfig(10, 2.0, 2.0, 2, 1.5)}
};

const std::map<int, LevelData::LevelConfig> LevelData::LEVELS = {
    {1, LevelConfig("normal", 6, 4.0, false, 0)},
    {2, LevelConfig("normal", 8, 4.0, false, 0)},
    {3, LevelConfig("normal", 10, 4.0, false, 0)},
    {4, LevelConfig("tough", 6, 4.0, false, 0)},
    {5, LevelConfig("", 1, 4.0, true, 5)},
    {6, LevelConfig("tough", 8, 3.0, false, 0)},
    {7, LevelConfig("normal", 10, 4.0, false, 0)},
    {8, LevelConfig("normal", 12, 4.0, false, 0)},
    {9, LevelConfig("elite", 8, 4.0, false, 0)},
    {10, LevelConfig("", 1, 4.0, true, 10)}
};

const int LevelData::MAX_LEVEL = 10;
const int LevelData::MAP_ROTATION[5] = {0, 1, 2, 3, 4};

const LevelData::LevelConfig* LevelData::getLevelConfig(int level) {
    auto it = LEVELS.find(level);
    if (it != LEVELS.end()) {
        return &(it->second);
    }
    return nullptr;
}

int LevelData::getMapId(int level) {
    return MAP_ROTATION[(level - 1) % 5];
}

bool LevelData::isBossLevel(int level) {
    auto it = LEVELS.find(level);
    if (it != LEVELS.end()) {
        return it->second.isBoss;
    }
    return false;
}

const LevelData::BossConfig* LevelData::getBossConfig(int level) {
    auto it = BOSS_TYPES.find(level);
    if (it != BOSS_TYPES.end()) {
        return &(it->second);
    }
    return nullptr;
}