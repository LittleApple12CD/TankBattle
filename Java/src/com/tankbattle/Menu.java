package com.tankbattle;

import java.awt.*;
import java.awt.event.KeyEvent;
import static com.tankbattle.Utils.*;

public class Menu {

    public void setMainItem(int index, String text) {
        if (index >= 0 && index < mainItems.length) {
            mainItems[index] = text;
        }
    }

    public String getMainItem(int index) {
        if (index >= 0 && index < mainItems.length) {
            return mainItems[index];
        }
        return null;
    }

    public String[] getMainItems() {
        return mainItems.clone();
    }

    public void setCurrentItems(String[] items) {
        this.currentItems = items;
        this.selected = 0;
    }

    public String getCurrentState() {
        return state;
    }

    public void setState(String state) {
        this.state = state;
    }

    public int getSelected() {
        return selected;
    }

    public void setSelected(int index) {
        if (index >= 0 && index < currentItems.length) {
            this.selected = index;
        }
    }

    private String[] mainItems = {"Single Player", "Multiplayer", "Mods", "Settings", "Exit"};
    private String[] singleItems = {"Endless Mode", "Level Mode", "Back"};
    private String[] levelItems = {"Load Game", "New Game", "Back"};
    private String[] multiItems = {"Local", "Lan", "Server", "Online", "Back"};
    private String[] modeItems = {"PVP", "PVE", "Back"};
    private String[] modItems = {"Reload Scripts", "Reload Resource Packs", "View Mod List", "Back"};

    private String[] currentItems = mainItems;
    private int selected = 0;
    private String state = "main";

    private int modListScroll = 0;
    private int modListSelected = 0;
    private boolean showingModList = false;

    public Menu() {}

    public String handleKeyPress(int keyCode, Game game) {
        if (showingModList) {
            return handleModListKey(keyCode);
        }

        switch (keyCode) {
            case KeyEvent.VK_UP:
                selected = (selected - 1 + currentItems.length) % currentItems.length;
                return null;
            case KeyEvent.VK_DOWN:
                selected = (selected + 1) % currentItems.length;
                return null;
            case KeyEvent.VK_ENTER:
                return selectCurrent(game);
            case KeyEvent.VK_ESCAPE:
                if (state.equals("main")) {
                    System.exit(0);
                } else {
                    goBack();
                }
                return null;
            default:
                return null;
        }
    }

    private String handleModListKey(int keyCode) {
        java.util.List<com.tankbattle.mod.ModInfo> mods = com.tankbattle.mod.ModLoader.getMods();
        int totalItems = Math.max(mods.size(), 1);

        switch (keyCode) {
            case KeyEvent.VK_UP:
                if (modListSelected > 0) {
                    modListSelected--;
                    if (modListSelected < modListScroll) {
                        modListScroll = modListSelected;
                    }
                }
                return null;
            case KeyEvent.VK_DOWN:
                if (modListSelected < totalItems - 1) {
                    modListSelected++;
                    if (modListSelected >= modListScroll + getVisibleModCount()) {
                        modListScroll = modListSelected - getVisibleModCount() + 1;
                    }
                }
                return null;
            case KeyEvent.VK_ENTER:
                if (modListSelected < mods.size()) {
                    com.tankbattle.mod.ModLoader.toggleMod(modListSelected);
                    return null;
                }
                return null;
            case KeyEvent.VK_ESCAPE:
            case KeyEvent.VK_BACK_SPACE:
                showingModList = false;
                modListScroll = 0;
                modListSelected = 0;
                return null;
            default:
                return null;
        }
    }

    private int getVisibleModCount() {
        return 12;
    }

