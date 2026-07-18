#include "Menu.h"
#include <iostream>

Menu::Menu() : state(MAIN), selected(0) {
    mainItems = {"Single Player", "Multiplayer", "Mod", "Settings", "Exit"};
    multiItems = {"Local", "Lan", "Online", "Back"};
    modeItems = {"PVP", "PVE", "Back"};
    currentItems = &mainItems;

    // SFML 3.0: openFromFile 代替 loadFromFile
    if (!font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        font.openFromFile("C:/Windows/Fonts/consola.ttf");
    }
    fontTitle = font;
}

Menu::~Menu() {}

std::string Menu::handleInput(const sf::Event& event) {
    // SFML 3.0: 用 is<KeyPressed>() 和 getIf<KeyPressed>()
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
            return "single_player";
        } else if (choice == "Multiplayer") {
            state = MULTIPLAYER;
            currentItems = &multiItems;
            selected = 0;
        } else if (choice == "Mod") {
            std::cout << "Mod mode - reserved for future" << std::endl;
        } else if (choice == "Exit") {
            return "exit";
        }
    } else if (state == MULTIPLAYER) {
        if (choice == "Local") {
            state = MODE;
            currentItems = &modeItems;
            selected = 0;
        } else if (choice == "Lan (Reserved)") {
            std::cout << "Lan mode - reserved for future" << std::endl;
        } else if (choice == "Online (Reserved)") {
            std::cout << "Online mode - reserved for future" << std::endl;
        } else if (choice == "Back") {
            goBack();
        }
    } else if (state == MODE) {
        if (choice == "PVP") {
            return "pvp";
        } else if (choice == "PVE") {
            return "pve";
        } else if (choice == "Back") {
            goBack();
        }
    }
    return "";
}

void Menu::goBack() {
    if (state == MULTIPLAYER) {
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

    // SFML 3.0: Text 构造函数参数顺序: (font, string, characterSize)
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
    if (state == MULTIPLAYER) {
        hint.setString("Select a multiplayer mode");
    } else if (state == MODE) {
        hint.setString("Select game mode");
    } else {
        hint.setString("Use UP/DOWN to navigate, ENTER to select, ESC to exit");
    }
    hint.setFillColor(sf::Color(100, 100, 120));
    hint.setPosition(sf::Vector2f(20.0f, WINDOW_HEIGHT - 40.0f));
    window.draw(hint);

    sf::Text ver(font, "v1.4", 14);
    ver.setFillColor(sf::Color(80, 80, 90));
    ver.setPosition(sf::Vector2f(WINDOW_WIDTH - 80.0f, WINDOW_HEIGHT - 30.0f));
    window.draw(ver);
}