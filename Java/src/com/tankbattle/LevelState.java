package com.tankbattle;

/**
 * 关卡状态机
 */
public class LevelState {

    public enum State {
        IDLE,           // 初始
        SPAWNING,       // 生成敌人中
        WAITING,        // 等待敌人被消灭
        CLEARED,        // 过关
        GAMEOVER,       // 失败
        BOSS_SPAWNING,  // Boss 生成中
        BOSS_FIGHT,     // Boss 战斗中
        VICTORY         // 通关胜利
    }

    public static class Controller {
        private int currentLevel;
        private State state;
        private int enemiesSpawned;
        private int enemiesTotal;
        private String enemyType;
        private double spawnTimer;
        private double spawnInterval;
        private boolean isBossLevel;
        private Integer bossId;
        private boolean bossSpawned;
        private double waitTimer;
        private final double waitDuration = 2.0;

        public Controller() {
            this.state = State.IDLE;
        }

        public void startLevel(int level) {
            LevelData.LevelConfig config = LevelData.getLevelConfig(level);
            if (config == null) return;

            this.currentLevel = level;
            this.enemiesSpawned = 0;
            this.enemiesTotal = config.count;
            this.enemyType = config.enemyType;
            this.spawnInterval = config.spawnInterval;
            this.isBossLevel = config.isBoss;
            this.bossId = config.bossId;
            this.bossSpawned = false;
            this.state = State.SPAWNING;
            this.spawnTimer = 0;
            this.waitTimer = 0;
        }

        public String update(double dt, int enemiesAlive) {
            switch (state) {
                case CLEARED:
                    waitTimer -= dt;
                    if (waitTimer <= 0) {
                        return "next_level";
                    }
                    return "waiting";

                case GAMEOVER:
                    return "gameover";

                case VICTORY:
                    waitTimer -= dt;
                    if (waitTimer <= 0) {
                        return "victory_done";
                    }
                    return "waiting";

                case SPAWNING:
                    if (isBossLevel && bossSpawned) {
                        return "boss_spawned";
                    }
                    if (isBossLevel) {
                        spawnTimer -= dt;
                        if (spawnTimer <= 0 && !bossSpawned) {
                            state = State.BOSS_SPAWNING;
                            return "spawn_boss";
                        }
                        return "spawning";
                    }
                    if (enemiesSpawned < enemiesTotal) {
                        spawnTimer -= dt;
                        if (spawnTimer <= 0) {
                            spawnTimer = spawnInterval;
                            enemiesSpawned++;
                            return "spawn_enemy";
                        }
                        return "spawning";
                    } else {
                        state = State.WAITING;
                        return "waiting";
                    }

                case BOSS_SPAWNING:
                    return "boss_spawning";

                case BOSS_FIGHT:
                    return "boss_fight";

                case WAITING:
                    if (enemiesAlive <= 0) {
                        state = State.CLEARED;
                        waitTimer = waitDuration;
                        if (currentLevel == 10) {
                            return "game_victory";
                        }
                        return "level_cleared";
                    }
                    return "waiting";

                default:
                    return "idle";
            }
        }

        public void onBossSpawned() {
            bossSpawned = true;
            state = State.BOSS_FIGHT;
        }

        public State getState() { return state; }
        public int getCurrentLevel() { return currentLevel; }
        public boolean isBossLevel() { return isBossLevel; }
        public boolean isBossSpawned() { return bossSpawned; }
        public void setState(State state) { this.state = state; }
        public void setWaitTimer(double time) { this.waitTimer = time; }
    }
}