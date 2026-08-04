package com.tankbattle;

import java.awt.*;
import java.util.HashMap;
import java.util.Map;

/**
 * 关卡配置数据
 */
public class LevelData {

    // 敌人类型配置
    public static class EnemyType {
        public final int hp;
        public final double speedMult;
        public final Color color;
        public final String label;

        public EnemyType(int hp, double speedMult, Color color, String label) {
            this.hp = hp;
            this.speedMult = speedMult;
            this.color = color;
            this.label = label;
        }
    }

    public static final Map<String, EnemyType> ENEMY_TYPES = new HashMap<>();
    static {
        ENEMY_TYPES.put("normal", new EnemyType(1, 1.0, new Color(200, 50, 50), "Normal"));
        ENEMY_TYPES.put("tough", new EnemyType(2, 1.0, new Color(255, 150, 50), "Tough"));
        ENEMY_TYPES.put("elite", new EnemyType(3, 1.5, new Color(200, 100, 255), "Elite"));
    }

    // Boss 配置
    public static class BossConfig {
        public final int hp;
        public final double sizeMult;
        public final double speedMult;
        public final int bulletDamage;
        public final double bulletSpeedMult;

        public BossConfig(int hp, double sizeMult, double speedMult, int bulletDamage, double bulletSpeedMult) {
            this.hp = hp;
            this.sizeMult = sizeMult;
            this.speedMult = speedMult;
            this.bulletDamage = bulletDamage;
            this.bulletSpeedMult = bulletSpeedMult;
        }
    }

    public static final Map<Integer, BossConfig> BOSS_TYPES = new HashMap<>();
    static {
        BOSS_TYPES.put(5, new BossConfig(5, 1.5, 1.5, 2, 1.0));
        BOSS_TYPES.put(10, new BossConfig(10, 2.0, 2.0, 2, 1.5));
    }

    // 关卡配置
    public static class LevelConfig {
        public final String enemyType;
        public final int count;
        public final double spawnInterval;
        public final boolean isBoss;
        public final Integer bossId;

        public LevelConfig(String enemyType, int count, double spawnInterval, boolean isBoss, Integer bossId) {
            this.enemyType = enemyType;
            this.count = count;
            this.spawnInterval = spawnInterval;
            this.isBoss = isBoss;
            this.bossId = bossId;
        }
    }

    public static final Map<Integer, LevelConfig> LEVELS = new HashMap<>();
    static {
        LEVELS.put(1, new LevelConfig("normal", 6, 4.0, false, null));
        LEVELS.put(2, new LevelConfig("normal", 8, 4.0, false, null));
        LEVELS.put(3, new LevelConfig("normal", 10, 4.0, false, null));
        LEVELS.put(4, new LevelConfig("tough", 6, 4.0, false, null));
        LEVELS.put(5, new LevelConfig(null, 1, 4.0, true, 5));
        LEVELS.put(6, new LevelConfig("tough", 8, 3.0, false, null));
        LEVELS.put(7, new LevelConfig("normal", 10, 4.0, false, null));
        LEVELS.put(8, new LevelConfig("normal", 12, 4.0, false, null));
        LEVELS.put(9, new LevelConfig("elite", 8, 4.0, false, null));
        LEVELS.put(10, new LevelConfig(null, 1, 4.0, true, 10));
    }

    public static final int MAX_LEVEL = 10;
    public static final int[] MAP_ROTATION = {0, 1, 2, 3, 4};

    public static LevelConfig getLevelConfig(int level) {
        return LEVELS.get(level);
    }

    public static int getMapId(int level) {
        return MAP_ROTATION[(level - 1) % MAP_ROTATION.length];
    }

    public static boolean isBossLevel(int level) {
        LevelConfig config = LEVELS.get(level);
        return config != null && config.isBoss;
    }

    public static BossConfig getBossConfig(int level) {
        return BOSS_TYPES.get(level);
    }
}