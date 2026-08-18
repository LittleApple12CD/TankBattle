#include "SettingsMenu.h"
#include "Utils.h"
#include "SoundManager.h"
#include <iostream>

SettingsMenu::SettingsMenu() : selected(0), state(NAVIGATE), windowIndex(0) {
    bool loaded = font.openFromFile("assets/fonts/arial.ttf");
    if (!loaded) {
        loaded = font.openFromFile("assets/fonts/consola.ttf");
    }
    windowSizes = {{1200, 800}, {1600, 900}, {1920, 1080}};
    buildItems();
}

void SettingsMenu::buildItems() {
    items.clear();

    // 窗口大小 (索引 0)
    items.push_back({"Window Size", "", true, 0, ""});

    // 音量 (索引 1)
    items.push_back({"Sound Volume", "", true, 0, ""});

    // 分隔线 (索引 2)
    items.push_back({"--- P1 Controls ---", "", false, 0, ""});
    // P1 键位 (索引 3-7)
    items.push_back({"  Up", "", true, 1, "up"});
    items.push_back({"  Down", "", true, 1, "down"});
    items.push_back({"  Left", "", true, 1, "left"});
    items.push_back({"  Right", "", true, 1, "right"});
    items.push_back({"  Shoot", "", true, 1, "shoot"});

    // 分隔线 (索引 8)
    items.push_back({"--- P2 Controls ---", "", false, 0, ""});
    // P2 键位 (索引 9-13)
    items.push_back({"  Up", "", true, 2, "up"});
    items.push_back({"  Down", "", true, 2, "down"});
    items.push_back({"  Left", "", true, 2, "left"});
    items.push_back({"  Right", "", true, 2, "right"});
    items.push_back({"  Shoot", "", true, 2, "shoot"});

    // 空行 (索引 14)
    items.push_back({"", "", false, 0, ""});
    // Reset (索引 15)
    items.push_back({"Reset to Default", "", true, 0, ""});
    // Back (索引 16)
    items.push_back({"Back", "", true, 0, ""});

    updateItemValues();
}

void SettingsMenu::updateItemValues() {
    auto& cfg = ConfigManager::getInstance();

    // 窗口大小
    int w = cfg.getWindowWidth();
    int h = cfg.getWindowHeight();
    for (size_t i = 0; i < windowSizes.size(); ++i) {
        if (windowSizes[i].first == w && windowSizes[i].second == h) {
            windowIndex = static_cast<int>(i);
            break;
        }
    }
    items[0].value = std::to_string(w) + "x" + std::to_string(h);

    // 音量 (显示为 0-100)
    items[1].value = std::to_string(static_cast<int>(cfg.getVolume() * 100));

    // 键位
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].player > 0 && !items[i].action.empty()) {
            std::string key = cfg.getKey(items[i].player, items[i].action);
            items[i].value = getKeyDisplayName(key);
        }
    }
}

std::string SettingsMenu::getKeyDisplayName(const std::string& key) const {
    if (key == "Space") return "Space";
    if (key == "Up") return "Up";
    if (key == "Down") return "Down";
    if (key == "Left") return "Left";
    if (key == "Right") return "Right";
    if (key == "Escape") return "Esc";
    if (key == "Enter") return "Enter";
    if (key == "LControl") return "Ctrl";
    if (key == "LShift") return "Shift";
    if (key == "Unknown") return "?";
    if (key.length() == 1) return key;
    return key;
}

bool SettingsMenu::isKeyItem(int idx) const {
    // P1: 索引 3-7, P2: 索引 9-13
    return (idx >= 3 && idx <= 7) || (idx >= 9 && idx <= 13);
}

