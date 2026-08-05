#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ConfigManager.h"

class SettingsMenu {
public:
    SettingsMenu();
    ~SettingsMenu() = default;

    std::string handleInput(const sf::Event& event);
    void draw(sf::RenderWindow& window);
    void reset();

private:
    enum MenuState {
        NAVIGATE,       // 正常导航
        EDITING_VOLUME, // 编辑音量（输入数字）
        EDITING_KEY     // 等待按键绑定
    };

    struct MenuItem {
        std::string label;
        std::string value;
        bool isSelectable;
        int player;         // 1 or 2
        std::string action; // "up", "down", "left", "right", "shoot"
    };

    std::vector<MenuItem> items;
    int selected;
    MenuState state;
    sf::Font font;

    // 音量编辑缓存
    std::string volumeInput;

    // 窗口大小选项
    std::vector<std::pair<int, int>> windowSizes;
    int windowIndex;

    // 键位绑定等待
    int bindingPlayer;
    std::string bindingAction;

    void buildItems();
    void updateItemValues();
    void selectCurrent();
    std::string getKeyDisplayName(const std::string& key) const;
    bool isKeyItem(int idx) const;
};