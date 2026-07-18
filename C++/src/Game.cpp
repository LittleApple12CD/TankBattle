#include "Game.h"
#include "MapGenerator.h"
#include <cmath>
#include <algorithm>

Game::Game()
    : player1(nullptr), player2(nullptr), enemySpawnTimer(0),
      score(0), gameOver(false), paused(false), currentMap(0), pvpMode(false),
      singleMode(false),
      rng(std::random_device{}()), dist(0.0f, 1.0f), intDist(0, 4) {
    initLevel();
}

void Game::initLevel() {
    gameOver = false;
    paused = false;
    score = 0;
    enemySpawnTimer = 0;
    enemyCount = singleMode ? ENEMY_COUNT * 2 : ENEMY_COUNT;  // 根据模式设置敌人数量
    walls.clear();
    explosions.clear();
    enemies.clear();
    enemyAIs.clear();
    delete player1;
    player1 = nullptr;
    delete player2;
    player2 = nullptr;
    currentMap = intDist(rng);
    MapGenerator::generateMap(currentMap, walls);
    
    // 玩家1（始终存在）
    float p1x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    float p1y = GRID_OFFSET_Y + (GRID_SIZE-3)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
    player1 = new Tank(p1x, p1y, COLOR_PLAYER1, TANK_SPEED, true, 1);
    
    // 玩家2：仅在双人模式存在
    if (!singleMode) {
        float p2x = GRID_OFFSET_X + (GRID_SIZE-2)*CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        float p2y = GRID_OFFSET_Y + CELL_SIZE + CELL_SIZE/2.0f - TANK_SIZE/2.0f;
        player2 = new Tank(p2x, p2y, COLOR_PLAYER2, TANK_SPEED, true, 2);
    } else {
        player2 = nullptr;  // 单人模式没有玩家2
    }
    
    // 生成敌人（使用 enemyCount）
    if (!pvpMode) {
        for (int i = 0; i < enemyCount; ++i) spawnEnemy();
    }
}

void Game::togglePvpMode() {
    pvpMode = !pvpMode;
    initLevel();
}

void Game::toggleSingleMode() {
    singleMode = !singleMode;
    if (singleMode) {
        pvpMode = false;      // 单人模式禁用 PVP
        enemyCount = ENEMY_COUNT * 2;
    } else {
        enemyCount = ENEMY_COUNT;
    }
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
    for (auto& enemy : enemies) {
        if (testRect.findIntersection(enemy.getRect()).has_value()) return;
    }
    if (player1 && player1->isAlive() && testRect.findIntersection(player1->getRect()).has_value()) return;
    if (player2 && player2->isAlive() && testRect.findIntersection(player2->getRect()).has_value()) return;
    
    enemies.emplace_back(x, y, COLOR_ENEMY, TANK_SPEED*0.7f, false, 0);
    enemyAIs.emplace_back(&enemies.back(), this);
}

void Game::update(float dt) {
    if (gameOver || paused) return;
    
    if (player1 && player1->isAlive()) player1->update(dt);
    if (player2 && player2->isAlive()) player2->update(dt);
    
    if (!pvpMode) {
        for (auto& ai : enemyAIs) {
            ai.update(dt);
        }
        
        auto it = enemies.begin();
        auto aiIt = enemyAIs.begin();
        while (it != enemies.end()) {
            if (!it->isAlive()) {
                it = enemies.erase(it);
                aiIt = enemyAIs.erase(aiIt);
            } else {
                ++it;
                ++aiIt;
            }
        }
        
        enemySpawnTimer -= dt;
        if (enemySpawnTimer <= 0 && enemies.size() < ENEMY_COUNT) {
            spawnEnemy();
            enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
        }
    }
    
    handleBulletCollisions();
    
    for (int i = explosions.size() - 1; i >= 0; --i) {
        explosions[i].update(dt);
        if (!explosions[i].isAlive()) {
            explosions.erase(explosions.begin() + i);
        }
    }
    
    if (pvpMode) {
        if ((player1 && !player1->isAlive()) || (player2 && !player2->isAlive())) {
            gameOver = true;
        }
    } else {
        if (player1 && !player1->isAlive() && player2 && !player2->isAlive()) {
            gameOver = true;
        }
    }

    if (singleMode) {
        if (player1 && !player1->isAlive()) {
            gameOver = true;
        }
    } else {
        if (player1 && !player1->isAlive() && player2 && !player2->isAlive()) {
            gameOver = true;
        }
    }
}


