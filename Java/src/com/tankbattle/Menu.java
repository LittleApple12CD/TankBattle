package com.tankbattle;

import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import static com.tankbattle.Utils.*;

public class Menu extends JPanel {
    private String[] mainItems = {"Single Player", "Multiplayer", "Mod", "Settings", "Exit"};
    private String[] multiItems = {"Local", "Lan", "Server", "Online", "Back"};
    private String[] modeItems = {"PVP", "PVE", "Back"};

    private String[] currentItems = mainItems;
    private int selected = 0;
    private String state = "main";

    public Menu() {
        setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        setBackground(new Color(20, 20, 30));
        setOpaque(true);
        setFocusable(true);
    }

    public String handleKeyPress(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_UP:
                selected = (selected - 1 + currentItems.length) % currentItems.length;
                repaint();
                return null;
            case KeyEvent.VK_DOWN:
                selected = (selected + 1) % currentItems.length;
                repaint();
                return null;
            case KeyEvent.VK_ENTER:
                return selectCurrent();
            case KeyEvent.VK_ESCAPE:
                if (state.equals("main")) {
                    System.exit(0);
                } else {
                    goBack();
                    repaint();
                }
                return null;
        }
        return null;
    }

    private String selectCurrent() {
        String choice = currentItems[selected];

        if (state.equals("main")) {
            if (choice.equals("Single Player")) {
                return "single_player";
            } else if (choice.equals("Multiplayer")) {
                state = "multiplayer";
                currentItems = multiItems;
                selected = 0;
                repaint();
                return null;
            } else if (choice.equals("Mod")) {
                System.out.println("Mod mode - reserved for future");
                return null;
            } else if (choice.equals("Exit")) {
                System.exit(0);
            }
        } else if (state.equals("multiplayer")) {
            if (choice.equals("Local")) {
                state = "mode";
                currentItems = modeItems;
                selected = 0;
                repaint();
                return null;
            } else if (choice.equals("Lan (Reserved)")) {
                System.out.println("Lan mode - reserved for future");
                return null;
            } else if (choice.equals("Online (Reserved)")) {
                System.out.println("Online mode - reserved for future");
                return null;
            } else if (choice.equals("Back")) {
                goBack();
                repaint();
                return null;
            }
        } else if (state.equals("mode")) {
            if (choice.equals("PVP")) {
                return "pvp";
            } else if (choice.equals("PVE")) {
                return "pve";
            } else if (choice.equals("Back")) {
                goBack();
                repaint();
                return null;
            }
        }
        return null;
    }

    private void goBack() {
        if (state.equals("multiplayer")) {
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

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        // 设置背景颜色
        g2d.setColor(new Color(20, 20, 30));
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // 标题
        g2d.setColor(Color.WHITE);
        g2d.setFont(new Font("Consolas", Font.BOLD, 72));
        String title = "Tank Battle";
        FontMetrics fm = g2d.getFontMetrics();
        g2d.drawString(title, (getWidth() - fm.stringWidth(title)) / 2, 200);

        // 菜单项
        g2d.setFont(new Font("Consolas", Font.PLAIN, 36));
        int yStart = 350;
        for (int i = 0; i < currentItems.length; i++) {
            g2d.setColor(i == selected ? Color.WHITE : new Color(150, 150, 160));
            String text = currentItems[i];
            fm = g2d.getFontMetrics();
            g2d.drawString(text, (getWidth() - fm.stringWidth(text)) / 2, yStart + i * 50);
        }

        // 底部提示
        g2d.setColor(new Color(100, 100, 120));
       g2d.setFont(new Font("Consolas", Font.PLAIN, 14));
        String hint = "Use UP/DOWN to navigate, ENTER to select, ESC to exit";
        fm = g2d.getFontMetrics();
        g2d.drawString(hint, 20, getHeight() - 40);

        // 版本号
        g2d.drawString("v1.4", getWidth() - 80, getHeight() - 30);
    }

        public String getState() { return state; }
    }