#include "Game.h"
#include "MapGenerator.h"
#include "LevelState.h"
#include "LevelData.h"
#include "SaveManager.h"
#include "Boss.h"
#include <cmath>
#include <algorithm>

Game::Game()
    : player1(nullptr), player2(nullptr), enemySpawnTimer(0),
      score(0), gameOver(false), paused(false), currentMap(0), pvpMode(false),
      singleMode(true), enemyCount(ENEMY_COUNT),
      rng(std::random_device{}()), dist(0.0f, 1.0f), intDist(0, 4) {

    gameMode = "endless";
    levelController = nullptr;
    level = 1;
    showMessage = "";
    waitingForEnter = false;
    victoryDone = false;

    soundManager.loadSounds();

    initLevel();
}

Game::~Game() {
    for (auto* enemy : enemies) delete enemy;
    for (auto* ai : enemyAIs) delete ai;
    delete player1;
    delete player2;
    delete levelController;
}

void Game::initLevel() {
    // ===== 强制修正模式状态 =====
    if (pvpMode) {
        singleMode = false;
    }
    if (singleMode) {
        pvpMode = false;
    }
    
    gameoverPlayed = false;
    gameOver = false;
    paused = false;
    score = 0;
    enemySpawnTimer = 0;
    walls.clear();
    explosions.clear();

    // ===== 清理敌人 =====
    for (auto* enemy : enemies) delete enemy;
    enemies.clear();
    for (auto* ai : enemyAIs) delete ai;
    enemyAIs.clear();

    // ===== 清理玩家（重要！） =====
    delete player1;
    player1 = nullptr;
    delete player2;
    player2 = nullptr;
    
    powerups.clear();
    powerupTimer = 0.0f;

    if (gameMode == "level") {
        if (levelController != nullptr) {
            delete levelController;
        }
        levelController = new LevelState::Controller();
        levelController->startLevel(level);
        
        currentMap = LevelData::getMapId(level);
        enemyCount = 0;
        showMessage = "";
        waitingForEnter = false;
        victoryDone = false;
    } else {
        currentMap = intDist(rng);
        enemyCount = ENEMY_COUNT;
    }
    
    MapGenerator::generateMap(currentMap, walls);

    // ===== 创建玩家1（总是存在） =====
    float p1x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    float p1y = GRID_OFFSET_Y + (GRID_SIZE-3)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    player1 = new Tank(p1x, p1y, COLOR_PLAYER1, TANK_SPEED, true, 1);

    // ===== 根据模式创建玩家2 =====
    if (!singleMode) {
        float p2x = GRID_OFFSET_X + (GRID_SIZE-2)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        float p2y = GRID_OFFSET_Y + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player2 = new Tank(p2x, p2y, COLOR_PLAYER2, TANK_SPEED, true, 2);
    } else {
        player2 = nullptr;
    }

    // ===== 生成敌人 =====
    if (gameMode == "endless" && !pvpMode) {
        for (int i = 0; i < enemyCount; ++i) {
            spawnEnemy();
        }
    }
}

void Game::togglePvpMode() {
    pvpMode = !pvpMode;
    initLevel();
}

void Game::toggleSingleMode() {
    singleMode = !singleMode;
    initLevel();
}

void Game::spawnEnemy() {
    if (enemies.size() >= enemyCount) return;

    int spawnPos[3][2] = {{GRID_SIZE-2,1},{GRID_SIZE/2,1},{1,1}};
    int idx = static_cast<int>(dist(rng)*3);
    if (idx >= 3) idx = 2;
    float x = GRID_OFFSET_X + spawnPos[idx][0]*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    float y = GRID_OFFSET_Y + spawnPos[idx][1]*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;

    sf::FloatRect testRect(sf::Vector2f(x,y), sf::Vector2f(TANK_SIZE,TANK_SIZE));
    for (auto* enemy : enemies) {
        if (testRect.findIntersection(enemy->getRect()).has_value()) return;
    }
    if (player1 && player1->isAlive() && testRect.findIntersection(player1->getRect()).has_value()) return;
    if (player2 && player2->isAlive() && testRect.findIntersection(player2->getRect()).has_value()) return;

    Tank* enemy = new Tank(x, y, COLOR_ENEMY, TANK_SPEED*0.7f, false, 0);
    enemies.push_back(enemy);
    enemyAIs.push_back(new EnemyAI(enemy, this));
}