std::string SettingsMenu::handleInput(const sf::Event& event) {
    auto& cfg = ConfigManager::getInstance();

    // ===== 键位绑定状态 =====
    if (state == EDITING_KEY) {
        if (event.is<sf::Event::KeyPressed>()) {
            auto key = event.getIf<sf::Event::KeyPressed>()->code;
            
            // Escape 取消绑定
            if (key == sf::Keyboard::Key::Escape) {
                state = NAVIGATE;
                updateItemValues();
                return "";
            }
            
            std::string keyName;
            switch (key) {
                case sf::Keyboard::Key::Space: keyName = "Space"; break;
                case sf::Keyboard::Key::Up: keyName = "Up"; break;
                case sf::Keyboard::Key::Down: keyName = "Down"; break;
                case sf::Keyboard::Key::Left: keyName = "Left"; break;
                case sf::Keyboard::Key::Right: keyName = "Right"; break;
                case sf::Keyboard::Key::Escape: keyName = "Escape"; break;
                case sf::Keyboard::Key::Enter: keyName = "Enter"; break;
                case sf::Keyboard::Key::LControl: keyName = "LControl"; break;
                case sf::Keyboard::Key::LShift: keyName = "LShift"; break;
                default: {
                    if (key >= sf::Keyboard::Key::A && key <= sf::Keyboard::Key::Z) {
                        keyName = static_cast<char>(static_cast<int>(key) + 65);
                    } else if (key >= sf::Keyboard::Key::Num0 && key <= sf::Keyboard::Key::Num9) {
                        keyName = static_cast<char>(static_cast<int>(key) - 26 + 48);
                    } else {
                        // 不允许其他键
                        return "";
                    }
                }
            }
            
            cfg.setKey(bindingPlayer, bindingAction, keyName);
            state = NAVIGATE;
            updateItemValues();
            
            // 音量设置后立即生效
            SoundManager::getInstance().setVolume(cfg.getVolume());
            if (cfg.isMuted()) {
                SoundManager::getInstance().toggleMute();
            }
            
            return "";
        }
        return "";
    }

    // ===== 音量编辑状态 =====
    if (state == EDITING_VOLUME) {
        if (event.is<sf::Event::KeyPressed>()) {
            auto key = event.getIf<sf::Event::KeyPressed>()->code;
            
            if (key == sf::Keyboard::Key::Enter) {
                int vol = 80;
                try { vol = std::stoi(volumeInput); } catch (...) { vol = 80; }
                vol = std::max(0, std::min(100, vol));
                cfg.setVolume(vol / 100.0f);
                state = NAVIGATE;
                updateItemValues();
                
                // 音量立即生效
                SoundManager::getInstance().setVolume(cfg.getVolume());
                if (cfg.isMuted()) {
                    SoundManager::getInstance().toggleMute();
                }
                return "";
            }
            
            if (key == sf::Keyboard::Key::Escape) {
                state = NAVIGATE;
                volumeInput.clear();
                updateItemValues();
                return "";
            }
            
            // 数字输入
            if (key >= sf::Keyboard::Key::Num0 && key <= sf::Keyboard::Key::Num9) {
                char ch = static_cast<char>(static_cast<int>(key) - 26 + 48);
                if (volumeInput.length() < 3) {
                    volumeInput += ch;
                    items[1].value = volumeInput;
                }
                return "";
            }
            
            // 退格
            if (key == sf::Keyboard::Key::Backspace) {
                if (!volumeInput.empty()) {
                    volumeInput.pop_back();
                    items[1].value = volumeInput;
                }
                return "";
            }
        }
        return "";
    }

    // ===== 正常导航状态 =====
    if (event.is<sf::Event::KeyPressed>()) {
        auto key = event.getIf<sf::Event::KeyPressed>()->code;
        
        switch (key) {
            case sf::Keyboard::Key::Up: {
                int oldSelected = selected;
                do {
                    selected = (selected - 1 + items.size()) % items.size();
                } while (!items[selected].isSelectable && selected != oldSelected);
                break;
            }
            
            case sf::Keyboard::Key::Down: {
                int oldSelected = selected;
                do {
                    selected = (selected + 1) % items.size();
                } while (!items[selected].isSelectable && selected != oldSelected);
                break;
            }
            
            case sf::Keyboard::Key::Left: {
                if (selected == 0) {
                    windowIndex = (windowIndex - 1 + windowSizes.size()) % windowSizes.size();
                    cfg.setWindowSize(windowSizes[windowIndex].first, windowSizes[windowIndex].second);
                    updateItemValues();
                    // 窗口大小需要重启生效，提示用户
                }
                break;
            }
            
            case sf::Keyboard::Key::Right: {
                if (selected == 0) {
                    windowIndex = (windowIndex + 1) % windowSizes.size();
                    cfg.setWindowSize(windowSizes[windowIndex].first, windowSizes[windowIndex].second);
                    updateItemValues();
                }
                break;
            }
            
            case sf::Keyboard::Key::Enter: {
                // 音量编辑 (索引 1)
                if (selected == 1) {
                    state = EDITING_VOLUME;
                    volumeInput = items[1].value;
                    return "";
                }
                
                // 键位绑定
                if (isKeyItem(selected)) {
                    state = EDITING_KEY;
                    bindingPlayer = items[selected].player;
                    bindingAction = items[selected].action;
                    items[selected].value = "...";
                    return "";
                }
                
                // Reset to Default (索引 15)
                if (selected == 15) {
                    cfg.resetToDefault();
                    updateItemValues();
                    
                    // 音量立即生效
                    SoundManager::getInstance().setVolume(cfg.getVolume());
                    if (cfg.isMuted()) {
                        SoundManager::getInstance().toggleMute();
                    }
                    return "";
                }
                
                // Back (索引 16)
                if (selected == 16) {
                    return "back";
                }
                break;
            }
            
            case sf::Keyboard::Key::Escape: {
                return "back";
            }
            
            default: {
                // J 键和 Enter 冲突：只在非编辑状态下忽略 J
                // 但如果 J 在编辑键位时被按下，应该作为键位绑定
                break;
            }
        }
    }
    
    return "";
}

