package com.tankbattle;

import java.awt.*;
import java.awt.event.KeyEvent;
import static com.tankbattle.Utils.*;

public class Menu {
    private String[] mainItems = {"Single Player", "Multiplayer", "Mod", "Settings", "Exit"};
    private String[] singleItems = {"Endless Mode", "Level Mode", "Back"};
    private String[] levelItems = {"Load Game", "New Game", "Back"};
    private String[] multiItems = {"Local", "Lan", "Server", "Online", "Back"};
    private String[] modeItems = {"PVP", "PVE", "Back"};

    private String[] currentItems = mainItems;
    private int selected = 0;
    private String state = "main"; // main, single, level, multiplayer, mode

    public Menu() {}

    public String handleKeyPress(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_UP:
                selected = (selected - 1 + currentItems.length) % currentItems.length;
                return null;
            case KeyEvent.VK_DOWN:
                selected = (selected + 1) % currentItems.length;
                return null;
            case KeyEvent.VK_ENTER:
                return selectCurrent();
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

    private String selectCurrent() {
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
            } else if (choice.equals("Mod")) {
                System.out.println("Mod mode - reserved for future");
                return null;
            } else if (choice.equals("Settings")) {
                System.out.println("Settings - reserved for future");
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
        } else if (choice.equals("Settings")) {
            return "settings";
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
            currentItems = mainItems;
        } else if (state.equals("mode")) {
            state = "multiplayer";
            currentItems = multiItems;
        }
        selected = 0;
    }

    public void resetState() {
        state = "main";
        currentItems = mainItems;
        selected = 0;
    }

    // ===== 绘制方法 =====
    public void draw(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        g.setColor(new Color(20, 20, 30));
        g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

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
        } else {
            hint = "Use UP/DOWN to navigate, ENTER to select, ESC to exit";
        }
        fm = g.getFontMetrics();
        g.drawString(hint, 20, WINDOW_HEIGHT - 40);

        g.setFont(new Font("Consolas", Font.PLAIN, 14));
        g.setColor(new Color(80, 80, 90));
        g.drawString("v1.8", WINDOW_WIDTH - 100, WINDOW_HEIGHT - 30);

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
        if (!path.isEmpty()) {
            g.setColor(new Color(100, 100, 120));
            g.setFont(new Font("Consolas", Font.PLAIN, 14));
            g.drawString(path, 20, 30);
        }
    }

    public String getState() { return state; }
}