    private String selectCurrent(Game game) {
        String choice = currentItems[selected];

        if (state.equals("main")) {
            if (choice.equals("Single Player")) {
                state = "single";
                currentItems = singleItems;
                selected = 0;
                return null;
            } else if (choice.equals("Multiplayer")) {
                state = "multiplayer";
                currentItems = multiItems;
                selected = 0;
                return null;
            } else if (choice.equals("Mods")) {
                state = "mods";
                currentItems = modItems;
                selected = 0;
                return null;
            } else if (choice.equals("Settings")) {
                return "settings";
            } else if (choice.equals("Exit")) {
                System.exit(0);
            }
        } else if (state.equals("single")) {
            if (choice.equals("Endless Mode")) {
                return "endless_mode";
            } else if (choice.equals("Level Mode")) {
                state = "level";
                currentItems = levelItems;
                selected = 0;
                return null;
            } else if (choice.equals("Back")) {
                goBack();
                return null;
            }
        } else if (state.equals("level")) {
            if (choice.equals("Load Game")) {
                return "load_game";
            } else if (choice.equals("New Game")) {
                return "new_game";
            } else if (choice.equals("Back")) {
                goBack();
                return null;
            }
        } else if (state.equals("multiplayer")) {
            if (choice.equals("Local")) {
                state = "mode";
                currentItems = modeItems;
                selected = 0;
                return null;
            } else if (choice.equals("Lan")) {
                System.out.println("Lan mode - reserved for future");
                return null;
            } else if (choice.equals("Online")) {
                System.out.println("Online mode - reserved for future");
                return null;
            } else if (choice.equals("Back")) {
                goBack();
                return null;
            }
        } else if (state.equals("mode")) {
            if (choice.equals("PVP")) {
                return "pvp";
            } else if (choice.equals("PVE")) {
                return "pve";
            } else if (choice.equals("Back")) {
                goBack();
                return null;
            }
        } else if (state.equals("mods")) {
            if (choice.equals("Reload Scripts")) {
                com.tankbattle.script.ScriptEngine.reloadAll();
                return null;
            } else if (choice.equals("Reload Resource Packs")) {
                com.tankbattle.resource.ResourcePackLoader.init(game);
                return null;
            } else if (choice.equals("View Mod List")) {
                showingModList = true;
                modListScroll = 0;
                modListSelected = 0;
                return null;
            } else if (choice.equals("Back")) {
                goBack();
                return null;
            }
        }

        return null;
    }

    private void goBack() {
        if (state.equals("single")) {
            state = "main";
            currentItems = mainItems;
        } else if (state.equals("level")) {
            state = "single";
            currentItems = singleItems;
        } else if (state.equals("multiplayer")) {
            state = "main";
            currentItems = multiItems;
        } else if (state.equals("mode")) {
            state = "multiplayer";
            currentItems = multiItems;
        } else if (state.equals("mods")) {
            state = "main";
            currentItems = mainItems;
        }
        selected = 0;
    }

    public void resetState() {
        state = "main";
        currentItems = mainItems;
        selected = 0;
        showingModList = false;
        modListScroll = 0;
        modListSelected = 0;
    }

    public void draw(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        g.setColor(new Color(20, 20, 30));
        g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        if (showingModList) {
            drawModList(g);
            return;
        }

        g.setColor(Color.WHITE);
        g.setFont(new Font("Consolas", Font.BOLD, 72));
        String title = "Tank Battle";
        FontMetrics fm = g.getFontMetrics();
        g.drawString(title, (WINDOW_WIDTH - fm.stringWidth(title)) / 2, 200);

        g.setFont(new Font("Consolas", Font.PLAIN, 36));
        int yStart = 350;
        for (int i = 0; i < currentItems.length; i++) {
            g.setColor(i == selected ? Color.WHITE : new Color(150, 150, 160));
            String text = currentItems[i];
            fm = g.getFontMetrics();
            g.drawString(text, (WINDOW_WIDTH - fm.stringWidth(text)) / 2, yStart + i * 50);
        }

        g.setColor(new Color(100, 100, 120));
        g.setFont(new Font("Consolas", Font.PLAIN, 14));
        String hint;
        if (state.equals("single")) {
            hint = "Select game mode";
        } else if (state.equals("level")) {
            hint = "Load saved progress or start fresh";
        } else if (state.equals("multiplayer")) {
            hint = "Select a multiplayer mode";
        } else if (state.equals("mode")) {
            hint = "Select game mode";
        } else if (state.equals("mods")) {
            hint = "Manage mods, scripts and resource packs";
        } else {
            hint = "Use UP/DOWN to navigate, ENTER to select, ESC to exit";
        }
        fm = g.getFontMetrics();
        g.drawString(hint, 20, WINDOW_HEIGHT - 40);

        g.setFont(new Font("Consolas", Font.PLAIN, 14));
        g.setColor(new Color(80, 80, 90));
        g.drawString("v1.9", WINDOW_WIDTH - 100, WINDOW_HEIGHT - 30);

        String path = "";
        if (state.equals("single") || state.equals("level")) {
            path = "Single Player";
        }
        if (state.equals("level")) {
            path += " > Level Mode";
        }
        if (state.equals("multiplayer") || state.equals("mode")) {
            path = "Multiplayer";
        }
        if (state.equals("mode")) {
            path += " > Local";
        }
        if (state.equals("mods")) {
            path = "Mods";
        }
        if (!path.isEmpty()) {
            g.setColor(new Color(100, 100, 120));
            g.setFont(new Font("Consolas", Font.PLAIN, 14));
            g.drawString(path, 20, 30);
        }
    }

