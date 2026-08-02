#include "LevelState.h"
#include "LevelData.h"

LevelState::Controller::Controller()
    : currentLevel(1), state(State::IDLE), enemiesSpawned(0),
      enemiesTotal(0), spawnTimer(0), spawnInterval(4.0),
      isBossLevelFlag(false), bossId(0), bossSpawned(false), waitTimer(0) {}

void LevelState::Controller::startLevel(int level) {
    const LevelData::LevelConfig* config = LevelData::getLevelConfig(level);
    if (!config) return;

    currentLevel = level;
    enemiesSpawned = 0;
    enemiesTotal = config->count;
    enemyType = config->enemyType;
    spawnInterval = config->spawnInterval;
    isBossLevelFlag = config->isBoss;
    bossId = config->bossId;
    bossSpawned = false;
    state = State::SPAWNING;
    spawnTimer = 0;
    waitTimer = 0;
}

std::string LevelState::Controller::update(double dt, int enemiesAlive) {
    switch (state) {
        case State::CLEARED:
            waitTimer -= dt;
            if (waitTimer <= 0) {
                return "next_level";
            }
            return "waiting";

        case State::GAMEOVER:
            return "gameover";

        case State::VICTORY:
            waitTimer -= dt;
            if (waitTimer <= 0) {
                return "victory_done";
            }
            return "waiting";

        case State::SPAWNING:
            if (isBossLevelFlag && bossSpawned) {
                return "boss_spawned";
            }
            if (isBossLevelFlag) {
                spawnTimer -= dt;
                if (spawnTimer <= 0 && !bossSpawned) {
                    state = State::BOSS_SPAWNING;
                    return "spawn_boss";
                }
                return "spawning";
            }
            if (enemiesSpawned < enemiesTotal) {
                spawnTimer -= dt;
                if (spawnTimer <= 0) {
                    spawnTimer = spawnInterval;
                    enemiesSpawned++;
                    return "spawn_enemy";
                }
                return "spawning";
            } else {
                state = State::WAITING;
                return "waiting";
            }

        case State::BOSS_SPAWNING:
            return "boss_spawning";

        case State::BOSS_FIGHT:
            if (enemiesAlive <= 0) {
                state = State::CLEARED;
                waitTimer = waitDuration;
                if (currentLevel == 10) {
                    return "game_victory";
                }
                return "boss_defeated";
            }
            return "boss_fight";

        case State::WAITING:
            if (enemiesAlive <= 0) {
                state = State::CLEARED;
                waitTimer = waitDuration;
                if (currentLevel == 10) {
                    return "game_victory";
                }
                return "level_cleared";
            }
            return "waiting";

        default:
            return "idle";
    }
}

void LevelState::Controller::onBossSpawned() {
    bossSpawned = true;
    state = State::BOSS_FIGHT;
}