package com.tankbattle;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

import static com.tankbattle.Utils.*;

/**
 * 坦克大战 - 主程序
 */
public class Main extends JPanel implements Runnable, KeyListener {

    private Game game;
    private Thread gameThread;
    private boolean running;

    // 键盘状态
    private boolean p1Left, p1Right, p1Up, p1Down, p1Shoot;
    private boolean p2Left, p2Right, p2Up, p2Down, p2Shoot;

    // 字体
    private Font fontNormal;
    private Font fontBig;
    private Font fontSmall;

    public Main() {
        setPreferredSize(new Dimension(WINDOW_WIDTH, WINDOW_HEIGHT));
        setBackground(COLOR_BG);
        setFocusable(true);
        addKeyListener(this);

        // 初始化字体
        initFonts();

        game = new Game();
        running = true;

        gameThread = new Thread(this);
        gameThread.start();
    }

    /**
     * 初始化字体 - 自动检测系统中文字体
     */
    private void initFonts() {
        // 尝试的中文字体列表（按优先级排序）
        String[] chineseFonts = {
            "Microsoft YaHei",      // Windows 微软雅黑
            "SimHei",               // Windows 黑体
            "SimSun",               // Windows 宋体
            "KaiTi",                // Windows 楷体
            "FangSong",             // Windows 仿宋
            "PingFang SC",          // macOS 苹方
            "Hiragino Sans GB",     // macOS
            "Noto Sans CJK SC",     // Linux 思源黑体
            "WenQuanYi Micro Hei",  // Linux 文泉驿
            "Droid Sans Fallback"   // Android
        };

        // 英文字体（回退）
        String[] englishFonts = {
            "Consolas",
            "Arial",
            "DejaVu Sans",
            "SansSerif"
        };

        // 先尝试中文字体
        String selectedFont = null;
        boolean isChinese = true;

        for (String fontName : chineseFonts) {
            if (isFontAvailable(fontName)) {
                selectedFont = fontName;
                break;
            }
        }

        // 如果没有中文字体，使用英文字体
        if (selectedFont == null) {
            isChinese = false;
            for (String fontName : englishFonts) {
                if (isFontAvailable(fontName)) {
                    selectedFont = fontName;
                    break;
                }
            }
        }

        // 最终回退
        if (selectedFont == null) {
            selectedFont = "SansSerif";
        }

        // 创建字体
        int normalSize = 18;
        int bigSize = 36;
        int smallSize = 14;

        fontNormal = new Font(selectedFont, Font.PLAIN, normalSize);
        fontBig = new Font(selectedFont, Font.BOLD, bigSize);
        fontSmall = new Font(selectedFont, Font.PLAIN, smallSize);

        // 调试输出
        System.out.println("使用字体: " + selectedFont + " (中文支持: " + isChinese + ")");
    }

    /**
     * 检查字体是否可用
     */
    private boolean isFontAvailable(String fontName) {
        Font testFont = new Font(fontName, Font.PLAIN, 12);
        // 如果字体名称被替换，说明系统不支持
        return testFont.getFamily().equals(fontName) ||
               testFont.getFontName().equals(fontName) ||
               testFont.getName().equals(fontName);
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
        double dt = 1.0 / FPS;

        if (!game.isPaused() && !game.isGameOver()) {
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
        }

        game.update(dt);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        game.draw(g2d, fontNormal, fontBig, fontSmall);
    }

    // ===== 键盘事件 =====

    @Override
    public void keyPressed(KeyEvent e) {
        int key = e.getKeyCode();

        switch (key) {
            case KeyEvent.VK_ESCAPE:
                running = false;
                System.exit(0);
                break;
            case KeyEvent.VK_P:
                game.setPaused(!game.isPaused());
                break;
            case KeyEvent.VK_R:
                game.initLevel();
                p1Left = false;
                p1Right = false;
                p1Up = false;
                p1Down = false;
                p1Shoot = false;
                p2Left = false;
                p2Right = false;
                p2Up = false;
                p2Down = false;
                p2Shoot = false;
                break;
            case KeyEvent.VK_G:
                game.togglePvpMode();
                p1Left = false;
                p1Right = false;
                p1Up = false;
                p1Down = false;
                p1Shoot = false;
                p2Left = false;
                p2Right = false;
                p2Up = false;
                p2Down = false;
                p2Shoot = false;
                break;
            // P1 控制
            case KeyEvent.VK_LEFT:
                p1Left = true;
                break;
            case KeyEvent.VK_RIGHT:
                p1Right = true;
                break;
            case KeyEvent.VK_UP:
                p1Up = true;
                break;
            case KeyEvent.VK_DOWN:
                p1Down = true;
                break;
            case KeyEvent.VK_SPACE:
                p1Shoot = true;
                break;
            // P2 控制
            case KeyEvent.VK_A:
                p2Left = true;
                break;
            case KeyEvent.VK_D:
                p2Right = true;
                break;
            case KeyEvent.VK_W:
                p2Up = true;
                break;
            case KeyEvent.VK_S:
                p2Down = true;
                break;
            case KeyEvent.VK_J:
                p2Shoot = true;
                break;
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        int key = e.getKeyCode();

        switch (key) {
            case KeyEvent.VK_LEFT:
                p1Left = false;
                break;
            case KeyEvent.VK_RIGHT:
                p1Right = false;
                break;
            case KeyEvent.VK_UP:
                p1Up = false;
                break;
            case KeyEvent.VK_DOWN:
                p1Down = false;
                break;
            case KeyEvent.VK_SPACE:
                p1Shoot = false;
                break;
            case KeyEvent.VK_A:
                p2Left = false;
                break;
            case KeyEvent.VK_D:
                p2Right = false;
                break;
            case KeyEvent.VK_W:
                p2Up = false;
                break;
            case KeyEvent.VK_S:
                p2Down = false;
                break;
            case KeyEvent.VK_J:
                p2Shoot = false;
                break;
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {}

    // ===== 主函数 =====

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("TankBattle - Java");
            Main panel = new Main();

            // 用 ImageIO 加载
            try {
                Image icon = ImageIO.read(new File("src/com/tankbattle/icon.png"));
                frame.setIconImage(icon);
            } catch (Exception e) {
                System.out.println("图标加载失败: " + e.getMessage());
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