void Game::update(float dt) {
    if (gameOver || paused) return;

    updatePowerups(dt);

    if (player1 && player1->isAlive()) player1->update(dt);
    if (player2 && player2->isAlive()) player2->update(dt);

    if (!pvpMode) {
        for (auto* ai : enemyAIs) {
            ai->update(dt);
        }

        auto it = enemies.begin();
        auto aiIt = enemyAIs.begin();
        while (it != enemies.end()) {
            if (!(*it)->isAlive()) {
                delete *it;
                it = enemies.erase(it);
                delete *aiIt;
                aiIt = enemyAIs.erase(aiIt);
            } else {
                ++it;
                ++aiIt;
            }
        }

        if (gameMode == "endless") {
            enemySpawnTimer -= dt;
            if (enemySpawnTimer <= 0 && enemies.size() < ENEMY_COUNT) {
                spawnEnemy();
                enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
            }
        } else {
            updateLevelEnemySpawn(dt);
        }
    }

    handleBulletCollisions();

    for (int i = explosions.size() - 1; i >= 0; --i) {
        explosions[i].update(dt);
        if (!explosions[i].isAlive()) {
            explosions.erase(explosions.begin() + i);
        }
    }

    // ===== 游戏结束检查 =====
    if (pvpMode) {
        if ((player1 && !player1->isAlive()) || (player2 && !player2->isAlive())) {
            if (!gameoverPlayed) {
                soundManager.play("gameover");
                gameoverPlayed = true;
            }
            gameOver = true;
        }
    } else {
        if (singleMode) {
            if (player1 && !player1->isAlive()) {
                if (!gameoverPlayed) {
                    soundManager.play("gameover");
                    gameoverPlayed = true;
                }
                gameOver = true;
            }
        } else {
            if (player1 && !player1->isAlive() && player2 && !player2->isAlive()) {
                if (!gameoverPlayed) {
                    soundManager.play("gameover");
                    gameoverPlayed = true;
                }
                gameOver = true;
            }
        }
    }
}

void Game::updateLevelEnemySpawn(float dt) {
    if (levelController == nullptr) return;

    int enemiesAlive = 0;
    for (auto* enemy : enemies) {
        if (enemy->isAlive()) enemiesAlive++;
    }

    std::string result = levelController->update(dt, enemiesAlive);

    if (result == "spawn_enemy") {
        spawnEnemyForLevel();
    } else if (result == "spawn_boss") {
        spawnBoss();
    } else if (result == "level_cleared") {
        onLevelCleared();
    } else if (result == "boss_defeated") {
        onBossDefeated();
    } else if (result == "game_victory") {
        onGameVictory();
    } else if (result == "gameover") {
        onLevelFailed();
    } else if (result == "victory_done") {
        victoryDone = true;
    }
}

void Game::spawnEnemyForLevel() {
    if (levelController == nullptr) return;
    const LevelData::LevelConfig* config = LevelData::getLevelConfig(level);
    if (config == nullptr) return;

    std::string typeName = config->enemyType;
    if (typeName.empty()) return;

    auto it = LevelData::ENEMY_TYPES.find(typeName);
    if (it == LevelData::ENEMY_TYPES.end()) {
        it = LevelData::ENEMY_TYPES.find("normal");
        if (it == LevelData::ENEMY_TYPES.end()) return;
    }

    const LevelData::EnemyType& enemyType = it->second;
    int hp = enemyType.hp;
    float speedMult = static_cast<float>(enemyType.speedMult);
    sf::Color color = enemyType.color;

    int spawnPos[3][2] = {{GRID_SIZE-2, 1}, {GRID_SIZE/2, 1}, {1, 1}};
    int idx = static_cast<int>(dist(rng) * 3);
    if (idx >= 3) idx = 2;

    float x = GRID_OFFSET_X + spawnPos[idx][0] * CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    float y = GRID_OFFSET_Y + spawnPos[idx][1] * CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;

    sf::FloatRect testRect(sf::Vector2f(x, y), sf::Vector2f(TANK_SIZE, TANK_SIZE));
    for (auto* enemy : enemies) {
        if (enemy->isAlive() && testRect.findIntersection(enemy->getRect()).has_value()) {
            return;
        }
    }
    if (player1 && player1->isAlive() && testRect.findIntersection(player1->getRect()).has_value()) {
        return;
    }

    Tank* enemy = new Tank(x, y, color, TANK_SPEED * speedMult, false, 0);
    enemy->setLives(hp);
    enemies.push_back(enemy);
    enemyAIs.push_back(new EnemyAI(enemy, this));
}

