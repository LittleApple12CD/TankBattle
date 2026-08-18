package com.tankbattle;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferStrategy;
import java.net.URL;

import static com.tankbattle.Utils.*;

public class Main implements Runnable, KeyListener {

    private static Main instance;
    private JFrame frame;
    private Canvas canvas;
    private BufferStrategy strategy;

    private Game game;
    private Menu menu;
    private SettingsMenu settingsMenu;
    private String gameState;
    private Thread gameThread;
    private boolean running;

    private boolean p1Left, p1Right, p1Up, p1Down, p1Shoot;
    private boolean p2Left, p2Right, p2Up, p2Down, p2Shoot;
    private boolean inSettings = false;

    public Main() {
        instance = this;

        GameConfig.load();
        GameConfig.applyToUtils();

        Settings.load();
        Settings.applyToUtils();

        Utils.refresh();

        frame = new JFrame("Tank Battle - Java");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);
        frame.setLayout(new BorderLayout());

        loadWindowIcon();

        game = new Game();
        menu = new Menu();
        settingsMenu = new SettingsMenu();

        canvas = new Canvas();
        canvas.setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        canvas.setBackground(new Color(20, 20, 30));
        canvas.setFocusable(true);
        canvas.addKeyListener(this);

        frame.add(canvas, BorderLayout.CENTER);
        frame.pack();
        frame.setLocationRelativeTo(null);

        // 创建缓冲策略
        canvas.createBufferStrategy(2);
        strategy = canvas.getBufferStrategy();

        gameState = "menu";
        running = true;

        gameThread = new Thread(this);
        gameThread.start();
        settingsMenu = new SettingsMenu();

