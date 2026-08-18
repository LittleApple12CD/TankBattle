package com.tankbattle;

import java.awt.*;
import java.awt.event.KeyEvent;

import static com.tankbattle.Utils.*;

/**
 * 设置界面 v1.8 - 修复方向键乱码
 */
public class SettingsMenu {

    // ===== 菜单状态 =====
    private enum State {
        NAVIGATING,     // 正常浏览
        BINDING_P1_UP, BINDING_P1_DOWN, BINDING_P1_LEFT, BINDING_P1_RIGHT, BINDING_P1_SHOOT,
        BINDING_P2_UP, BINDING_P2_DOWN, BINDING_P2_LEFT, BINDING_P2_RIGHT, BINDING_P2_SHOOT,
        BINDING_VOLUME  // 音量输入
    }

    private State state = State.NAVIGATING;
    private int selectedIndex = 0;
    private String volumeInput = "";

    // ===== 菜单项 =====
    private static final int ITEM_WINDOW = 0;
    private static final int ITEM_VOLUME = 1;
    private static final int ITEM_P1_UP = 2;
    private static final int ITEM_P1_DOWN = 3;
    private static final int ITEM_P1_LEFT = 4;
    private static final int ITEM_P1_RIGHT = 5;
    private static final int ITEM_P1_SHOOT = 6;
    private static final int ITEM_P2_UP = 7;
    private static final int ITEM_P2_DOWN = 8;
    private static final int ITEM_P2_LEFT = 9;
    private static final int ITEM_P2_RIGHT = 10;
    private static final int ITEM_P2_SHOOT = 11;
    private static final int ITEM_RESET = 12;
    private static final int ITEM_BACK = 13;

    private static final int TOTAL_ITEMS = 14;

    private String[] labels = {
        "Window Size",
        "Sound Volume",
        "P1 Up",
        "P1 Down",
        "P1 Left",
        "P1 Right",
        "P1 Shoot",
        "P2 Up",
        "P2 Down",
        "P2 Left",
        "P2 Right",
        "P2 Shoot",
        "Reset to Default",
        "Back"
    };

    private Font fontNormal;
    private Font fontHighlight;
    private Font fontSmall;

    public SettingsMenu() {
        fontNormal = loadFont("consola.ttf", 28, Font.PLAIN);
        fontHighlight = loadFont("consola.ttf", 28, Font.BOLD);
        fontSmall = loadFont("consola.ttf", 18, Font.PLAIN);
    }

    public String handleKeyPress(int keyCode) {
        switch (state) {
            case NAVIGATING:
                return handleNavigation(keyCode);

            case BINDING_VOLUME:
                return handleVolumeInput(keyCode);

            case BINDING_P1_UP:
            case BINDING_P1_DOWN:
            case BINDING_P1_LEFT:
            case BINDING_P1_RIGHT:
            case BINDING_P1_SHOOT:
            case BINDING_P2_UP:
            case BINDING_P2_DOWN:
            case BINDING_P2_LEFT:
            case BINDING_P2_RIGHT:
            case BINDING_P2_SHOOT:
                return handleKeyBinding(keyCode);

            default:
                return null;
        }
    }

