package com.tankbattle;

import java.io.*;
import java.util.Properties;

/**
 * 游戏参数配置管理器
 * 负责加载/保存 config.properties（游戏参数，不含UI设置）
 */
public class GameConfig {

    private static final String CONFIG_FILE = "config.properties";
    private static Properties props = new Properties();

    // ===== 游戏参数（默认值） =====
    public static int FPS = 60;
    public static int TANK_SIZE = 36;
    public static int TANK_SPEED = 6;
    public static int PLAYER_LIVES = 3;
    public static double SHOT_COOLDOWN = 0.5;

    public static int BULLET_SIZE = 8;
    public static double BULLET_SPEED = 480.0;
    public static int MAX_BULLETS = 3;

    public static int ENEMY_COUNT = 4;
    public static double ENEMY_SPAWN_INTERVAL = 4.0;
    public static double AI_DIRECTION_CHANGE = 2.0;
    public static double AI_SHOOT_CHANCE = 0.4;

    public static double MOVE_STEP = 2.0;

    public static int GRID_SIZE = 13;
    public static int CELL_SIZE = 50;

    // ===== 默认值备份（用于重置） =====
    private static final int DEFAULT_FPS = 60;
    private static final int DEFAULT_TANK_SIZE = 36;
    private static final int DEFAULT_TANK_SPEED = 6;
    private static final int DEFAULT_PLAYER_LIVES = 3;
    private static final double DEFAULT_SHOT_COOLDOWN = 0.5;
    private static final int DEFAULT_BULLET_SIZE = 8;
    private static final double DEFAULT_BULLET_SPEED = 480.0;
    private static final int DEFAULT_MAX_BULLETS = 3;
    private static final int DEFAULT_ENEMY_COUNT = 4;
    private static final double DEFAULT_ENEMY_SPAWN_INTERVAL = 4.0;
    private static final double DEFAULT_AI_DIRECTION_CHANGE = 2.0;
    private static final double DEFAULT_AI_SHOOT_CHANCE = 0.4;
    private static final double DEFAULT_MOVE_STEP = 2.0;
    private static final int DEFAULT_GRID_SIZE = 13;
    private static final int DEFAULT_CELL_SIZE = 50;

    static {
        load();
    }

    /**
     * 加载配置文件
     */
    public static void load() {
        File file = new File(CONFIG_FILE);
        if (!file.exists()) {
            save(); // 不存在则创建默认配置
            return;
        }

        try (FileInputStream fis = new FileInputStream(file)) {
            props.load(fis);

            FPS = Integer.parseInt(props.getProperty("game.fps", String.valueOf(DEFAULT_FPS)));
            TANK_SIZE = Integer.parseInt(props.getProperty("game.tank_size", String.valueOf(DEFAULT_TANK_SIZE)));
            TANK_SPEED = Integer.parseInt(props.getProperty("game.tank_speed", String.valueOf(DEFAULT_TANK_SPEED)));
            PLAYER_LIVES = Integer.parseInt(props.getProperty("game.player_lives", String.valueOf(DEFAULT_PLAYER_LIVES)));
            SHOT_COOLDOWN = Double.parseDouble(props.getProperty("game.shot_cooldown", String.valueOf(DEFAULT_SHOT_COOLDOWN)));

            BULLET_SIZE = Integer.parseInt(props.getProperty("bullet.size", String.valueOf(DEFAULT_BULLET_SIZE)));
            BULLET_SPEED = Double.parseDouble(props.getProperty("bullet.speed", String.valueOf(DEFAULT_BULLET_SPEED)));
            MAX_BULLETS = Integer.parseInt(props.getProperty("bullet.max", String.valueOf(DEFAULT_MAX_BULLETS)));

            ENEMY_COUNT = Integer.parseInt(props.getProperty("enemy.count", String.valueOf(DEFAULT_ENEMY_COUNT)));
            ENEMY_SPAWN_INTERVAL = Double.parseDouble(props.getProperty("enemy.spawn_interval", String.valueOf(DEFAULT_ENEMY_SPAWN_INTERVAL)));
            AI_DIRECTION_CHANGE = Double.parseDouble(props.getProperty("enemy.ai_direction_change", String.valueOf(DEFAULT_AI_DIRECTION_CHANGE)));
            AI_SHOOT_CHANCE = Double.parseDouble(props.getProperty("enemy.ai_shoot_chance", String.valueOf(DEFAULT_AI_SHOOT_CHANCE)));

            MOVE_STEP = Double.parseDouble(props.getProperty("game.move_step", String.valueOf(DEFAULT_MOVE_STEP)));

            GRID_SIZE = Integer.parseInt(props.getProperty("map.grid_size", String.valueOf(DEFAULT_GRID_SIZE)));
            CELL_SIZE = Integer.parseInt(props.getProperty("map.cell_size", String.valueOf(DEFAULT_CELL_SIZE)));

            System.out.println("游戏配置加载成功: " + CONFIG_FILE);

        } catch (Exception e) {
            System.err.println("加载游戏配置失败，使用默认值: " + e.getMessage());
            resetToDefaults();
        }
    }