void SettingsMenu::draw(sf::RenderWindow& window) {
    window.clear(COLOR_BG);

    // 标题
    sf::Text title(font, "Settings", 56);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, 50.0f));
    window.draw(title);

    // ===== 列表 =====
    int yStart = 130;
    int yGap = 30;
    
    // 计算居中偏移：让标签和值整体居中
    int labelX = 600;   // 标签起始 X（原来是 250）
    int valueX = 1000;   // 值起始 X（原来是 620）

    for (size_t i = 0; i < items.size(); ++i) {
        // 分隔线特殊处理
        if (!items[i].isSelectable && !items[i].label.empty() && 
            items[i].label.find("---") != std::string::npos) {
            sf::Text label(font, items[i].label, 20);
            label.setFillColor(sf::Color(100, 100, 130));
            
            // 分隔线居中
            sf::FloatRect labelBounds = label.getLocalBounds();
            label.setPosition(sf::Vector2f((WINDOW_WIDTH - labelBounds.size.x) / 2.0f, yStart + i * yGap));
            window.draw(label);
            continue;
        }
        
        if (!items[i].isSelectable) {
            continue;
        }

        sf::Color textColor;
        if (i == static_cast<size_t>(selected)) {
            textColor = sf::Color::White;
        } else {
            textColor = sf::Color(180, 180, 190);
        }

        // 标签
        sf::Text label(font, items[i].label, 24);
        label.setFillColor(textColor);
        label.setPosition(sf::Vector2f(labelX, yStart + i * yGap));

        // 高亮选中
        if (i == static_cast<size_t>(selected) && state == NAVIGATE) {
            // 高亮框覆盖标签和值
            sf::RectangleShape highlight(sf::Vector2f(420, 32));
            highlight.setPosition(sf::Vector2f(labelX - 10, yStart + i * yGap - 4));
            highlight.setFillColor(sf::Color(60, 60, 80, 150));
            window.draw(highlight);
        }

        window.draw(label);

        // 值
        if (!items[i].value.empty()) {
            sf::Color valueColor;
            if (state == EDITING_KEY && i == static_cast<size_t>(selected)) {
                valueColor = sf::Color(255, 255, 100);
            } else if (state == EDITING_VOLUME && i == 1) {
                valueColor = sf::Color(255, 255, 100);
            } else if (i == static_cast<size_t>(selected)) {
                valueColor = sf::Color(100, 255, 150);
            } else {
                valueColor = sf::Color(200, 200, 200);
            }

            sf::Text val(font, items[i].value, 24);
            val.setFillColor(valueColor);
            val.setPosition(sf::Vector2f(valueX, yStart + i * yGap));
            window.draw(val);
        }
    }

    // 底部提示
    sf::Text hint(font, "UP/DOWN navigate  ENTER select  ESC back", 16);
    hint.setFillColor(sf::Color(100, 100, 120));
    bounds = hint.getLocalBounds();
    hint.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, WINDOW_HEIGHT - 40.0f));
    window.draw(hint);

    if (state == EDITING_VOLUME) {
        sf::Text editHint(font, "Enter number (0-100), ENTER to confirm, ESC to cancel", 18);
        editHint.setFillColor(sf::Color(255, 255, 100));
        bounds = editHint.getLocalBounds();
        editHint.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, WINDOW_HEIGHT - 80.0f));
        window.draw(editHint);
    }

    if (state == EDITING_KEY) {
        sf::Text editHint(font, "Press any key to bind, ESC to cancel", 18);
        editHint.setFillColor(sf::Color(255, 255, 100));
        bounds = editHint.getLocalBounds();
        editHint.setPosition(sf::Vector2f((WINDOW_WIDTH - bounds.size.x) / 2.0f, WINDOW_HEIGHT - 80.0f));
        window.draw(editHint);
    }

    // 底部版本
    sf::Text ver(font, "v1.8", 14);
    ver.setFillColor(sf::Color(80, 80, 90));
    ver.setPosition(sf::Vector2f(WINDOW_WIDTH - 80.0f, WINDOW_HEIGHT - 30.0f));
    window.draw(ver);
}

void SettingsMenu::reset() {
    selected = 0;
    state = NAVIGATE;
    volumeInput.clear();
    buildItems();
}