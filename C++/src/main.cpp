#include <SFML/Graphics.hpp>
#include <iostream>
#include "Game.h"
#include "Menu.h"
#include "SettingsMenu.h"
#include "SaveManager.h"
#include "ConfigManager.h"
#include "SoundManager.h"
#include "mod/ModLoader.h"
#include "resource/ResourcePackLoader.h"
#include "script/ScriptEngine.h"

// ===== 辅助函数：从配置获取按键 =====
sf::Keyboard::Key getKeyFromConfig(int player, const std::string& action) {
    std::string keyStr = ConfigManager::getInstance().getKey(player, action);
    return ConfigManager::stringToKey(keyStr);
}

int main() {
    // ===== 最先初始化 ConfigManager =====
    auto& cfg = ConfigManager::getInstance();

    ModLoader::getInstance().loadGameVersion();
    std::cout << "TankBattle v" << ModLoader::getInstance().getGameVersion() << std::endl;
    
    // ===== 从配置读取窗口大小 =====
    sf::VideoMode vm(sf::Vector2u(cfg.getWindowWidth(), cfg.getWindowHeight()));
    sf::RenderWindow window(vm, "Tank Battle - C++");

    // ===== 设置窗口图标 =====
    sf::Image icon;
    if (icon.loadFromFile("icon.png")) {
        window.setIcon(icon);
    }

    window.setFramerateLimit(60);

    Game game;
    Menu menu;
    SettingsMenu settingsMenu;
    
    bool inMenu = true;
    bool inSettings = false;

    bool p1Left=false, p1Right=false, p1Up=false, p1Down=false, p1Shoot=false;
    bool p2Left=false, p2Right=false, p2Up=false, p2Down=false, p2Shoot=false;

    // ===== 从配置读取键位 =====
    sf::Keyboard::Key KEY_P1_UP = getKeyFromConfig(1, "up");
    sf::Keyboard::Key KEY_P1_DOWN = getKeyFromConfig(1, "down");
    sf::Keyboard::Key KEY_P1_LEFT = getKeyFromConfig(1, "left");
    sf::Keyboard::Key KEY_P1_RIGHT = getKeyFromConfig(1, "right");
    sf::Keyboard::Key KEY_P1_SHOOT = getKeyFromConfig(1, "shoot");
    
    sf::Keyboard::Key KEY_P2_UP = getKeyFromConfig(2, "up");
    sf::Keyboard::Key KEY_P2_DOWN = getKeyFromConfig(2, "down");
    sf::Keyboard::Key KEY_P2_LEFT = getKeyFromConfig(2, "left");
    sf::Keyboard::Key KEY_P2_RIGHT = getKeyFromConfig(2, "right");
    sf::Keyboard::Key KEY_P2_SHOOT = getKeyFromConfig(2, "shoot");

    // ===== 主循环 =====
    while (window.isOpen()) {
        float dt = 1.0f / 60.0f;

        while (auto eventOpt = window.pollEvent()) {
            sf::Event event = *eventOpt;
            
            if (event.is<sf::Event::Closed>()) {
                window.close();
                continue;
            }

            // ===== 设置界面 =====
            if (inSettings) {
                std::string result = settingsMenu.handleInput(event);
                if (result == "back") {
                    inSettings = false;
                    settingsMenu.reset();
                    // 从设置返回后，重新读取键位
                    KEY_P1_UP = getKeyFromConfig(1, "up");
                    KEY_P1_DOWN = getKeyFromConfig(1, "down");
                    KEY_P1_LEFT = getKeyFromConfig(1, "left");
                    KEY_P1_RIGHT = getKeyFromConfig(1, "right");
                    KEY_P1_SHOOT = getKeyFromConfig(1, "shoot");
                    KEY_P2_UP = getKeyFromConfig(2, "up");
                    KEY_P2_DOWN = getKeyFromConfig(2, "down");
                    KEY_P2_LEFT = getKeyFromConfig(2, "left");
                    KEY_P2_RIGHT = getKeyFromConfig(2, "right");
                    KEY_P2_SHOOT = getKeyFromConfig(2, "shoot");
                }
                continue;
            }

            // ===== 主菜单 =====
            if (inMenu) {
                if (event.is<sf::Event::KeyPressed>()) {
                    std::string result = menu.handleInput(event);
                    
                    if (result == "exit") {
                        window.close();
                        continue;
                    } else if (result == "settings") {
                        inSettings = true;
                        settingsMenu.reset();
                        continue;
                    } else if (result == "single_player") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.setSingleMode(true);
                        game.setPvpMode(false);
                        game.initLevel();
                    } else if (result == "pvp") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.setSingleMode(false);
                        game.setPvpMode(true);
                        game.initLevel();
                    } else if (result == "pve") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.setSingleMode(false);
                        game.setPvpMode(false);
                        game.initLevel();
                    } else if (result == "endless_mode") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.setSingleMode(true);
                        game.setPvpMode(false);
                        game.initLevel();
                    } else if (result == "load_game") {
                        int level = SaveManager::hasSave() ? SaveManager::loadProgress() : 1;
                        inMenu = false;
                        game.setPlayingMode();
                        game.startLevelMode(level);
                    } else if (result == "new_game") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.startLevelMode(1);
                    } else if (result == "reload_scripts") {
                        ScriptEngine::getInstance().reloadAll();
                        std::cout << "Scripts reloaded!" << std::endl;
                    } else if (result == "reload_resourcepacks") {
                        game.reloadResourcePacks();
                        std::cout << "Resource packs reloaded!" << std::endl;
                    }
                    // "view_mods" 由 Menu 内部处理
                }
                continue;
            }

            // ===== 游戏模式 - 按键按下 =====
            if (event.is<sf::Event::KeyPressed>()) {
                auto key = event.getIf<sf::Event::KeyPressed>()->code;

                if (key == sf::Keyboard::Key::Escape) {
                    inMenu = true;
                    menu.reset();
                    game.setMenuMode();
                    continue;
                }

                // Enter 键：关卡继续
                if (key == sf::Keyboard::Key::Enter) {
                    if (game.isWaitingForEnter()) {
                        game.continueToNext();
                        if (game.isVictoryDone()) {
                            inMenu = true;
                            menu.reset();
                            game.setMenuMode();
                        }
                    }
                    continue;
                }

                // 禁用 O 和 G
                if (key == sf::Keyboard::Key::O || key == sf::Keyboard::Key::G) {
                    continue;
                }

                // ===== P1 按键 =====
                if (key == KEY_P1_UP) p1Up = true;
                else if (key == KEY_P1_DOWN) p1Down = true;
                else if (key == KEY_P1_LEFT) p1Left = true;
                else if (key == KEY_P1_RIGHT) p1Right = true;
                else if (key == KEY_P1_SHOOT) p1Shoot = true;
                
                // ===== P2 按键 =====
                else if (key == KEY_P2_UP) p2Up = true;
                else if (key == KEY_P2_DOWN) p2Down = true;
                else if (key == KEY_P2_LEFT) p2Left = true;
                else if (key == KEY_P2_RIGHT) p2Right = true;
                else if (key == KEY_P2_SHOOT) p2Shoot = true;
                
                // ===== 通用按键 =====
                else if (key == sf::Keyboard::Key::P) {
                    game.setPaused(!game.isPaused());
                } else if (key == sf::Keyboard::Key::R) {
                    game.initLevel();
                }
            }

            // ===== 游戏模式 - 按键释放 =====
            if (event.is<sf::Event::KeyReleased>()) {
                auto key = event.getIf<sf::Event::KeyReleased>()->code;
                
                if (key == KEY_P1_UP) p1Up = false;
                else if (key == KEY_P1_DOWN) p1Down = false;
                else if (key == KEY_P1_LEFT) p1Left = false;
                else if (key == KEY_P1_RIGHT) p1Right = false;
                else if (key == KEY_P1_SHOOT) p1Shoot = false;
                
                else if (key == KEY_P2_UP) p2Up = false;
                else if (key == KEY_P2_DOWN) p2Down = false;
                else if (key == KEY_P2_LEFT) p2Left = false;
                else if (key == KEY_P2_RIGHT) p2Right = false;
                else if (key == KEY_P2_SHOOT) p2Shoot = false;
            }
        }

        // ===== 更新逻辑 =====
        if (!inMenu && !inSettings && !game.isPaused() && !game.isGameOver()) {
            int dx1=0, dy1=0;
            if (p1Left) dx1 = -1;
            else if (p1Right) dx1 = 1;
            else if (p1Up) dy1 = -1;
            else if (p1Down) dy1 = 1;
            if (dx1 != 0 || dy1 != 0) game.movePlayer1(dx1, dy1);
            if (p1Shoot) { game.player1Shoot(); p1Shoot = false; }

            int dx2=0, dy2=0;
            if (p2Left) dx2 = -1;
            else if (p2Right) dx2 = 1;
            else if (p2Up) dy2 = -1;
            else if (p2Down) dy2 = 1;
            if (dx2 != 0 || dy2 != 0) game.movePlayer2(dx2, dy2);
            if (p2Shoot) { game.player2Shoot(); p2Shoot = false; }

            game.update(dt);
        }

        // ===== 渲染 =====
        window.clear(sf::Color(20, 20, 30));

        if (inSettings) {
            settingsMenu.draw(window);
        } else if (inMenu) {
            menu.draw(window);
        } else {
            game.draw(window);
        }
        
        window.display();
    }

    return 0;
}