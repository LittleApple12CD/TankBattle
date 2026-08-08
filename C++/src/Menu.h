// src/Menu.h
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
        MODE,
        SINGLE,
        LEVEL,
        MODS,
        MOD_LIST
    };

    std::vector<std::string> mainItems;
    std::vector<std::string> multiItems;
    std::vector<std::string> singleItems;
    std::vector<std::string> levelItems;
    std::vector<std::string> modeItems;
    std::vector<std::string> modItems;
    std::vector<std::string>* currentItems;
    MenuState state;
    int selected;

    int modListScroll;
    int modListSelected;
    bool showingModList;

    sf::Font font;
    sf::Font fontTitle;

    std::string selectCurrent();
    void goBack();
    std::string handleModListKey(const sf::Event::KeyPressed& key);
    int getVisibleModCount() const;
    void drawModList(sf::RenderWindow& window);
};