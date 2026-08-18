// src/Menu.cpp
#include "Menu.h"
#include "mod/ModLoader.h"
#include "resource/ResourcePackLoader.h"
#include "script/ScriptEngine.h"
#include "Game.h"
#include <iostream>

Menu::Menu() : state(MAIN), selected(0), modListScroll(0), modListSelected(0), showingModList(false) {
    mainItems = {"Single Player", "Multiplayer", "Mods", "Settings", "Exit"};
    singleItems = {"Endless Mode", "Level Mode", "Back"};
    levelItems = {"Load Game", "New Game", "Back"};
    multiItems = {"Local", "Lan", "Server", "Online", "Back"};
    modeItems = {"PVP", "PVE", "Back"};
    modItems = {"Reload Scripts", "Reload Resource Packs", "View Mod List", "Back"};
    currentItems = &mainItems;

    // 加载字体
    bool loaded = font.openFromFile("assets/fonts/arial.ttf");
    if (!loaded) {
        loaded = font.openFromFile("assets/fonts/consola.ttf");
    }
    fontTitle = font;
}

Menu::~Menu() {}

std::string Menu::handleInput(const sf::Event& event) {
    // ===== 模组列表视图 =====
    if (showingModList) {
        if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            return handleModListKey(*keyPressed);
        }
        return "";
    }

    // ===== 主菜单导航 =====
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

std::string Menu::handleModListKey(const sf::Event::KeyPressed& key) {
    auto& mods = ModLoader::getInstance().getMods();
    int totalItems = std::max((int)mods.size(), 1);

    switch (key.code) {
        case sf::Keyboard::Key::Up:
            if (modListSelected > 0) {
                modListSelected--;
                if (modListSelected < modListScroll) {
                    modListScroll = modListSelected;
                }
            }
            return "";

        case sf::Keyboard::Key::Down:
            if (modListSelected < totalItems - 1) {
                modListSelected++;
                if (modListSelected >= modListScroll + getVisibleModCount()) {
                    modListScroll = modListSelected - getVisibleModCount() + 1;
                }
            }
            return "";

        case sf::Keyboard::Key::Enter:
            if (modListSelected < (int)mods.size()) {
                ModLoader::getInstance().toggleMod(modListSelected);
                std::cout << "模组切换完成" << std::endl;
            }
            return "";

        case sf::Keyboard::Key::Escape:
        case sf::Keyboard::Key::Backspace:
            showingModList = false;
            modListScroll = 0;
            modListSelected = 0;
            return "";

        default:
            return "";
    }
}

int Menu::getVisibleModCount() const {
    return 12;  // 每页显示 12 个模组
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
        } else if (choice == "Mods") {
            state = MODS;
            currentItems = &modItems;
            selected = 0;
            return "";
        } else if (choice == "Settings") {
            return "settings";
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
        } else if (choice == "Server") {
            std::cout << "Server mode - reserved for future" << std::endl;
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
    } else if (state == MODS) {
        if (choice == "Reload Scripts") {
            return "reload_scripts";
        } else if (choice == "Reload Resource Packs") {
            return "reload_resourcepacks";
        } else if (choice == "View Mod List") {
            showingModList = true;
            modListScroll = 0;
            modListSelected = 0;
            return "";
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
    } else if (state == MODS) {
        state = MAIN;
        currentItems = &mainItems;
    }
    selected = 0;
}

void Menu::reset() {
    state = MAIN;
    currentItems = &mainItems;
    selected = 0;
    showingModList = false;
    modListScroll = 0;
    modListSelected = 0;
}

void Menu::draw(sf::RenderWindow& window) {
    window.clear(COLOR_BG);

    // ===== 模组列表视图 =====
    if (showingModList) {
        drawModList(window);
        return;
    }

    // ===== 标题 =====
    sf::Text title(fontTitle, "Tank Battle", 72);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, 150.0f));
    window.draw(title);

    // ===== 菜单项 =====
    int yStart = 320;
    for (size_t i = 0; i < currentItems->size(); ++i) {
        sf::Text item(font, (*currentItems)[i], 36);
        item.setFillColor(i == (size_t)selected ? sf::Color::White : sf::Color(150, 150, 160));
        bounds = item.getLocalBounds();
        item.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, yStart + i * 55.0f));
        window.draw(item);
    }

    // ===== 底部提示 =====
    sf::Text hint(font, "", 14);
    if (state == SINGLE) {
        hint.setString("Select game mode");
    } else if (state == LEVEL) {
        hint.setString("Load saved progress or start fresh");
    } else if (state == MULTIPLAYER) {
        hint.setString("Select a multiplayer mode");
    } else if (state == MODE) {
        hint.setString("Select game mode");
    } else if (state == MODS) {
        hint.setString("Manage mods, scripts and resource packs");
    } else {
        hint.setString("Use UP/DOWN to navigate, ENTER to select, ESC to exit");
    }
    hint.setFillColor(sf::Color(100, 100, 120));
    hint.setPosition(sf::Vector2f(20.0f, WINDOW_HEIGHT - 40.0f));
    window.draw(hint);

    // ===== 版本信息 =====
    sf::Text ver(font, "v1.9", 14);
    ver.setFillColor(sf::Color(80, 80, 90));
    ver.setPosition(sf::Vector2f(WINDOW_WIDTH - 80.0f, WINDOW_HEIGHT - 30.0f));
    window.draw(ver);

    // ===== 路径显示 =====
    std::string path = "";
    if (state == SINGLE || state == LEVEL) {
        path = "Single Player";
    }
    if (state == LEVEL) {
        path += " > Level Mode";
    }
    if (state == MULTIPLAYER || state == MODE) {
        path = "Multiplayer";
    }
    if (state == MODE) {
        path += " > Local";
    }
    if (state == MODS) {
        path = "Mods";
    }
    if (!path.empty()) {
        sf::Text pathText(font, path, 14);
        pathText.setFillColor(sf::Color(100, 100, 120));
        pathText.setPosition(sf::Vector2f(20.0f, 30.0f));
        window.draw(pathText);
    }
}