void Game::spawnBoss() {
    const LevelData::BossConfig* bossConfig = LevelData::getBossConfig(level);
    if (bossConfig == nullptr) return;

    int bossSize = static_cast<int>(TANK_SIZE * bossConfig->sizeMult);
    float x = GRID_OFFSET_X + GRID_SIZE * CELL_SIZE / 2.0f - bossSize / 2.0f;
    float y = GRID_OFFSET_Y + CELL_SIZE * 2;

    Boss* boss = new Boss(x, y, bossConfig->hp, bossConfig->sizeMult,
                          bossConfig->speedMult, bossConfig->bulletDamage,
                          bossConfig->bulletSpeedMult);
    boss->setLives(bossConfig->hp);
    boss->isBoss = true;

    enemies.push_back(boss);
    enemyAIs.push_back(new EnemyAI(boss, this));

    if (levelController != nullptr) {
        levelController->onBossSpawned();
    }
}

// ===== 关卡事件 =====

void Game::onLevelCleared() {
    soundManager.play("victory");
    showMessage = "STAGE CLEAR!";
    waitingForEnter = true;
    if (levelController != nullptr) {
        levelController->setState(LevelState::State::CLEARED);
        levelController->setWaitTimer(999.0);
    }
    SaveManager::saveProgress(level);
}

void Game::onBossDefeated() {
    soundManager.play("victory");
    showMessage = "BOSS DEFEATED!";
    waitingForEnter = true;
    if (levelController != nullptr) {
        levelController->setState(LevelState::State::CLEARED);
        levelController->setWaitTimer(999.0);
    }
    SaveManager::saveProgress(level);
}

void Game::onGameVictory() {
    soundManager.play("victory");
    showMessage = "GAME VICTORY!";
    waitingForEnter = true;
    if (levelController != nullptr) {
        levelController->setState(LevelState::State::VICTORY);
        levelController->setWaitTimer(999.0);
    }
    SaveManager::saveProgress(10);
}

void Game::onLevelFailed() {
    gameOver = true;
    showMessage = "GAME OVER";
    waitingForEnter = true;
}

// ===== continueToNext =====
void Game::continueToNext() {
    if (!waitingForEnter) return;

    waitingForEnter = false;
    showMessage = "";

    // 检查是否通关游戏（第10关完成）
    if (level >= LevelData::MAX_LEVEL) {
        if (level == LevelData::MAX_LEVEL) {
            showMessage = "GAME VICTORY!";
            waitingForEnter = true;
            victoryDone = true;
            if (levelController != nullptr) {
                levelController->setState(LevelState::State::VICTORY);
            }
            SaveManager::saveProgress(level);
            return;
        }
        victoryDone = true;
        return;
    }

    // 检查是否为 Boss 关卡（已经被击败）
    bool isBossLevel = LevelData::isBossLevel(level);
    
    // 进入下一关
    level++;
    SaveManager::saveProgress(level);

    // 重置关卡控制器
    if (levelController != nullptr) {
        delete levelController;
        levelController = new LevelState::Controller();
        levelController->startLevel(level);
    }

    // 重置玩家
    if (player1 != nullptr) {
        player1->setLives(PLAYER_LIVES);
        player1->effects.clear();
        player1->x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player1->y = GRID_OFFSET_Y + (GRID_SIZE-3)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player1->setAlive(true);
        player1->w = TANK_SIZE;
        player1->h = TANK_SIZE;
    }
    delete player2;
    player2 = nullptr;

    // 清除敌人
    for (auto* enemy : enemies) delete enemy;
    enemies.clear();
    for (auto* ai : enemyAIs) delete ai;
    enemyAIs.clear();

    // 初始化关卡
    this->currentMap = LevelData::getMapId(level);
    initLevel();
    this->gameMode = "level";
    this->victoryDone = false;
}

