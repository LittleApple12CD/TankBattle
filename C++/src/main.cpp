#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Menu.h"

int main() {
    sf::VideoMode vm(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
    sf::RenderWindow window(vm, "Tank Battle - C++");
    window.setFramerateLimit(FPS);

    Game game;
    Menu menu;
    bool inMenu = true;

    bool p1Left=false, p1Right=false, p1Up=false, p1Down=false, p1Shoot=false;
    bool p2Left=false, p2Right=false, p2Up=false, p2Down=false, p2Shoot=false;

    while (window.isOpen()) {
        float dt = 1.0f / FPS;

        // 事件处理
        while (auto eventOpt = window.pollEvent()) {
            sf::Event event = *eventOpt;
            if (event.is<sf::Event::Closed>()) window.close();

            if (event.is<sf::Event::KeyPressed>()) {
                // ESC 键处理
                if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape) {
                    if (inMenu) {
                        window.close();
                    } else {
                        inMenu = true;
                        menu.reset();
                        game.setMenuMode();
                    }
                    continue;
                }

                if (inMenu) {
                    std::string result = menu.handleInput(event);
                    if (result == "single_player") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.initLevel();
                        game.setSingleMode(true);
                        game.setPvpMode(false);
                    } else if (result == "pvp") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.initLevel();
                        game.setSingleMode(false);
                        game.setPvpMode(true);
                    } else if (result == "pve") {
                        inMenu = false;
                        game.setPlayingMode();
                        game.initLevel();
                        game.setSingleMode(false);
                        game.setPvpMode(false);
                    } else if (result == "exit") {
                        window.close();
                    }
                    continue;
                }

                // ===== 游戏中 =====
                // 禁用 O 和 G
                if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::O ||
                    event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::G) {
                    continue;
                }

                // P1 控制
                switch (event.getIf<sf::Event::KeyPressed>()->code) {
                    case sf::Keyboard::Key::Left: p1Left = true; break;
                    case sf::Keyboard::Key::Right: p1Right = true; break;
                    case sf::Keyboard::Key::Up: p1Up = true; break;
                    case sf::Keyboard::Key::Down: p1Down = true; break;
                    case sf::Keyboard::Key::Space: p1Shoot = true; break;
                    case sf::Keyboard::Key::A: p2Left = true; break;
                    case sf::Keyboard::Key::D: p2Right = true; break;
                    case sf::Keyboard::Key::W: p2Up = true; break;
                    case sf::Keyboard::Key::S: p2Down = true; break;
                    case sf::Keyboard::Key::J: p2Shoot = true; break;
                    case sf::Keyboard::Key::P: game.setPaused(!game.isPaused()); break;
                    case sf::Keyboard::Key::R: game.initLevel(); break;
                    default: break;
                }
            }

            // 按键释放
            if (event.is<sf::Event::KeyReleased>()) {
                switch (event.getIf<sf::Event::KeyReleased>()->code) {
                    case sf::Keyboard::Key::Left: p1Left = false; break;
                    case sf::Keyboard::Key::Right: p1Right = false; break;
                    case sf::Keyboard::Key::Up: p1Up = false; break;
                    case sf::Keyboard::Key::Down: p1Down = false; break;
                    case sf::Keyboard::Key::Space: p1Shoot = false; break;
                    case sf::Keyboard::Key::A: p2Left = false; break;
                    case sf::Keyboard::Key::D: p2Right = false; break;
                    case sf::Keyboard::Key::W: p2Up = false; break;
                    case sf::Keyboard::Key::S: p2Down = false; break;
                    case sf::Keyboard::Key::J: p2Shoot = false; break;
                    default: break;
                }
            }
        }

        // 更新（仅游戏中）
        if (!inMenu && !game.isPaused() && !game.isGameOver()) {
            // P1
            int dx1=0, dy1=0;
            if (p1Left) dx1 = -1;
            else if (p1Right) dx1 = 1;
            else if (p1Up) dy1 = -1;
            else if (p1Down) dy1 = 1;
            if (dx1 != 0 || dy1 != 0) game.movePlayer1(dx1, dy1);
            if (p1Shoot) { game.player1Shoot(); p1Shoot = false; }

            // P2
            int dx2=0, dy2=0;
            if (p2Left) dx2 = -1;
            else if (p2Right) dx2 = 1;
            else if (p2Up) dy2 = -1;
            else if (p2Down) dy2 = 1;
            if (dx2 != 0 || dy2 != 0) game.movePlayer2(dx2, dy2);
            if (p2Shoot) { game.player2Shoot(); p2Shoot = false; }

            game.update(dt);
        }

        // 渲染
        if (inMenu) {
            menu.draw(window);
        } else {
            game.draw(window);
        }
        window.display();
    }

    return 0;
}