        frame.setVisible(true);
        canvas.requestFocusInWindow();
    }

    public static Main getInstance() {
        return instance;
    }
    
    public Menu getMenu() {
        return menu;
    }

    public SettingsMenu getSettingsMenu() {
        return settingsMenu;
    }

    public Game getGame() {
        return game;
    }

    public JFrame getFrame() {
        return frame;
    }

    public Canvas getCanvas() {
        return canvas;
    }

    public String getGameState() {
        return gameState;
    }

    public void setGameState(String state) {
        this.gameState = state;
    }

    public boolean isInMenu() {
        return gameState.equals("menu");
    }

    public boolean isInSettings() {
        return inSettings;
    }

    public void exitGame() {
        running = false;
        frame.dispose();
        System.exit(0);
    }

    public void restartGame() {
        if (game != null) {
            game.initLevel();
        }
    }

    public void togglePause() {
        if (game != null) {
            game.setPaused(!game.isPaused());
        }
    }

    private void loadWindowIcon() {
        // 方法1: 从项目资源目录加载
        URL iconURL = getClass().getResource("/icon.png");
        if (iconURL != null) {
            ImageIcon icon = new ImageIcon(iconURL);
            frame.setIconImage(icon.getImage());
            System.out.println("图标加载成功: " + iconURL);
        } else {
            // 方法2: 从文件系统加载
            try {
                ImageIcon icon = new ImageIcon("icon.png");
                if (icon.getImage() != null) {
                    frame.setIconImage(icon.getImage());
                    System.out.println("图标从文件系统加载成功");
                }
            } catch (Exception e) {
                System.out.println("未找到 icon.png，使用默认图标");
            }
        }
    }

    private void startGame(String mode) {
        game = new Game();
        if (mode.equals("single_player")) {
            game.setSingleMode(true);
            game.setPvpMode(false);
            game.setEnemyCount(ENEMY_COUNT);
            game.initLevel();
        } else if (mode.equals("pvp")) {
            game.setSingleMode(false);
            game.setPvpMode(true);
            game.setEnemyCount(0);
            game.initLevel();
        } else if (mode.equals("pve")) {
            game.setSingleMode(false);
            game.setPvpMode(false);
            game.setEnemyCount(ENEMY_COUNT);
            game.initLevel();
        }
        gameState = "playing";
    }

    private void startLevelMode(int level) {
        game = new Game();
        game.startLevelMode(level);
        gameState = "playing";
    }

    @Override
    public void run() {
        double nsPerTick = 1_000_000_000.0 / GameConfig.FPS;
        double delta = 0;
        long lastTime = System.nanoTime();

        while (running) {
            long now = System.nanoTime();
            delta += (now - lastTime) / nsPerTick;
            lastTime = now;

            while (delta >= 1) {
                update();
                delta--;
            }

            render();
            
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void update() {
        if (gameState.equals("playing") && !game.isPaused() && !game.isGameOver()) {
            double dt = 1.0 / FPS;

            int dx1 = 0, dy1 = 0;
            if (p1Left) dx1 = -1;
            else if (p1Right) dx1 = 1;
            else if (p1Up) dy1 = -1;
            else if (p1Down) dy1 = 1;
            if (dx1 != 0 || dy1 != 0) {
                game.movePlayer1(dx1, dy1);
            }
            if (p1Shoot) {
                game.player1Shoot();
                p1Shoot = false;
            }

            int dx2 = 0, dy2 = 0;
            if (p2Left) dx2 = -1;
            else if (p2Right) dx2 = 1;
            else if (p2Up) dy2 = -1;
            else if (p2Down) dy2 = 1;
            if (dx2 != 0 || dy2 != 0) {
                game.movePlayer2(dx2, dy2);
            }
            if (p2Shoot) {
                game.player2Shoot();
                p2Shoot = false;
            }

            game.update(dt);
        }
    }

    private void render() {
        Graphics2D g = null;
        try {
            g = (Graphics2D) strategy.getDrawGraphics();
            g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

            // 清空画布
            g.setColor(new Color(20, 20, 30));
            g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

            if (gameState.equals("menu")) {
                    if (inSettings) {
                        settingsMenu.draw(g);
                    } else {
                        menu.draw(g);
                    }
                } else {
                    game.draw(g);
                }

            strategy.show();
        } catch (ClassCastException e) {
            // 如果类型转换失败，使用 Graphics 兼容方式
            Graphics g2 = strategy.getDrawGraphics();
            if (gameState.equals("menu")) {
                    if (inSettings) {
                        settingsMenu.draw(g);
                    } else {
                        menu.draw(g);
                    }
                } else {
                    game.draw(g);
                }
            strategy.show();
            if (g2 != null) {
                g2.dispose();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (g != null) {
                g.dispose();
            }
        }
    }

    private void resizeWindow() {
        frame.setSize(Utils.WINDOW_WIDTH, Utils.WINDOW_HEIGHT);
        canvas.setPreferredSize(new Dimension(Utils.WINDOW_WIDTH, Utils.WINDOW_HEIGHT));
        frame.pack();
        frame.setLocationRelativeTo(null);
        canvas.requestFocusInWindow();
    }

    // ===== 键盘事件 =====

    @Override
    public void keyPressed(KeyEvent e) {
        int key = e.getKeyCode();

        // ===== 设置菜单模式 =====
        if (inSettings) {
            String result = settingsMenu.handleKeyPress(key);
            if (result != null) {
                if (result.equals("back") || result.equals("exit")) {
                    inSettings = false;
                    menu.resetState();
                    Settings.applyToUtils();
                    resizeWindow();
                    SoundManager.getInstance().setVolume(Settings.SOUND_VOLUME);
                    if (Settings.SOUND_MUTED) {
                        SoundManager.getInstance().toggleMute();
                    }
                } else if (result.equals("resize")) {
                    resizeWindow();
                    Settings.applyToUtils();
                }
            }
            return;
        }

        if (gameState.equals("menu")) {
            String result = menu.handleKeyPress(key, game);
            if (result != null) {
                switch (result) {
                    case "back":
                        break;
                    case "endless_mode": startGame("single_player"); break;
                    case "new_game": startLevelMode(1); break;
                    case "load_game":
                        if (SaveManager.hasSave()) {
                            int level = SaveManager.loadProgress();
                            startLevelMode(level);
                        } else {
                            startLevelMode(1);
                        }
                        break;
                    case "pvp": startGame("pvp"); break;
                    case "pve": startGame("pve"); break;
                    case "settings": inSettings = true; settingsMenu.resetState(); break;
                    case "exit": System.exit(0); break;
                }
            }
            return;
        }
    
        if (key == KeyEvent.VK_ESCAPE) {
            if (gameState.equals("playing")) {
                gameState = "menu";
                menu.resetState();
                game.setPaused(false);
                return;
            }
        }

        if (key == KeyEvent.VK_ENTER) {
            if (gameState.equals("playing") && game != null && game.isWaitingForEnter()) {
                game.continueToNext();
                if (game.isVictoryDone()) {
                    gameState = "menu";
                    menu.resetState();
                }
                return;
            }
        }

        // ===== P1 控制（使用 Settings 中的按键码） =====
        // 使用 if-else if 确保每个按键只触发一个动作
        if (key == Settings.KEY_P1_LEFT) {
            p1Left = true;
        } else if (key == Settings.KEY_P1_RIGHT) {
            p1Right = true;
        } else if (key == Settings.KEY_P1_UP) {
            p1Up = true;
        } else if (key == Settings.KEY_P1_DOWN) {
            p1Down = true;
        } else if (key == Settings.KEY_P1_SHOOT) {
            p1Shoot = true;
        }
        // P2 控制
        else if (key == Settings.KEY_P2_LEFT) {
            p2Left = true;
        } else if (key == Settings.KEY_P2_RIGHT) {
            p2Right = true;
        } else if (key == Settings.KEY_P2_UP) {
            p2Up = true;
        } else if (key == Settings.KEY_P2_DOWN) {
            p2Down = true;
        } else if (key == Settings.KEY_P2_SHOOT) {
            p2Shoot = true;
        }
        // 其他功能键（保持硬编码）
        else if (key == KeyEvent.VK_P) {
            game.setPaused(!game.isPaused());
        } else if (key == KeyEvent.VK_R) {
            if (game != null) {
                if ("level".equals(game.getGameMode())) {
                    int currentLevel = game.getCurrentLevel();
                    game.startLevelMode(currentLevel);
                } else {
                    game.initLevel();
                }
            }
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int key = e.getKeyCode();

        // ===== 使用 if-else if 确保每个按键只触发一个动作 =====
        if (key == Settings.KEY_P1_LEFT) {
            p1Left = false;
        } else if (key == Settings.KEY_P1_RIGHT) {
            p1Right = false;
        } else if (key == Settings.KEY_P1_UP) {
            p1Up = false;
        } else if (key == Settings.KEY_P1_DOWN) {
            p1Down = false;
        } else if (key == Settings.KEY_P1_SHOOT) {
            p1Shoot = false;
        }
        // P2 控制
        else if (key == Settings.KEY_P2_LEFT) {
            p2Left = false;
        } else if (key == Settings.KEY_P2_RIGHT) {
            p2Right = false;
        } else if (key == Settings.KEY_P2_UP) {
            p2Up = false;
        } else if (key == Settings.KEY_P2_DOWN) {
            p2Down = false;
        } else if (key == Settings.KEY_P2_SHOOT) {
            p2Shoot = false;
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {}

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                e.printStackTrace();
            }
            new Main();
        });
    }
}