// ===== 子弹碰撞 =====

void Game::handleBulletCollisions() {
    std::vector<Bullet*> allBullets;

    if (player1) {
        for (auto& b : player1->bullets) allBullets.push_back(&b);
    }
    if (player2) {
        for (auto& b : player2->bullets) allBullets.push_back(&b);
    }
    if (!pvpMode) {
        for (auto* enemy : enemies) {
            for (auto& b : enemy->bullets) allBullets.push_back(&b);
        }
    }

    for (auto* bullet : allBullets) {
        if (!bullet->isAlive()) continue;

        bool bulletHit = false;

        // 子弹 vs 墙壁
        for (auto& wall : walls) {
            if (!wall.isAlive()) continue;
            if (bullet->getRect().findIntersection(wall.getRect()).has_value()) {
                if (bullet->damage == 2 && bullet->isPlayerBullet()) {
                    explodeAt(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                    bullet->setAlive(false);
                    bulletHit = true;
                    if (!wall.isSteel()) wall.setAlive(false);
                    addExplosion(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                } else {
                    bullet->setAlive(false);
                    bulletHit = true;
                    if (!wall.isSteel()) wall.setAlive(false);
                    addExplosion(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                }
                break;
            }
        }
        if (bulletHit || !bullet->isAlive()) continue;

        // PVP 模式
        if (pvpMode) {
            if (bullet->isPlayerBullet() && bullet->getPlayerId() == 1) {
                if (player2 && player2->isAlive() && bullet->getRect().findIntersection(player2->getRect()).has_value()) {
                    if (bullet->damage == 2 && bullet->isPlayerBullet()) {
                        explodeAt(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                        bullet->setAlive(false);
                    } else {
                        bullet->setAlive(false);
                        if (player2->effects.find("protection") != player2->effects.end()) {
                            player2->effects.erase("protection");
                        } else {
                            player2->setLives(player2->getLives() - bullet->damage);
                            addExplosion(player2->getCenter().x, player2->getCenter().y);
                        }
                        if (player2->getLives() <= 0) player2->setAlive(false);
                    }
                    continue;
                }
            }
            if (bullet->isPlayerBullet() && bullet->getPlayerId() == 2) {
                if (player1 && player1->isAlive() && bullet->getRect().findIntersection(player1->getRect()).has_value()) {
                    if (bullet->damage == 2 && bullet->isPlayerBullet()) {
                        explodeAt(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                        bullet->setAlive(false);
                    } else {
                        bullet->setAlive(false);
                        if (player1->effects.find("protection") != player1->effects.end()) {
                            player1->effects.erase("protection");
                        } else {
                            player1->setLives(player1->getLives() - bullet->damage);
                            addExplosion(player1->getCenter().x, player1->getCenter().y);
                        }
                        if (player1->getLives() <= 0) player1->setAlive(false);
                    }
                    continue;
                }
            }
        } else {
            // PVE 模式
            if (bullet->isPlayerBullet()) {
                for (auto* enemy : enemies) {
                    if (!enemy->isAlive()) continue;
                    if (bullet->getRect().findIntersection(enemy->getRect()).has_value()) {

                        // 检查是否为 Boss
                        if (enemy->isBoss) {
                            enemy->setLives(enemy->getLives() - bullet->damage);
                            bullet->setAlive(false);

                            if (enemy->getLives() <= 0) {
                                enemy->setAlive(false);
                                score += 50;
                                addExplosion(enemy->getCenter().x, enemy->getCenter().y);

                                // 判断是否为最后一关
                                if (level >= LevelData::MAX_LEVEL) {
                                    onGameVictory();
                                } else {
                                    onBossDefeated();
                                }
                            }
                        } else {
                            // 普通敌人
                            if (bullet->damage == 2 && bullet->isPlayerBullet()) {
                                explodeAt(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                                bullet->setAlive(false);
                            } else {
                                bullet->setAlive(false);
                                if (enemy->effects.find("protection") != enemy->effects.end()) {
                                    enemy->effects.erase("protection");
                                } else {
                                    enemy->setLives(enemy->getLives() - bullet->damage);
                                }
                                if (enemy->getLives() <= 0) {
                                    enemy->setAlive(false);
                                    score += 10;
                                    addExplosion(enemy->getCenter().x, enemy->getCenter().y);
                                }
                            }
                        }
                        break;
                    }
                }
            } else {
                // 敌人子弹 vs 玩家
                Tank* players[2] = {player1, player2};
                for (auto* p : players) {
                    if (p && p->isAlive() && bullet->getRect().findIntersection(p->getRect()).has_value()) {
                        if (bullet->damage == 2 && bullet->isPlayerBullet()) {
                            explodeAt(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                            bullet->setAlive(false);
                        } else {
                            bullet->setAlive(false);
                            if (p->effects.find("protection") != p->effects.end()) {
                                p->effects.erase("protection");
                            } else {
                                p->setLives(p->getLives() - bullet->damage);
                                addExplosion(p->getCenter().x, p->getCenter().y);
                            }
                            if (p->getLives() <= 0) p->setAlive(false);
                        }
                        break;
                    }
                }
            }
        }
    }

    if (player1) {
        player1->bullets.remove_if([](const Bullet& b){ return !b.isAlive(); });
    }
    if (player2) {
        player2->bullets.remove_if([](const Bullet& b){ return !b.isAlive(); });
    }
    if (!pvpMode) {
        for (auto* enemy : enemies) {
            enemy->bullets.remove_if([](const Bullet& b){ return !b.isAlive(); });
        }
    }
}

// ===== 辅助方法 =====

void Game::addExplosion(float x, float y) {
    soundManager.play("explode");
    for (int i=0; i<4; ++i) {
        float ox = x + (dist(rng)-0.5f)*30.0f;
        float oy = y + (dist(rng)-0.5f)*30.0f;
        explosions.emplace_back(ox, oy);
    }
}

void Game::movePlayer1(int dx, int dy) {
    if (player1 && player1->isAlive()) {
        player1->setDirection(dx, dy);
        player1->move(dx, dy, walls);
    }
}

void Game::movePlayer2(int dx, int dy) {
    if (player2 && player2->isAlive()) {
        player2->setDirection(dx, dy);
        player2->move(dx, dy, walls);
    }
}

void Game::player1Shoot() {
    if (player1 && player1->isAlive()) player1->shoot();
    soundManager.play("shoot");
}

void Game::player2Shoot() {
    if (player2 && player2->isAlive()) player2->shoot();
    soundManager.play("shoot");
}

sf::Font Game::loadFont() {
    sf::Font font;
    const char* paths[] = {
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simsun.ttc",
        "C:/Windows/Fonts/simkai.ttf",
        "C:/Windows/Fontmsyh.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };
    for (const char* p : paths) {
        if (font.openFromFile(p)) {
            return font;
        }
    }
    return font;
}

void Game::draw(sf::RenderWindow& window) {
    window.clear(COLOR_BG);

    for (int i=0; i<=GRID_SIZE; ++i) {
        sf::Vertex hLine[2];
        hLine[0].position = sf::Vector2f(GRID_OFFSET_X + i*CELL_SIZE, GRID_OFFSET_Y);
        hLine[0].color = COLOR_GRID;
        hLine[1].position = sf::Vector2f(GRID_OFFSET_X + i*CELL_SIZE, GRID_OFFSET_Y + GRID_SIZE*CELL_SIZE);
        hLine[1].color = COLOR_GRID;

        sf::Vertex vLine[2];
        vLine[0].position = sf::Vector2f(GRID_OFFSET_X, GRID_OFFSET_Y + i*CELL_SIZE);
        vLine[0].color = COLOR_GRID;
        vLine[1].position = sf::Vector2f(GRID_OFFSET_X + GRID_SIZE*CELL_SIZE, GRID_OFFSET_Y + i*CELL_SIZE);
        vLine[1].color = COLOR_GRID;

        window.draw(hLine, 2, sf::PrimitiveType::Lines);
        window.draw(vLine, 2, sf::PrimitiveType::Lines);
    }

    sf::RectangleShape border(sf::Vector2f(GRID_SIZE*CELL_SIZE, GRID_SIZE*CELL_SIZE));
    border.setPosition(sf::Vector2f(GRID_OFFSET_X, GRID_OFFSET_Y));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(60,60,80));
    border.setOutlineThickness(2.0f);
    window.draw(border);

    for (auto& p : powerups) {
        p.draw(window);
    }

    for (auto& wall : walls) wall.draw(window);

    // 敌人
    if (!pvpMode) {
        for (auto* enemy : enemies) {
            enemy->draw(window);
        }
    }

    if (player1) player1->draw(window);
    if (player2) player2->draw(window);

    // 子弹（包括 Boss 子弹）
    if (!pvpMode) {
        for (auto* enemy : enemies) {
            for (auto& b : enemy->bullets) {
                b.draw(window);
            }
        }
    }
    if (player1) {
        for (auto& b : player1->bullets) b.draw(window);
    }
    if (player2) {
        for (auto& b : player2->bullets) b.draw(window);
    }

    for (auto& exp : explosions) exp.draw(window);

    // 绘制 UI
    drawUI(window);

    // 关卡消息（最上层）
    if (!showMessage.empty()) {
        sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        sf::Font font;
        if (font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
            sf::Text msg(font, showMessage, 72);
            msg.setFillColor(sf::Color(255, 255, 100));
            msg.setStyle(sf::Text::Bold);
            sf::FloatRect bounds = msg.getLocalBounds();
            msg.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - bounds.size.x/2.0f,
                                         WINDOW_HEIGHT/2.0f - 40.0f));
            window.draw(msg);

            if (waitingForEnter) {
                sf::Text hint(font, "Press ENTER to continue", 28);
                hint.setFillColor(sf::Color(200, 200, 200));
                bounds = hint.getLocalBounds();
                hint.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - bounds.size.x/2.0f,
                                              WINDOW_HEIGHT/2.0f + 40.0f));
                window.draw(hint);
            }
        }
    }
}

void Game::drawUI(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        (void)font.openFromFile("C:/Windows/Fonts/consola.ttf");
    }

    if (font.getInfo().family.empty()) return;

    int uiY = 10;

    std::string modeText;
    if (gameMode == "level") {
        modeText = "Level " + std::to_string(level) + "/" + std::to_string(LevelData::MAX_LEVEL);
    } else if (singleMode) {
        modeText = "Single";
    } else {
        modeText = pvpMode ? "PVP" : "PVE";
    }

    sf::Text modeLabel(font, modeText, 18);
    modeLabel.setFillColor(pvpMode ? COLOR_PVP : COLOR_TEXT);
    modeLabel.setPosition(sf::Vector2f(10, uiY));
    window.draw(modeLabel);
    uiY += 30;

    if (!pvpMode && gameMode == "endless") {
        sf::Text scoreText(font, "Score: " + std::to_string(score), 18);
        scoreText.setFillColor(COLOR_TEXT);
        scoreText.setPosition(sf::Vector2f(10, uiY));
        window.draw(scoreText);
        uiY += 30;
    }

    if (gameMode == "level" && levelController != nullptr) {
        sf::Text progressText(font,
            "Enemy: " + std::to_string(enemies.size()) + " / " +
            std::to_string(levelController->getEnemiesTotal()), 18);
        progressText.setFillColor(COLOR_TEXT);
        progressText.setPosition(sf::Vector2f(10, uiY));
        window.draw(progressText);
        uiY += 30;
    }

    std::string p1Text = "P1: ";
    for (int i = 0; i < (player1 ? player1->getLives() : 0); ++i) p1Text += "[] ";
    sf::Text p1Label(font, p1Text, 18);
    p1Label.setFillColor(COLOR_PLAYER1);
    p1Label.setPosition(sf::Vector2f(10, uiY));
    window.draw(p1Label);
    uiY += 30;

    std::string p2Text = "P2: ";
    for (int i = 0; i < (player2 ? player2->getLives() : 0); ++i) p2Text += "[] ";
    sf::Text p2Label(font, p2Text, 18);
    p2Label.setFillColor(COLOR_PLAYER2);
    p2Label.setPosition(sf::Vector2f(10, uiY));
    window.draw(p2Label);
    uiY += 30;

    if (!pvpMode && gameMode == "endless") {
        sf::Text enemyText(font, "Enemy: " + std::to_string(enemies.size()) + "/" + std::to_string(ENEMY_COUNT), 18);
        enemyText.setFillColor(COLOR_TEXT);
        enemyText.setPosition(sf::Vector2f(10, uiY));
        window.draw(enemyText);
    }

    std::string mapNames[] = {"Empty", "Cross", "Maze", "Bunker", "Sym"};
    sf::Text mapText(font, "Map: " + mapNames[currentMap], 14);
    mapText.setFillColor(COLOR_TEXT_DIM);
    mapText.setPosition(sf::Vector2f(10, WINDOW_HEIGHT - 20));
    window.draw(mapText);

    if (paused) {
        sf::Text pauseText(font, "PAUSED", 36);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - 80.0f, WINDOW_HEIGHT/2.0f));
        window.draw(pauseText);
    }

    if (gameOver) {
        sf::Text overText(font, "GAME OVER", 36);
        overText.setFillColor(sf::Color(255,50,50));
        overText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - 120.0f, WINDOW_HEIGHT/2.0f - 20.0f));
        window.draw(overText);

        sf::Text restartText(font, "Press R to restart", 18);
        restartText.setFillColor(COLOR_TEXT);
        restartText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - 80.0f, WINDOW_HEIGHT/2.0f + 40.0f));
        window.draw(restartText);
    }

    const char* controls[] = {
        "P1: Arrows + SPACE",
        "P2: WASD + J",
        "G: PVP/PVE  P: Pause  R: Restart",
        "ESC: Exit"
    };
    for (int i = 0; i < 4; ++i) {
        sf::Text ctrlText(font, controls[i], 14);
        ctrlText.setFillColor(COLOR_TEXT_DIM);
        ctrlText.setPosition(sf::Vector2f(WINDOW_WIDTH - 200.0f, 10.0f + i*20.0f));
        window.draw(ctrlText);
    }
}