// ============================================================
// 绘制模组列表
// ============================================================

void Menu::drawModList(sf::RenderWindow& window) {
    // 背景
    window.clear(COLOR_BG);

    // 标题
    sf::Text title(fontTitle, "Mod List  (ENTER to toggle)", 48);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, 50.0f));
    window.draw(title);

    // 表头
    sf::Text header(font, "Status  Name                    Version      Game Version", 16);
    header.setFillColor(sf::Color(100, 100, 120));
    header.setPosition(sf::Vector2f(80.0f, 120.0f));
    window.draw(header);

    // 分隔线
    sf::RectangleShape line(sf::Vector2f(WINDOW_WIDTH - 160.0f, 1.0f));
    line.setPosition(sf::Vector2f(80.0f, 145.0f));
    line.setFillColor(sf::Color(60, 60, 70));
    window.draw(line);

    auto& mods = ModLoader::getInstance().getMods();
    int visibleCount = getVisibleModCount();
    int total = std::max((int)mods.size(), 1);

    int yStart = 160;
    int lineHeight = 28;

    for (int i = modListScroll; i < std::min(modListScroll + visibleCount, total); ++i) {
        int y = yStart + (i - modListScroll) * lineHeight;
        bool isSelected = (i == modListSelected);

        if (i < (int)mods.size()) {
            const auto& info = mods[i];

            bool isLoaded = info.isLoaded();
            bool isDisabled = info.isDisabled();
            bool hasError = info.hasError();

            std::string status = isLoaded ? "[X]" : "[ ]";
            std::string name = info.name;
            std::string version = info.version;
            std::string gameVer = info.gameVersion;

            if (name.length() > 24) {
                name = name.substr(0, 22) + "..";
            }

            sf::Color statusColor;
            if (isLoaded) {
                statusColor = sf::Color(0, 200, 80);
            } else if (isDisabled) {
                statusColor = sf::Color(130, 130, 130);
            } else if (hasError) {
                statusColor = sf::Color(255, 150, 50);
            } else {
                statusColor = sf::Color(200, 50, 50);
            }

            sf::Text statusText(font, status, 16);
            statusText.setFillColor(statusColor);
            statusText.setPosition(sf::Vector2f(80.0f, y));
            window.draw(statusText);

            sf::Color nameColor;
            if (isSelected) {
                nameColor = sf::Color::White;
            } else if (isDisabled) {
                nameColor = sf::Color(130, 130, 130);
            } else {
                nameColor = sf::Color(200, 200, 200);
            }

            sf::Text nameText(font, name, 16);
            if (isSelected) nameText.setStyle(sf::Text::Bold);
            nameText.setFillColor(nameColor);
            nameText.setPosition(sf::Vector2f(170.0f, y));
            window.draw(nameText);

            sf::Text versionText(font, version, 16);
            versionText.setFillColor(isSelected ? sf::Color(255, 255, 100) : sf::Color(150, 150, 160));
            versionText.setPosition(sf::Vector2f(450.0f, y));
            window.draw(versionText);

            sf::Text gameVerText(font, gameVer, 16);
            gameVerText.setFillColor(sf::Color(130, 130, 140));
            gameVerText.setPosition(sf::Vector2f(580.0f, y));
            window.draw(gameVerText);

            if (info.hasError()) {
                std::string err = info.error;
                if (isDisabled) {
                    err = "Disabled";
                }
                if (err.length() > 30) {
                    err = err.substr(0, 28) + "..";
                }
                sf::Text errText(font, err, 13);
                errText.setFillColor(isDisabled ? sf::Color(130, 130, 130) : sf::Color(255, 150, 50));
                errText.setPosition(sf::Vector2f(700.0f, y));
                window.draw(errText);
            }

            if (isSelected) {
                sf::RectangleShape highlight(sf::Vector2f(WINDOW_WIDTH - 160.0f, lineHeight - 4));
                highlight.setPosition(sf::Vector2f(80.0f, y - lineHeight + 6));
                highlight.setFillColor(sf::Color(60, 60, 80, 150));
                window.draw(highlight);
            }

        } else {
            sf::Text emptyText(font, "No mods found. Place DLL/SO files in mods/", 16);
            emptyText.setFillColor(sf::Color(100, 100, 120));
            emptyText.setPosition(sf::Vector2f(80.0f, y));
            window.draw(emptyText);
        }
    }

    std::string countText = "Total: " + std::to_string(mods.size()) + " mods";
    if ((int)mods.size() > visibleCount) {
        int totalPages = ((int)mods.size() - 1) / visibleCount + 1;
        int currentPage = modListScroll / visibleCount + 1;
        countText += " (Page " + std::to_string(currentPage) + "/" + std::to_string(totalPages) + ")";
    }

    sf::Text countInfo(font, countText, 14);
    countInfo.setFillColor(sf::Color(100, 100, 120));
    bounds = countInfo.getLocalBounds();
    countInfo.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, WINDOW_HEIGHT - 30.0f));
    window.draw(countInfo);

    sf::Text hint(font, "[X] loaded  [ ] unloaded  [ ] disabled  ENTER toggle  UP/DOWN scroll  ESC back", 14);
    hint.setFillColor(sf::Color(80, 80, 90));
    bounds = hint.getLocalBounds();
    hint.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, WINDOW_HEIGHT - 60.0f));
    window.draw(hint);
}