    /**
     * 保存配置文件
     */
    public static void save() {
        props.setProperty("game.fps", String.valueOf(FPS));
        props.setProperty("game.tank_size", String.valueOf(TANK_SIZE));
        props.setProperty("game.tank_speed", String.valueOf(TANK_SPEED));
        props.setProperty("game.player_lives", String.valueOf(PLAYER_LIVES));
        props.setProperty("game.shot_cooldown", String.valueOf(SHOT_COOLDOWN));
        props.setProperty("game.move_step", String.valueOf(MOVE_STEP));

        props.setProperty("bullet.size", String.valueOf(BULLET_SIZE));
        props.setProperty("bullet.speed", String.valueOf(BULLET_SPEED));
        props.setProperty("bullet.max", String.valueOf(MAX_BULLETS));

        props.setProperty("enemy.count", String.valueOf(ENEMY_COUNT));
        props.setProperty("enemy.spawn_interval", String.valueOf(ENEMY_SPAWN_INTERVAL));
        props.setProperty("enemy.ai_direction_change", String.valueOf(AI_DIRECTION_CHANGE));
        props.setProperty("enemy.ai_shoot_chance", String.valueOf(AI_SHOOT_CHANCE));

        props.setProperty("map.grid_size", String.valueOf(GRID_SIZE));
        props.setProperty("map.cell_size", String.valueOf(CELL_SIZE));

        try (FileOutputStream fos = new FileOutputStream(CONFIG_FILE)) {
            props.store(fos, "Tank Battle Game Config (FPS, speeds, sizes, etc.)");
            System.out.println("游戏配置保存成功: " + CONFIG_FILE);
        } catch (IOException e) {
            System.err.println("保存游戏配置失败: " + e.getMessage());
        }
    }

    /**
     * 重置为默认值
     */
    public static void resetToDefaults() {
        FPS = DEFAULT_FPS;
        TANK_SIZE = DEFAULT_TANK_SIZE;
        TANK_SPEED = DEFAULT_TANK_SPEED;
        PLAYER_LIVES = DEFAULT_PLAYER_LIVES;
        SHOT_COOLDOWN = DEFAULT_SHOT_COOLDOWN;
        BULLET_SIZE = DEFAULT_BULLET_SIZE;
        BULLET_SPEED = DEFAULT_BULLET_SPEED;
        MAX_BULLETS = DEFAULT_MAX_BULLETS;
        ENEMY_COUNT = DEFAULT_ENEMY_COUNT;
        ENEMY_SPAWN_INTERVAL = DEFAULT_ENEMY_SPAWN_INTERVAL;
        AI_DIRECTION_CHANGE = DEFAULT_AI_DIRECTION_CHANGE;
        AI_SHOOT_CHANCE = DEFAULT_AI_SHOOT_CHANCE;
        MOVE_STEP = DEFAULT_MOVE_STEP;
        GRID_SIZE = DEFAULT_GRID_SIZE;
        CELL_SIZE = DEFAULT_CELL_SIZE;
        save();
    }

    /**
     * 应用配置到 Utils（同步静态变量）
     */
    public static void applyToUtils() {
        Utils.FPS = FPS;
        Utils.TANK_SIZE = TANK_SIZE;
        Utils.TANK_SPEED = TANK_SPEED;
        Utils.PLAYER_LIVES = PLAYER_LIVES;
        Utils.SHOT_COOLDOWN = SHOT_COOLDOWN;
        Utils.BULLET_SIZE = BULLET_SIZE;
        Utils.BULLET_SPEED = BULLET_SPEED;
        Utils.MAX_BULLETS = MAX_BULLETS;
        Utils.ENEMY_COUNT = ENEMY_COUNT;
        Utils.ENEMY_SPAWN_INTERVAL = ENEMY_SPAWN_INTERVAL;
        Utils.AI_DIRECTION_CHANGE = AI_DIRECTION_CHANGE;
        Utils.AI_SHOOT_CHANCE = AI_SHOOT_CHANCE;
        Utils.MOVE_STEP = MOVE_STEP;
        Utils.GRID_SIZE = GRID_SIZE;
        Utils.CELL_SIZE = CELL_SIZE;
        Utils.refresh();
    }
}