void Game::updatePowerups(float dt) {
    powerupTimer += dt;
    if (powerupTimer >= powerupInterval && powerups.size() < maxPowerups) {
        spawnPowerup();
        powerupTimer = 0.0f;
    }

    for (auto it = powerups.begin(); it != powerups.end(); ) {
        if (!it->isAlive()) {
            it = powerups.erase(it);
            continue;
        }

        std::vector<Tank*> allTanks;
        if (player1 && player1->isAlive()) allTanks.push_back(player1);
        if (player2 && player2->isAlive()) allTanks.push_back(player2);
        for (auto* enemy : enemies) {
            if (enemy->isAlive()) allTanks.push_back(enemy);
        }

        bool picked = false;
        for (Tank* tank : allTanks) {
            if (it->getRect().findIntersection(tank->getRect()).has_value()) {
                applyPowerup(tank, &(*it));
                it->setAlive(false);
                picked = true;
                break;
            }
        }

        if (picked) {
            it = powerups.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::spawnPowerup() {
    int margin = 40;
    char types[] = {'S', 'P', 'H', 'T'};
    for (int attempt = 0; attempt < 20; ++attempt) {
        float x = GRID_OFFSET_X + margin +
                  static_cast<float>(rand()) / RAND_MAX * (GRID_SIZE * CELL_SIZE - margin * 2 - 24);
        float y = GRID_OFFSET_Y + margin +
                  static_cast<float>(rand()) / RAND_MAX * (GRID_SIZE * CELL_SIZE - margin * 2 - 24);
        sf::FloatRect testRect(sf::Vector2f(x, y), sf::Vector2f(24, 24));
        bool blocked = false;
        for (auto& wall : walls) {
            if (wall.isAlive() && testRect.findIntersection(wall.getRect()).has_value()) {
                blocked = true;
                break;
            }
        }
        if (!blocked) {
            char type = types[rand() % 4];
            powerups.emplace_back(x, y, type);
            return;
        }
    }
}

void Game::applyPowerup(Tank* tank, PowerUp* powerup) {
    soundManager.play("powerup");
    char type = powerup->getType();
    switch (type) {
        case 'H':
            tank->setLives(std::min(tank->getLives() + 1, 5));
            break;
        case 'S':
            tank->effects["speed"] = 5.0f;
            break;
        case 'P':
            tank->effects["protection"] = 5.0f;
            break;
        case 'T':
            tank->effects["strength"] = 5.0f;
            break;
    }
}

void Game::explodeAt(float x, float y) {
    float radius = TANK_SIZE * 2.0f;

    for (auto* enemy : enemies) {
        if (!enemy->isAlive()) continue;
        sf::Vector2f eCenter = enemy->getCenter();
        float dist = std::sqrt(std::pow(eCenter.x - x, 2) + std::pow(eCenter.y - y, 2));
        if (dist <= radius) {
            if (enemy->effects.find("protection") != enemy->effects.end()) {
                enemy->effects.erase("protection");
            } else {
                enemy->setLives(enemy->getLives() - 2);
                if (enemy->getLives() <= 0) {
                    enemy->setAlive(false);
                    score += 10;
                    addExplosion(eCenter.x, eCenter.y);
                }
            }
        }
    }

    if (pvpMode) {
        for (Tank* player : {player1, player2}) {
            if (!player || !player->isAlive()) continue;
            sf::Vector2f pCenter = player->getCenter();
            float dist = std::sqrt(std::pow(pCenter.x - x, 2) + std::pow(pCenter.y - y, 2));
            if (dist <= radius) {
                if (player->effects.find("protection") != player->effects.end()) {
                    player->effects.erase("protection");
                } else {
                    player->setLives(player->getLives() - 2);
                    addExplosion(pCenter.x, pCenter.y);
                    if (player->getLives() <= 0) {
                        player->setAlive(false);
                    }
                }
            }
        }
    }

    for (auto& wall : walls) {
        if (!wall.isAlive() || wall.isSteel()) continue;
        sf::Vector2f wCenter(wall.x + wall.w/2.0f, wall.y + wall.h/2.0f);
        float dist = std::sqrt(std::pow(wCenter.x - x, 2) + std::pow(wCenter.y - y, 2));
        if (dist <= radius) {
            wall.setAlive(false);
        }
    }

    for (int i = 0; i < 8; ++i) {
        float ox = x + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * radius;
        float oy = y + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * radius;
        addExplosion(ox, oy);
    }
}

// ===== 关卡模式 =====

void Game::startLevelMode(int level) {
    this->gameMode = "level";
    this->level = level;
    this->singleMode = true;
    this->pvpMode = false;
    this->enemyCount = 0;

    this->levelController = new LevelState::Controller();
    this->levelController->startLevel(level);

    if (player1 != nullptr) {
        player1->setLives(PLAYER_LIVES);
        player1->effects.clear();
        player1->x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player1->y = GRID_OFFSET_Y + (GRID_SIZE-3)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player1->setAlive(true);
        player1->w = TANK_SIZE;
        player1->h = TANK_SIZE;
    }
    delete player2;
    player2 = nullptr;

    this->currentMap = LevelData::getMapId(level);
    initLevel();

    SaveManager::saveProgress(level);

    this->showMessage = "";
    this->waitingForEnter = false;
    this->victoryDone = false;
}