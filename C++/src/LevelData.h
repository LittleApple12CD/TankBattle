#pragma once
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

class LevelData {
public:
    struct EnemyType {
        int hp;
        double speedMult;
        sf::Color color;
        std::string label;

        EnemyType(int h, double s, sf::Color c, const std::string& l)
            : hp(h), speedMult(s), color(c), label(l) {}
    };

    struct BossConfig {
        int hp;
        double sizeMult;
        double speedMult;
        int bulletDamage;
        double bulletSpeedMult;

        BossConfig(int h, double sm, double sp, int bd, double bsm)
            : hp(h), sizeMult(sm), speedMult(sp), bulletDamage(bd), bulletSpeedMult(bsm) {}
    };

    struct LevelConfig {
        std::string enemyType;
        int count;
        double spawnInterval;
        bool isBoss;
        int bossId;

        LevelConfig(const std::string& et, int c, double si, bool ib, int bid)
            : enemyType(et), count(c), spawnInterval(si), isBoss(ib), bossId(bid) {}
    };

    static const std::map<std::string, EnemyType> ENEMY_TYPES;
    static const std::map<int, BossConfig> BOSS_TYPES;
    static const std::map<int, LevelConfig> LEVELS;
    static const int MAX_LEVEL;
    static const int MAP_ROTATION[5];

    static const LevelConfig* getLevelConfig(int level);
    static int getMapId(int level);
    static bool isBossLevel(int level);
    static const BossConfig* getBossConfig(int level);
};