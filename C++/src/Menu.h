#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Utils.h"

class Menu {
public:
    Menu();
    ~Menu();

    std::string handleInput(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void reset();

private:
    enum MenuState {
        MAIN,
        MULTIPLAYER,
        MODE
    };

    std::vector<std::string> mainItems;
    std::vector<std::string> multiItems;
    std::vector<std::string> modeItems;
    std::vector<std::string>* currentItems;
    MenuState state;
    int selected;

    sf::Font font;
    sf::Font fontTitle;

    std::string selectCurrent();
    void goBack();
};