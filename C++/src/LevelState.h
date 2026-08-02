#pragma once
#include <string>

class LevelState {
public:
    enum class State {
        IDLE,
        SPAWNING,
        WAITING,
        CLEARED,
        GAMEOVER,
        BOSS_SPAWNING,
        BOSS_FIGHT,
        VICTORY
    };

    class Controller {
    public:
        Controller();

        void startLevel(int level);
        std::string update(double dt, int enemiesAlive);
        void onBossSpawned();

        State getState() const { return state; }
        int getCurrentLevel() const { return currentLevel; }
        int getEnemiesTotal() const { return enemiesTotal; }
        bool isBossLevel() const { return isBossLevelFlag; }
        bool isBossSpawned() const { return bossSpawned; }
        bool isWaitingForEnter() const {
            return state == State::CLEARED || state == State::VICTORY;
        }
        bool isGameOver() const { return state == State::GAMEOVER; }
        void setState(State s) { state = s; }
        void setWaitTimer(double time) { waitTimer = time; }

    private:
        int currentLevel;
        State state;
        int enemiesSpawned;
        int enemiesTotal;
        std::string enemyType;
        double spawnTimer;
        double spawnInterval;
        bool isBossLevelFlag;  // 改名
        int bossId;
        bool bossSpawned;
        double waitTimer;
        const double waitDuration = 2.0;
    };
};