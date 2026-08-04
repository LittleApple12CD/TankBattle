#include "Menu.h"
#include <iostream>

Menu::Menu() : state(MAIN), selected(0) {
    mainItems = {"Single Player", "Multiplayer", "Mod", "Settings", "Exit"};
    singleItems = {"Endless Mode", "Level Mode", "Back"};      // 添加
    levelItems = {"Load Game", "New Game", "Back"};           // 添加
    multiItems = {"Local", "Lan", "Server", "Online", "Back"};
    modeItems = {"PVP", "PVE", "Back"};
    currentItems = &mainItems;

    // 加载字体
    bool loaded = font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    if (!loaded) {
        loaded = font.openFromFile("C:/Windows/Fonts/consola.ttf");
    }
    fontTitle = font;
}

Menu::~Menu() {}

std::string Menu::handleInput(const sf::Event& event) {
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Up:
                selected = (selected - 1 + (int)currentItems->size()) % (int)currentItems->size();
                break;
            case sf::Keyboard::Key::Down:
                selected = (selected + 1) % (int)currentItems->size();
                break;
            case sf::Keyboard::Key::Enter:
                return selectCurrent();
            case sf::Keyboard::Key::Escape:
                if (state == MAIN) {
                    return "exit";
                } else {
                    goBack();
                }
                break;
            default:
                break;
        }
    }
    return "";
}

std::string Menu::selectCurrent() {
    std::string choice = (*currentItems)[selected];

    if (state == MAIN) {
        if (choice == "Single Player") {
            state = SINGLE;
            currentItems = &singleItems;
            selected = 0;
            return "";
        } else if (choice == "Multiplayer") {
            state = MULTIPLAYER;
            currentItems = &multiItems;
            selected = 0;
            return "";
        } else if (choice == "Mod") {
            std::cout << "Mod mode - reserved for future" << std::endl;
            return "";
        } else if (choice == "Settings") {
            std::cout << "Settings - reserved for future" << std::endl;
            return "";
        } else if (choice == "Exit") {
            return "exit";
        }
    } else if (state == SINGLE) {
        if (choice == "Endless Mode") {
            return "endless_mode";
        } else if (choice == "Level Mode") {
            state = LEVEL;
            currentItems = &levelItems;
            selected = 0;
            return "";
        } else if (choice == "Back") {
            goBack();
            return "";
        }
    } else if (state == LEVEL) {
        if (choice == "Load Game") {
            return "load_game";
        } else if (choice == "New Game") {
            return "new_game";
        } else if (choice == "Back") {
            goBack();
            return "";
        }
    } else if (state == MULTIPLAYER) {
        if (choice == "Local") {
            state = MODE;
            currentItems = &modeItems;
            selected = 0;
            return "";
        } else if (choice == "Lan") {
            std::cout << "Lan mode - reserved for future" << std::endl;
            return "";
        } else if (choice == "Online") {
            std::cout << "Online mode - reserved for future" << std::endl;
            return "";
        } else if (choice == "Back") {
            goBack();
            return "";
        }
    } else if (state == MODE) {
        if (choice == "PVP") {
            return "pvp";
        } else if (choice == "PVE") {
            return "pve";
        } else if (choice == "Back") {
            goBack();
            return "";
        }
    }
    return "";
}

void Menu::goBack() {
    if (state == SINGLE) {
        state = MAIN;
        currentItems = &mainItems;
    } else if (state == LEVEL) {
        state = SINGLE;
        currentItems = &singleItems;
    } else if (state == MULTIPLAYER) {
        state = MAIN;
        currentItems = &mainItems;
    } else if (state == MODE) {
        state = MULTIPLAYER;
        currentItems = &multiItems;
    }
    selected = 0;
}

void Menu::reset() {
    state = MAIN;
    currentItems = &mainItems;
    selected = 0;
}

void Menu::draw(sf::RenderWindow& window) {
    window.clear(COLOR_BG);

    sf::Text title(fontTitle, "Tank Battle", 72);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, 150.0f));
    window.draw(title);

    int yStart = 320;
    for (size_t i = 0; i < currentItems->size(); ++i) {
        sf::Text item(font, (*currentItems)[i], 36);
        item.setFillColor(i == (size_t)selected ? sf::Color::White : sf::Color(150, 150, 160));
        bounds = item.getLocalBounds();
        item.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, yStart + i * 55.0f));
        window.draw(item);
    }

    sf::Text hint(font, "", 14);
    if (state == SINGLE) {
        hint.setString("Select game mode");
    } else if (state == LEVEL) {
        hint.setString("Load saved progress or start fresh");
    } else if (state == MULTIPLAYER) {
        hint.setString("Select a multiplayer mode");
    } else if (state == MODE) {
        hint.setString("Select game mode");
    } else {
        hint.setString("Use UP/DOWN to navigate, ENTER to select, ESC to exit");
    }
    hint.setFillColor(sf::Color(100, 100, 120));
    hint.setPosition(sf::Vector2f(20.0f, WINDOW_HEIGHT - 40.0f));
    window.draw(hint);

    sf::Text ver(font, "v1.7", 14);
    ver.setFillColor(sf::Color(80, 80, 90));
    ver.setPosition(sf::Vector2f(WINDOW_WIDTH - 80.0f, WINDOW_HEIGHT - 30.0f));
    window.draw(ver);
}