    private void drawModList(Graphics2D g) {
        g.setColor(new Color(20, 20, 30));
        g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        g.setColor(Color.WHITE);
        g.setFont(new Font("Consolas", Font.BOLD, 48));
        String title = "Mod List  (ENTER to toggle)";
        FontMetrics fm = g.getFontMetrics();
        g.drawString(title, (WINDOW_WIDTH - fm.stringWidth(title)) / 2, 80);

        g.setColor(new Color(100, 100, 120));
        g.setFont(new Font("Consolas", Font.PLAIN, 16));
        String header = "Status  Name                    Version      Game Version";
        g.drawString(header, 80, 130);

        g.setColor(new Color(60, 60, 70));
        g.drawLine(80, 138, WINDOW_WIDTH - 80, 138);

        java.util.List<com.tankbattle.mod.ModInfo> mods = com.tankbattle.mod.ModLoader.getMods();
        int visibleCount = getVisibleModCount();
        int total = Math.max(mods.size(), 1);
    
        int yStart = 160;
        int lineHeight = 28;

        for (int i = modListScroll; i < Math.min(modListScroll + visibleCount, total); i++) {
            int y = yStart + (i - modListScroll) * lineHeight;
            boolean selected = (i == modListSelected);

            if (i < mods.size()) {
                com.tankbattle.mod.ModInfo info = mods.get(i);
            
                // 判断状态
                boolean isLoaded = info.isLoaded();
                boolean isDisabled = info.isDisabled();
                boolean hasError = info.getError() != null && !isDisabled;
            
                String status = isLoaded ? "[X]" : "[ ]";
                String name = info.getName();
                String version = info.getVersion();
                String gameVer = info.getGameVersion();

                if (name.length() > 24) name = name.substring(0, 22) + "..";

                g.setColor(selected ? Color.WHITE : new Color(180, 180, 190));
                if (selected) g.setFont(new Font("Consolas", Font.BOLD, 16));
                else g.setFont(new Font("Consolas", Font.PLAIN, 16));

                // 状态颜色
                Color statusColor;
                if (isLoaded) {
                    statusColor = new Color(0, 200, 80);
                } else if (isDisabled) {
                    statusColor = new Color(130, 130, 130);
                } else if (hasError) {
                    statusColor = new Color(255, 150, 50);
                } else {
                    statusColor = new Color(200, 50, 50);
                }
                g.setColor(statusColor);
                g.drawString(status, 80, y);

                // 名称颜色
                if (selected) {
                    g.setColor(Color.WHITE);
                } else if (isDisabled) {
                    g.setColor(new Color(130, 130, 130));
                } else {
                    g.setColor(new Color(200, 200, 200));
                }
                g.drawString(name, 170, y);
    
                // 版本颜色
                g.setColor(selected ? new Color(255, 255, 100) : new Color(150, 150, 160));
                g.drawString(version, 450, y);

                // 游戏版本
                g.setColor(new Color(130, 130, 140));
                g.drawString(gameVer, 580, y);

                // 错误信息
                if (info.getError() != null) {
                    String err = info.getError();
                    if (isDisabled) {
                        g.setColor(new Color(130, 130, 130));
                    } else {
                        g.setColor(new Color(255, 150, 50));
                    }
                    g.setFont(new Font("Consolas", Font.PLAIN, 13));
                    if (err.length() > 30) err = err.substring(0, 28) + "..";
                    g.drawString(err, 700, y);
                }
            } else {
                g.setColor(new Color(100, 100, 120));
                g.setFont(new Font("Consolas", Font.PLAIN, 16));
                g.drawString("No mods found. Place .jar files in mods/", 80, y);
            }
        }

        g.setColor(new Color(100, 100, 120));
        g.setFont(new Font("Consolas", Font.PLAIN, 14));
        String countText = "Total: " + mods.size() + " mods";
        if (mods.size() > visibleCount) {
            countText += " (Page " + (modListScroll / visibleCount + 1) + "/" + ((mods.size() - 1) / visibleCount + 1) + ")";
        }
        fm = g.getFontMetrics();
        g.drawString(countText, (WINDOW_WIDTH - fm.stringWidth(countText)) / 2, WINDOW_HEIGHT - 30);

        g.setColor(new Color(80, 80, 90));
        g.setFont(new Font("Consolas", Font.PLAIN, 14));
        String hint = "[X] loaded  [ ] unloaded  [ ] disabled  ENTER toggle  UP/DOWN scroll  ESC back";
        fm = g.getFontMetrics();
        g.drawString(hint, (WINDOW_WIDTH - fm.stringWidth(hint)) / 2, WINDOW_HEIGHT - 60);
    }

    public String getState() { return state; }
}