void Game::handleBulletCollisions() {
    std::vector<Bullet*> allBullets;
    
    if (player1) {
        for (auto& b : player1->bullets) allBullets.push_back(&b);
    }
    if (player2) {
        for (auto& b : player2->bullets) allBullets.push_back(&b);
    }
    if (!pvpMode) {
        for (auto& enemy : enemies) {
            for (auto& b : enemy.bullets) allBullets.push_back(&b);
        }
    }

    for (auto* bullet : allBullets) {
        if (!bullet->isAlive()) continue;
        
        bool bulletHit = false;
        for (auto& wall : walls) {
            if (!wall.isAlive()) continue;
            if (bullet->getRect().findIntersection(wall.getRect()).has_value()) {
                bullet->setAlive(false);
                bulletHit = true;
                if (!wall.isSteel()) wall.setAlive(false);
                addExplosion(bullet->getX() + BULLET_SIZE/2.0f, bullet->getY() + BULLET_SIZE/2.0f);
                break;
            }
        }
        if (bulletHit || !bullet->isAlive()) continue;

        if (pvpMode) {
            if (bullet->isPlayerBullet() && bullet->getPlayerId() == 1) {
                if (player2 && player2->isAlive() && bullet->getRect().findIntersection(player2->getRect()).has_value()) {
                    bullet->setAlive(false);
                    player2->setLives(player2->getLives() - 1);
                    addExplosion(player2->getCenter().x, player2->getCenter().y);
                    if (player2->getLives() <= 0) player2->setAlive(false);
                    continue;
                }
            }
            if (bullet->isPlayerBullet() && bullet->getPlayerId() == 2) {
                if (player1 && player1->isAlive() && bullet->getRect().findIntersection(player1->getRect()).has_value()) {
                    bullet->setAlive(false);
                    player1->setLives(player1->getLives() - 1);
                    addExplosion(player1->getCenter().x, player1->getCenter().y);
                    if (player1->getLives() <= 0) player1->setAlive(false);
                    continue;
                }
            }
        } else {
            if (bullet->isPlayerBullet()) {
                for (auto& enemy : enemies) {
                    if (enemy.isAlive() && bullet->getRect().findIntersection(enemy.getRect()).has_value()) {
                        bullet->setAlive(false);
                        enemy.setAlive(false);
                        score += 10;
                        addExplosion(enemy.getCenter().x, enemy.getCenter().y);
                        break;
                    }
                }
            } else {
                Tank* players[2] = {player1, player2};
                for (auto* p : players) {
                    if (p && p->isAlive() && bullet->getRect().findIntersection(p->getRect()).has_value()) {
                        bullet->setAlive(false);
                        p->setLives(p->getLives() - 1);
                        addExplosion(p->getCenter().x, p->getCenter().y);
                        if (p->getLives() <= 0) p->setAlive(false);
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
        for (auto& enemy : enemies) {
            enemy.bullets.remove_if([](const Bullet& b){ return !b.isAlive(); });
        }
    }
}

void Game::addExplosion(float x, float y) {
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
}

void Game::player2Shoot() {
    if (player2 && player2->isAlive()) player2->shoot();
}

sf::Font Game::loadFont() {
    sf::Font font;
    
    // 中文字体路径（按优先级排列）
    const char* paths[] = {
        // Windows 中文字体
        "C:/Windows/Fonts/simhei.ttf",        // 黑体
        "C:/Windows/Fonts/msyh.ttc",          // 微软雅黑
        "C:/Windows/Fonts/simsun.ttc",        // 宋体
        "C:/Windows/Fonts/simkai.ttf",        // 楷体
        "C:/Windows/Fontmsyh.ttc",            // 微软雅黑（备用路径）
        // Linux 中文字体
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
        // 英文字体（备选）
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };
    
    for (const char* p : paths) {
        if (font.openFromFile(p)) {
            return font;
        }
    }
    
    // 如果所有字体都加载失败，返回空字体（图形界面会用默认字体，可能显示方框）
    return font;
}

void Game::draw(sf::RenderWindow& window) {
    window.clear(COLOR_BG);

    // 网格
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
    // border.setCornerRadius(4.0f);  // SFML 3.0 不支持
    window.draw(border);

    // 墙壁
    for (auto& wall : walls) wall.draw(window);
    
    // 敌人
    if (!pvpMode) {
        for (auto& enemy : enemies) enemy.draw(window);
    }
    
    // 玩家
    if (player1) player1->draw(window);
    if (player2) player2->draw(window);
    
    // 爆炸
    for (auto& exp : explosions) exp.draw(window);
    
    // UI
    drawUI(window);
}

void Game::drawUI(sf::RenderWindow& window) {
    sf::Font font;
    
    // 英文字体（Windows 一定有 Arial）
    if (!font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        // 如果 Arial 加载失败，尝试 consola
        (void)font.openFromFile("C:/Windows/Fonts/consola.ttf");
    }
    
    if (font.getInfo().family.empty()) {
        return;
    }
    
    int uiY = 10;
    
    // Mode
    std::string modeText = singleMode ? "Single" : (pvpMode ? "PVP" : "PVE");
    sf::Text modeLabel(font, modeText, 18);
    modeLabel.setFillColor(pvpMode ? COLOR_PVP : COLOR_TEXT);
    modeLabel.setPosition(sf::Vector2f(10, uiY));
    window.draw(modeLabel);
    uiY += 30;

    // Score (PVE only)
    if (!pvpMode) {
        sf::Text scoreText(font, "Score: " + std::to_string(score), 18);
        scoreText.setFillColor(COLOR_TEXT);
        scoreText.setPosition(sf::Vector2f(10, uiY));
        window.draw(scoreText);
        uiY += 30;
    }

    // P1 Lives
    std::string p1Text = "P1: ";
    for (int i=0; i<(player1?player1->getLives():0); ++i) p1Text += "[] ";
    sf::Text p1Label(font, p1Text, 18);
    p1Label.setFillColor(COLOR_PLAYER1);
    p1Label.setPosition(sf::Vector2f(10, uiY));
    window.draw(p1Label);
    uiY += 30;

    // P2 Lives
    std::string p2Text = "P2: ";
    for (int i=0; i<(player2?player2->getLives():0); ++i) p2Text += "[] ";
    sf::Text p2Label(font, p2Text, 18);
    p2Label.setFillColor(COLOR_PLAYER2);
    p2Label.setPosition(sf::Vector2f(10, uiY));
    window.draw(p2Label);
    uiY += 30;

    // Enemies (PVE only)
    if (!pvpMode) {
        sf::Text enemyText(font, "Enemy: " + std::to_string(enemies.size()) + "/" + std::to_string(ENEMY_COUNT), 18);
        enemyText.setFillColor(COLOR_TEXT);
        enemyText.setPosition(sf::Vector2f(10, uiY));
        window.draw(enemyText);
    }

    // Map name
    std::string mapNames[] = {"Empty", "Cross", "Maze", "Bunker", "Sym"};
    sf::Text mapText(font, "Map: " + mapNames[currentMap], 14);
    mapText.setFillColor(COLOR_TEXT_DIM);
    mapText.setPosition(sf::Vector2f(10, WINDOW_HEIGHT - 20));
    window.draw(mapText);

    // Pause
    if (paused) {
        sf::Text pauseText(font, "PAUSED", 36);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f - 80.0f, WINDOW_HEIGHT/2.0f));
        window.draw(pauseText);
    }

    // Game Over
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

    // Controls
    const char* controls[] = {
        "P1: Arrows + SPACE",
        "P2: WASD + J",
        "G: PVP/PVE  P: Pause  R: Restart",
        "ESC: Exit"
    };
    for (int i=0; i<4; ++i) {
        sf::Text ctrlText(font, controls[i], 14);
        ctrlText.setFillColor(COLOR_TEXT_DIM);
        ctrlText.setPosition(sf::Vector2f(WINDOW_WIDTH - 200.0f, 10.0f + i*20.0f));
        window.draw(ctrlText);
    }
}