    private String handleNavigation(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_UP:
                selectedIndex = (selectedIndex - 1 + TOTAL_ITEMS) % TOTAL_ITEMS;
                return null;

            case KeyEvent.VK_DOWN:
                selectedIndex = (selectedIndex + 1) % TOTAL_ITEMS;
                return null;

            case KeyEvent.VK_LEFT:
                if (selectedIndex == ITEM_WINDOW) {
                    Settings.prevWindowSize();
                    Settings.save();
                    return "resize";
                }
                if (selectedIndex == ITEM_VOLUME) {
                    Settings.SOUND_VOLUME = Math.max(0, Settings.SOUND_VOLUME - 0.05f);
                    Settings.SOUND_VOLUME = Math.round(Settings.SOUND_VOLUME * 100) / 100.0f;
                    Settings.save();
                    SoundManager.getInstance().setVolume(Settings.SOUND_VOLUME);
                }
                return null;

            case KeyEvent.VK_RIGHT:
                if (selectedIndex == ITEM_WINDOW) {
                    Settings.nextWindowSize();
                    Settings.save();
                    return "resize";
                }
                if (selectedIndex == ITEM_VOLUME) {
                    Settings.SOUND_VOLUME = Math.min(1.0f, Settings.SOUND_VOLUME + 0.05f);
                    Settings.SOUND_VOLUME = Math.round(Settings.SOUND_VOLUME * 100) / 100.0f;
                    Settings.save();
                    SoundManager.getInstance().setVolume(Settings.SOUND_VOLUME);
                }
                return null;

            case KeyEvent.VK_ENTER:
                return selectCurrent();

            case KeyEvent.VK_ESCAPE:
                Settings.save();
                return "back";

            default:
                return null;
        }
    }

    private String selectCurrent() {
        if (selectedIndex == ITEM_WINDOW) {
            Settings.nextWindowSize();
            Settings.save();
            return "resize";
        }

        if (selectedIndex == ITEM_VOLUME) {
            state = State.BINDING_VOLUME;
            volumeInput = String.valueOf(Settings.SOUND_VOLUME);
            return null;
        }

        if (selectedIndex == ITEM_RESET) {
            Settings.resetToDefaults();
            SoundManager.getInstance().setVolume(Settings.SOUND_VOLUME);
            return "resize";
        }

        if (selectedIndex == ITEM_BACK) {
            Settings.save();
            return "back";
        }

        // 按键绑定
        switch (selectedIndex) {
            case ITEM_P1_UP: state = State.BINDING_P1_UP; return null;
            case ITEM_P1_DOWN: state = State.BINDING_P1_DOWN; return null;
            case ITEM_P1_LEFT: state = State.BINDING_P1_LEFT; return null;
            case ITEM_P1_RIGHT: state = State.BINDING_P1_RIGHT; return null;
            case ITEM_P1_SHOOT: state = State.BINDING_P1_SHOOT; return null;
            case ITEM_P2_UP: state = State.BINDING_P2_UP; return null;
            case ITEM_P2_DOWN: state = State.BINDING_P2_DOWN; return null;
            case ITEM_P2_LEFT: state = State.BINDING_P2_LEFT; return null;
            case ITEM_P2_RIGHT: state = State.BINDING_P2_RIGHT; return null;
            case ITEM_P2_SHOOT: state = State.BINDING_P2_SHOOT; return null;
        }

        return null;
    }

    private String handleVolumeInput(int keyCode) {
        if (keyCode == KeyEvent.VK_ESCAPE) {
            state = State.NAVIGATING;
            volumeInput = "";
            return null;
        }

        if (keyCode == KeyEvent.VK_ENTER) {
            try {
                float vol = Float.parseFloat(volumeInput);
                vol = Math.max(0, Math.min(1.0f, vol));
                Settings.SOUND_VOLUME = vol;
                Settings.save();
                SoundManager.getInstance().setVolume(vol);
            } catch (NumberFormatException e) {
                // 输入无效，保持原值
            }
            state = State.NAVIGATING;
            volumeInput = "";
            return null;
        }

        // 数字输入 (0-9, .)
        char c = (char) keyCode;
        if (Character.isDigit(c) || c == '.') {
            if (volumeInput.length() < 6) {
                volumeInput += c;
            }
        } else if (keyCode == KeyEvent.VK_BACK_SPACE) {
            if (volumeInput.length() > 0) {
                volumeInput = volumeInput.substring(0, volumeInput.length() - 1);
            }
        }

        return null;
    }

    private String handleKeyBinding(int keyCode) {
        // ESC 取消绑定
        if (keyCode == KeyEvent.VK_ESCAPE) {
            state = State.NAVIGATING;
            return null;
        }

        // 禁止绑定特殊键
        if (keyCode == KeyEvent.VK_ENTER || 
            keyCode == KeyEvent.VK_ESCAPE) {
            return null;
        }

        // 保存按键
        switch (state) {
            case BINDING_P1_UP: Settings.KEY_P1_UP = keyCode; break;
            case BINDING_P1_DOWN: Settings.KEY_P1_DOWN = keyCode; break;
            case BINDING_P1_LEFT: Settings.KEY_P1_LEFT = keyCode; break;
            case BINDING_P1_RIGHT: Settings.KEY_P1_RIGHT = keyCode; break;
            case BINDING_P1_SHOOT: Settings.KEY_P1_SHOOT = keyCode; break;
            case BINDING_P2_UP: Settings.KEY_P2_UP = keyCode; break;
            case BINDING_P2_DOWN: Settings.KEY_P2_DOWN = keyCode; break;
            case BINDING_P2_LEFT: Settings.KEY_P2_LEFT = keyCode; break;
            case BINDING_P2_RIGHT: Settings.KEY_P2_RIGHT = keyCode; break;
            case BINDING_P2_SHOOT: Settings.KEY_P2_SHOOT = keyCode; break;
            default: return null;
        }

        Settings.save();
        state = State.NAVIGATING;
        return null;
    }

    public void resetState() {
        state = State.NAVIGATING;
        selectedIndex = 0;
        volumeInput = "";
    }

    // ============================================================
    // 自定义按键名称（修复方向键乱码）
    // ============================================================
    private String getCustomKeyName(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_UP: return "↑";
            case KeyEvent.VK_DOWN: return "↓";
            case KeyEvent.VK_LEFT: return "←";
            case KeyEvent.VK_RIGHT: return "→";
            case KeyEvent.VK_SPACE: return "Space";
            case KeyEvent.VK_ENTER: return "Enter";
            case KeyEvent.VK_ESCAPE: return "Esc";
            case KeyEvent.VK_TAB: return "Tab";
            case KeyEvent.VK_BACK_SPACE: return "Backspace";
            case KeyEvent.VK_CONTROL: return "Ctrl";
            case KeyEvent.VK_SHIFT: return "Shift";
            case KeyEvent.VK_ALT: return "Alt";
            case KeyEvent.VK_WINDOWS: return "Win";
            case KeyEvent.VK_DELETE: return "Delete";
            case KeyEvent.VK_HOME: return "Home";
            case KeyEvent.VK_END: return "End";
            case KeyEvent.VK_PAGE_UP: return "PageUp";
            case KeyEvent.VK_PAGE_DOWN: return "PageDown";
            case KeyEvent.VK_INSERT: return "Insert";
            case KeyEvent.VK_NUMPAD0: return "Num0";
            case KeyEvent.VK_NUMPAD1: return "Num1";
            case KeyEvent.VK_NUMPAD2: return "Num2";
            case KeyEvent.VK_NUMPAD3: return "Num3";
            case KeyEvent.VK_NUMPAD4: return "Num4";
            case KeyEvent.VK_NUMPAD5: return "Num5";
            case KeyEvent.VK_NUMPAD6: return "Num6";
            case KeyEvent.VK_NUMPAD7: return "Num7";
            case KeyEvent.VK_NUMPAD8: return "Num8";
            case KeyEvent.VK_NUMPAD9: return "Num9";
            case KeyEvent.VK_MULTIPLY: return "Num*";
            case KeyEvent.VK_ADD: return "Num+";
            case KeyEvent.VK_SUBTRACT: return "Num-";
            case KeyEvent.VK_DIVIDE: return "Num/";
            case KeyEvent.VK_DECIMAL: return "Num.";
            case KeyEvent.VK_F1: return "F1";
            case KeyEvent.VK_F2: return "F2";
            case KeyEvent.VK_F3: return "F3";
            case KeyEvent.VK_F4: return "F4";
            case KeyEvent.VK_F5: return "F5";
            case KeyEvent.VK_F6: return "F6";
            case KeyEvent.VK_F7: return "F7";
            case KeyEvent.VK_F8: return "F8";
            case KeyEvent.VK_F9: return "F9";
            case KeyEvent.VK_F10: return "F10";
            case KeyEvent.VK_F11: return "F11";
            case KeyEvent.VK_F12: return "F12";
            default:
                // 字母和数字
                if (keyCode >= KeyEvent.VK_A && keyCode <= KeyEvent.VK_Z) {
                    return String.valueOf((char) keyCode);
                }
                if (keyCode >= KeyEvent.VK_0 && keyCode <= KeyEvent.VK_9) {
                    return String.valueOf((char) keyCode);
                }
                // 其他键使用 KeyEvent.getKeyText 作为备选
                String text = KeyEvent.getKeyText(keyCode);
                // 过滤掉中文
                if (text.matches("[\\u4e00-\\u9fa5]+")) {
                    return "Key_" + keyCode;
                }
                return text;
        }
    }

    // ===== 绘制 =====
    public void draw(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        g.setColor(new Color(20, 20, 30));
        g.fillRect(0, 0, Utils.WINDOW_WIDTH, Utils.WINDOW_HEIGHT);

        // 标题
        g.setColor(Color.WHITE);
        g.setFont(new Font("Consolas", Font.BOLD, 48));
        String title = "Settings";
        FontMetrics fm = g.getFontMetrics();
        g.drawString(title, (Utils.WINDOW_WIDTH - fm.stringWidth(title)) / 2, 80);

        // 菜单项
        int yStart = 160;
        int lineHeight = 40;
        int leftCol = Utils.WINDOW_WIDTH / 2 - 300;
        int rightCol = Utils.WINDOW_WIDTH / 2 + 50;

        g.setFont(fontNormal);

        for (int i = 0; i < labels.length; i++) {
            int y = yStart + i * lineHeight;
            boolean selected = (i == selectedIndex && state == State.NAVIGATING);

            // 标签
            g.setColor(selected ? Color.WHITE : new Color(180, 180, 190));
            if (selected) g.setFont(fontHighlight);
            else g.setFont(fontNormal);
            g.drawString(labels[i], leftCol, y);

            // 使用自定义按键名称
            String value = getValue(i);
            if (selected) g.setFont(fontHighlight);
            else g.setFont(fontNormal);
            g.setColor(selected ? new Color(255, 255, 100) : new Color(150, 150, 160));
            g.drawString(value, rightCol, y);

            // 按键绑定提示
            if (i >= ITEM_P1_UP && i <= ITEM_P2_SHOOT && state == State.NAVIGATING) {
                g.setColor(new Color(80, 80, 90));
                g.setFont(fontSmall);
                g.drawString("[ENTER to bind]", rightCol + 180, y);
            }
        }

        // ===== 绑定状态提示 =====
        if (state != State.NAVIGATING) {
            g.setColor(new Color(0, 0, 0, 180));
            g.fillRect(0, 0, Utils.WINDOW_WIDTH, Utils.WINDOW_HEIGHT);

            g.setColor(Color.WHITE);
            g.setFont(new Font("Consolas", Font.BOLD, 36));

            String msg;
            if (state == State.BINDING_VOLUME) {
                msg = "Enter Volume (0.0 - 1.0): " + volumeInput + "_";
            } else {
                String keyName = getBindLabel();
                msg = "Press any key for: " + keyName + " (ESC to cancel)";
            }
            fm = g.getFontMetrics();
            g.drawString(msg, (Utils.WINDOW_WIDTH - fm.stringWidth(msg)) / 2, Utils.WINDOW_HEIGHT / 2);
        }

        // 底部提示
        g.setColor(new Color(100, 100, 120));
        g.setFont(new Font("Consolas", Font.PLAIN, 16));
        String hint;
        if (state == State.NAVIGATING) {
            hint = "UP/DOWN: navigate  LEFT/RIGHT: adjust  ENTER: select/bind  ESC: save & exit";
        } else {
            hint = "Press a key to bind  |  ESC to cancel";
        }
        fm = g.getFontMetrics();
        g.drawString(hint, (Utils.WINDOW_WIDTH - fm.stringWidth(hint)) / 2, Utils.WINDOW_HEIGHT - 30);
    }

    // 使用自定义按键名称
    private String getValue(int index) {
        switch (index) {
            case ITEM_WINDOW:
                return Settings.WINDOW_WIDTH + "x" + Settings.WINDOW_HEIGHT;
            case ITEM_VOLUME:
                if (state == State.BINDING_VOLUME) return volumeInput + "_";
                return String.format("%.2f", Settings.SOUND_VOLUME);
            case ITEM_P1_UP: return getCustomKeyName(Settings.KEY_P1_UP);
            case ITEM_P1_DOWN: return getCustomKeyName(Settings.KEY_P1_DOWN);
            case ITEM_P1_LEFT: return getCustomKeyName(Settings.KEY_P1_LEFT);
            case ITEM_P1_RIGHT: return getCustomKeyName(Settings.KEY_P1_RIGHT);
            case ITEM_P1_SHOOT: return getCustomKeyName(Settings.KEY_P1_SHOOT);
            case ITEM_P2_UP: return getCustomKeyName(Settings.KEY_P2_UP);
            case ITEM_P2_DOWN: return getCustomKeyName(Settings.KEY_P2_DOWN);
            case ITEM_P2_LEFT: return getCustomKeyName(Settings.KEY_P2_LEFT);
            case ITEM_P2_RIGHT: return getCustomKeyName(Settings.KEY_P2_RIGHT);
            case ITEM_P2_SHOOT: return getCustomKeyName(Settings.KEY_P2_SHOOT);
            case ITEM_RESET: return "";
            case ITEM_BACK: return "";
            default: return "";
        }
    }

    private String getBindLabel() {
        switch (state) {
            case BINDING_P1_UP: return "P1 Up";
            case BINDING_P1_DOWN: return "P1 Down";
            case BINDING_P1_LEFT: return "P1 Left";
            case BINDING_P1_RIGHT: return "P1 Right";
            case BINDING_P1_SHOOT: return "P1 Shoot";
            case BINDING_P2_UP: return "P2 Up";
            case BINDING_P2_DOWN: return "P2 Down";
            case BINDING_P2_LEFT: return "P2 Left";
            case BINDING_P2_RIGHT: return "P2 Right";
            case BINDING_P2_SHOOT: return "P2 Shoot";
            default: return "";
        }
    }

    public boolean isNavigating() {
        return state == State.NAVIGATING;
    }
}