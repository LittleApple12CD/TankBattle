package com.tankbattle;

import java.awt.*;

/**
 * 工具类 - 包含常量和向量工具
 */
public class Utils {

    // ===== 在类加载时应用配置 =====
    static {
        // 先加载配置，再初始化其他
        GameConfig.applyToUtils();
    }

    // ===== 窗口变量 =====
    public static int WINDOW_WIDTH = 1600;
    public static int WINDOW_HEIGHT = 900;
    public static int FPS = 60;

    // ===== 网格常量 =====
    public static int GRID_SIZE = 13;
    public static int CELL_SIZE = 50;
    public static int GRID_OFFSET_X;
    public static int GRID_OFFSET_Y;

    // ===== 坦克常量 =====
    public static int TANK_SIZE = 36;
    public static int TANK_SPEED = 6;
    public static int PLAYER_LIVES = 3;
    public static double SHOT_COOLDOWN = 0.5;

    // ===== 子弹常量 =====
    public static int BULLET_SIZE = 8;
    public static double BULLET_SPEED = 480.0;
    public static int MAX_BULLETS = 3;

    // ===== 敌人常量 =====
    public static int ENEMY_COUNT = 4;
    public static double ENEMY_SPAWN_INTERVAL = 4.0;
    public static double AI_DIRECTION_CHANGE = 2.0;
    public static double AI_SHOOT_CHANCE = 0.4;

    // ===== 移动常量 =====
    public static double MOVE_STEP = 2.0;

    // ===== 音效常量（这些由 Settings 管理） =====
    public static boolean SOUND_ENABLED = true;
    public static float SOUND_VOLUME = 0.8f;
    public static boolean SOUND_MUTED = false;
    public static final String SOUND_PATH = "/sounds/";
    public static final String[] SOUND_NAMES = {
        "shoot", "explode", "powerup", "victory", "gameover"
    };
    public static final String[] SOUND_FILES = {
        "shoot.wav", "explode.wav", "powerup.wav", "victory.wav", "gameover.wav"
    };

    // ===== 颜色常量 =====
    public static final Color COLOR_BG = new Color(20, 20, 30);
    public static final Color COLOR_GRID = new Color(30, 30, 40);
    public static final Color COLOR_PLAYER1 = new Color(0, 200, 80);
    public static final Color COLOR_PLAYER2 = new Color(255, 180, 50);
    public static final Color COLOR_ENEMY = new Color(200, 50, 50);
    public static final Color COLOR_BULLET_P1 = new Color(100, 255, 100);
    public static final Color COLOR_BULLET_P2 = new Color(255, 220, 100);
    public static final Color COLOR_BULLET_ENEMY = new Color(255, 100, 100);
    public static final Color COLOR_WALL = new Color(150, 150, 150);
    public static final Color COLOR_STEEL = new Color(180, 180, 200);
    public static final Color COLOR_TEXT = new Color(255, 255, 255);
    public static final Color COLOR_TEXT_DIM = new Color(150, 150, 160);
    public static final Color COLOR_PVP = new Color(255, 100, 100);

    // ===== 地图名称 =====
    public static final String[] MAP_NAMES = {"空地", "十字", "迷宫", "碉堡", "对称"};

    /**
     * 更新网格偏移量（窗口大小改变后调用）
     */
    public static void updateGridOffsets() {
        GRID_OFFSET_X = (WINDOW_WIDTH - GRID_SIZE * CELL_SIZE) / 2;
        GRID_OFFSET_Y = (WINDOW_HEIGHT - GRID_SIZE * CELL_SIZE) / 2;
    }

    // 静态初始化
    static {
        updateGridOffsets();
    }

    // ===== 向量类 =====
    public static class Vec2 {
        public double x, y;

        public Vec2() {
            this(0, 0);
        }

        public Vec2(double x, double y) {
            this.x = x;
            this.y = y;
        }

        public Vec2 add(Vec2 other) {
            return new Vec2(x + other.x, y + other.y);
        }

        public Vec2 sub(Vec2 other) {
            return new Vec2(x - other.x, y - other.y);
        }

        public Vec2 mul(double scalar) {
            return new Vec2(x * scalar, y * scalar);
        }

        public Vec2 normalize() {
            double len = length();
            if (len > 0) {
                return new Vec2(x / len, y / len);
            }
            return new Vec2(0, 0);
        }

        public double length() {
            return Math.sqrt(x * x + y * y);
        }

        public double lengthSq() {
            return x * x + y * y;
        }

        public double dot(Vec2 other) {
            return x * other.x + y * other.y;
        }
    }

    /**
     * 限制值在范围内
     */
    public static double clamp(double val, double min, double max) {
        return Math.max(min, Math.min(max, val));
    }

    public static int clamp(int val, int min, int max) {
        return Math.max(min, Math.min(max, val));
    }

    public static java.awt.image.BufferedImage getEntityTexture(String entityId) {
        return com.tankbattle.resource.TextureManager.getEntityTexture(entityId);
    }

    /**
     * 应用所有设置（窗口大小改变后需要调用）
     */
    public static void refresh() {
        updateGridOffsets();
    }
}