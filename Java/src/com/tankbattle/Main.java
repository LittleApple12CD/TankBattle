package com.tankbattle;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import static com.tankbattle.Utils.*;

public class Main extends JPanel implements Runnable, KeyListener {

    private Game game;
    private Menu menu;
    private String gameState; // "menu" 或 "playing"
    private Thread gameThread;
    private boolean running;

    // 键盘状态
    private boolean p1Left, p1Right, p1Up, p1Down, p1Shoot;
    private boolean p2Left, p2Right, p2Up, p2Down, p2Shoot;

    public Main() {
        setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        setBackground(new Color(20, 20, 30));
        setFocusable(true);
        addKeyListener(this);

        game = new Game();
        menu = new Menu();
        gameState = "menu";
        running = true;

        gameThread = new Thread(this);
        gameThread.start();
    }

    // ===== 启动游戏模式 =====
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
        repaint();
    }

    // ===== 启动关卡模式 =====
    private void startLevelMode(int level) {
        game = new Game();
        game.startLevelMode(level);
        gameState = "playing";
        repaint();
    }

    @Override
    public void run() {
        double nsPerTick = 1_000_000_000.0 / FPS;
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

            repaint();

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

            // ===== 检测 Enter 键（关卡模式继续） =====
            // 注意：Enter 是在 KeyListener 中处理的，这里只更新游戏逻辑

            // P1 移动
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

            // P2 移动
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

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        if (gameState.equals("menu")) {
            menu.setBounds(0, 0, getWidth(), getHeight());
            menu.paint(g2d);
        } else {
            game.draw(g2d);
        }
    }

    // ===== 键盘事件 =====

    @Override
    public void keyPressed(KeyEvent e) {
        int key = e.getKeyCode();

        // ===== 菜单模式 =====
        if (gameState.equals("menu")) {
            String result = menu.handleKeyPress(key);
            if (result != null) {
                switch (result) {
                    case "endless_mode":
                        startGame("single_player");
                        break;
                    case "new_game":
                        startLevelMode(1);
                        break;
                    case "load_game":
                        if (SaveManager.hasSave()) {
                            int level = SaveManager.loadProgress();
                            startLevelMode(level);
                        } else {
                            System.out.println("No save found, starting from level 1");
                            startLevelMode(1);
                        }
                        break;
                    case "pvp":
                        startGame("pvp");
                        break;
                    case "pve":
                        startGame("pve");
                        break;
                    case "exit":
                        System.exit(0);
                        break;
                }
            }
            repaint();
            return;
        }

        // ===== 游戏中 =====

        // ESC 返回菜单
        if (key == KeyEvent.VK_ESCAPE) {
            if (gameState.equals("playing")) {
                gameState = "menu";
                menu.resetState();
                game.setPaused(false);
                repaint();
                return;
            }
        }

        // 关卡模式：按 Enter 继续
        if (key == KeyEvent.VK_ENTER) {
            if (gameState.equals("playing") && game != null && game.isWaitingForEnter()) {
                game.continueToNext();
                // 如果通关完成，返回菜单
                if (game.isVictoryDone()) {
                    gameState = "menu";
                    menu.resetState();
                    repaint();
                }
                return;
            }
        }

        // 禁用 O 和 G
        if (key == KeyEvent.VK_O || key == KeyEvent.VK_G) {
            return;
        }

        // P1 控制
        switch (key) {
            case KeyEvent.VK_LEFT: p1Left = true; break;
            case KeyEvent.VK_RIGHT: p1Right = true; break;
            case KeyEvent.VK_UP: p1Up = true; break;
            case KeyEvent.VK_DOWN: p1Down = true; break;
            case KeyEvent.VK_SPACE: p1Shoot = true; break;
            // P2 控制
            case KeyEvent.VK_A: p2Left = true; break;
            case KeyEvent.VK_D: p2Right = true; break;
            case KeyEvent.VK_W: p2Up = true; break;
            case KeyEvent.VK_S: p2Down = true; break;
            case KeyEvent.VK_J: p2Shoot = true; break;
            // 其他
            case KeyEvent.VK_P: game.setPaused(!game.isPaused()); break;
            case KeyEvent.VK_R: game.initLevel(); break;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int key = e.getKeyCode();

        switch (key) {
            case KeyEvent.VK_LEFT: p1Left = false; break;
            case KeyEvent.VK_RIGHT: p1Right = false; break;
            case KeyEvent.VK_UP: p1Up = false; break;
            case KeyEvent.VK_DOWN: p1Down = false; break;
            case KeyEvent.VK_SPACE: p1Shoot = false; break;
            case KeyEvent.VK_A: p2Left = false; break;
            case KeyEvent.VK_D: p2Right = false; break;
            case KeyEvent.VK_W: p2Up = false; break;
            case KeyEvent.VK_S: p2Down = false; break;
            case KeyEvent.VK_J: p2Shoot = false; break;
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {}

    // ===== 主入口 =====
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Tank Battle - Java");
            Main panel = new Main();

            // 图标
            try {
                java.net.URL iconURL = Main.class.getResource("/icon.png");
                if (iconURL != null) {
                    Image icon = Toolkit.getDefaultToolkit().getImage(iconURL);
                    frame.setIconImage(icon);
                }
            } catch (Exception ex) {
                // 忽略
            }

            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setResizable(false);
            frame.getContentPane().add(panel);
            frame.pack();
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);

            panel.requestFocusInWindow();
        });
    }
}