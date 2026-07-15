package com.tankbattle;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import static com.tankbattle.Utils.*;

/**
 * 游戏逻辑主类
 */
public class Game {
    private List<Wall> walls;
    private List<Explosion> explosions;
    private Tank player1;
    private Tank player2;
    private List<Tank> enemies;
    private List<EnemyAI> enemyAIs;
    private double enemySpawnTimer;
    private int score;
    private boolean gameOver;
    private boolean paused;
    private int currentMap;
    private boolean pvpMode;

    private Random random;
    private String[] mapNames;

    public Game() {
        walls = new ArrayList<>();
        explosions = new ArrayList<>();
        enemies = new ArrayList<>();
        enemyAIs = new ArrayList<>();
        random = new Random();
        score = 0;
        gameOver = false;
        paused = false;
        pvpMode = false;
        mapNames = new String[]{"Empty", "Cross", "Maze", "Bunker", "Sym"};

        initLevel();
    }

    public List<Wall> getWalls() {
        return walls;
    }

    public Tank getPlayer1() {
        return player1;
    }

    public Tank getPlayer2() {
        return player2;
    }

    public void togglePvpMode() {
        pvpMode = !pvpMode;
        initLevel();
    }

    public void initLevel() {
        // ===== 重置游戏状态 =====
        gameOver = false;
        paused = false;
        score = 0;
        enemySpawnTimer = 0;
        // =========================

        walls.clear();
        explosions.clear();
        enemies.clear();
        enemyAIs.clear();

        currentMap = random.nextInt(5);
        MapGenerator.generateMap(currentMap, walls);

        // P1 出生在左下
        double p1x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;
        double p1y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;
        player1 = new Tank(p1x, p1y, COLOR_PLAYER1, TANK_SPEED, true, 1);
    
        // P2 出生在右上
        double p2x = GRID_OFFSET_X + (GRID_SIZE - 2) * CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;
        double p2y = GRID_OFFSET_Y + CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;
        player2 = new Tank(p2x, p2y, COLOR_PLAYER2, TANK_SPEED, true, 2);

        if (!pvpMode) {
            for (int i = 0; i < ENEMY_COUNT; i++) {
            spawnEnemy();
            }
        }
    }

    private void spawnEnemy() {
        if (enemies.size() >= ENEMY_COUNT) return;

        int[][] spawnPos = {
                {GRID_SIZE - 2, 1},
                {GRID_SIZE / 2, 1},
                {1, 1}
        };
        int[] pos = spawnPos[random.nextInt(spawnPos.length)];
        double x = GRID_OFFSET_X + pos[0] * CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;
        double y = GRID_OFFSET_Y + pos[1] * CELL_SIZE + CELL_SIZE / 2.0 - TANK_SIZE / 2.0;

        Rectangle testRect = new Rectangle((int) x, (int) y, TANK_SIZE, TANK_SIZE);
        for (Tank enemy : enemies) {
            if (testRect.intersects(enemy.getRect())) return;
        }
        if (player1 != null && player1.alive && testRect.intersects(player1.getRect())) return;
        if (player2 != null && player2.alive && testRect.intersects(player2.getRect())) return;

        Tank enemy = new Tank(x, y, COLOR_ENEMY, TANK_SPEED * 0.7, false, 0);
        enemies.add(enemy);
        enemyAIs.add(new EnemyAI(enemy, this));
    }

    public void update(double dt) {
        if (gameOver || paused) return;

        // 更新 P1
        if (player1 != null && player1.alive) {
            player1.update(dt);
        }

        // 更新 P2
        if (player2 != null && player2.alive) {
            player2.update(dt);
        }

        // PVE模式：更新敌人
        if (!pvpMode) {
            for (EnemyAI ai : enemyAIs) {
                ai.update(dt);
            }

            // 清除死亡敌人
            for (int i = enemies.size() - 1; i >= 0; i--) {
                if (!enemies.get(i).alive) {
                    enemies.remove(i);
                    enemyAIs.remove(i);
                }
            }

            // 生成新敌人
            enemySpawnTimer -= dt;
            if (enemySpawnTimer <= 0 && enemies.size() < ENEMY_COUNT) {
                spawnEnemy();
                enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
            }
        }

        // 子弹碰撞
        handleBulletCollisions();

        // 爆炸更新
        for (int i = explosions.size() - 1; i >= 0; i--) {
            explosions.get(i).update(dt);
            if (!explosions.get(i).alive) {
                explosions.remove(i);
            }
        }

        // 游戏结束检查
        if (pvpMode) {
            if ((player1 != null && !player1.alive) || (player2 != null && !player2.alive)) {
                gameOver = true;
            }
        } else {
            if (player1 != null && !player1.alive && player2 != null && !player2.alive) {
                gameOver = true;
            }
        }
    }

    private void handleBulletCollisions() {
        List<Bullet> allBullets = new ArrayList<>();
        if (player1 != null) allBullets.addAll(player1.bullets);
        if (player2 != null) allBullets.addAll(player2.bullets);
        if (!pvpMode) {
            for (Tank enemy : enemies) {
                allBullets.addAll(enemy.bullets);
            }
        }

        for (Bullet bullet : allBullets) {
            if (!bullet.alive) continue;

            // 子弹 vs 墙壁
            boolean bulletHit = false;
            for (Wall wall : walls) {
                if (!wall.alive) continue;
                if (bullet.getRect().intersects(wall.getRect())) {
                    bullet.alive = false;
                    bulletHit = true;
                    if (!wall.isSteel) {
                        wall.alive = false;
                    }
                    addExplosion(bullet.x + bullet.w / 2.0, bullet.y + bullet.h / 2.0);
                    break;
                }
            }
            if (bulletHit || !bullet.alive) continue;

            // PVP模式：玩家互伤
            if (pvpMode) {
                // P1子弹 vs P2
                if (bullet.isPlayer && bullet.playerId == 1) {
                    if (player2 != null && player2.alive && bullet.getRect().intersects(player2.getRect())) {
                        bullet.alive = false;
                        player2.lives--;
                        addExplosion(player2.x + player2.w / 2.0, player2.y + player2.h / 2.0);
                        if (player2.lives <= 0) {
                            player2.alive = false;
                        }
                        continue;
                    }
                }
                // P2子弹 vs P1
                if (bullet.isPlayer && bullet.playerId == 2) {
                    if (player1 != null && player1.alive && bullet.getRect().intersects(player1.getRect())) {
                        bullet.alive = false;
                        player1.lives--;
                        addExplosion(player1.x + player1.w / 2.0, player1.y + player1.h / 2.0);
                        if (player1.lives <= 0) {
                            player1.alive = false;
                        }
                        continue;
                    }
                }
            } else {
                // PVE模式
                // 玩家子弹 vs 敌人
                if (bullet.isPlayer) {
                    for (Tank enemy : enemies) {
                        if (enemy.alive && bullet.getRect().intersects(enemy.getRect())) {
                            bullet.alive = false;
                            enemy.alive = false;
                            score += 10;
                            addExplosion(enemy.x + enemy.w / 2.0, enemy.y + enemy.h / 2.0);
                            break;
                        }
                    }
                } else {
                    // 敌人子弹 vs 玩家
                    for (Tank player : new Tank[]{player1, player2}) {
                        if (player != null && player.alive && bullet.getRect().intersects(player.getRect())) {
                            bullet.alive = false;
                            player.lives--;
                            addExplosion(player.x + player.w / 2.0, player.y + player.h / 2.0);
                            if (player.lives <= 0) {
                                player.alive = false;
                            }
                            break;
                        }
                    }
                }
            }
        }

        // 清理死亡子弹
        if (player1 != null) {
            player1.bullets.removeIf(b -> !b.alive);
        }
        if (player2 != null) {
            player2.bullets.removeIf(b -> !b.alive);
        }
        if (!pvpMode) {
            for (Tank enemy : enemies) {
                enemy.bullets.removeIf(b -> !b.alive);
            }
        }
    }

    private void addExplosion(double x, double y) {
        for (int i = 0; i < 4; i++) {
            double ox = x + (random.nextDouble() - 0.5) * 30;
            double oy = y + (random.nextDouble() - 0.5) * 30;
            explosions.add(new Explosion(ox, oy));
        }
    }

    // ===== 玩家控制 =====

    public void movePlayer1(int dx, int dy) {
        if (player1 != null && player1.alive) {
            player1.dir = new Vec2(dx, dy);
            player1.move(dx, dy, walls);
        }
    }

    public void movePlayer2(int dx, int dy) {
        if (player2 != null && player2.alive) {
            player2.dir = new Vec2(dx, dy);
            player2.move(dx, dy, walls);
        }
    }

    public void player1Shoot() {
        if (player1 != null && player1.alive) {
            player1.shoot();
        }
    }

    public void player2Shoot() {
        if (player2 != null && player2.alive) {
            player2.shoot();
        }
    }

    // ===== 渲染 =====

    public void draw(Graphics2D g, Font fontNormal, Font fontBig, Font fontSmall) {
        // 背景
        g.setColor(COLOR_BG);
        g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // 网格
        g.setColor(COLOR_GRID);
        for (int i = 0; i <= GRID_SIZE; i++) {
            int x = GRID_OFFSET_X + i * CELL_SIZE;
            int y = GRID_OFFSET_Y + i * CELL_SIZE;
            g.drawLine(x, GRID_OFFSET_Y, x, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE);
            g.drawLine(GRID_OFFSET_X, y, GRID_OFFSET_X + GRID_SIZE * CELL_SIZE, y);
        }

        // 游戏区域边框
        g.setColor(new Color(60, 60, 80));
        g.drawRect(GRID_OFFSET_X, GRID_OFFSET_Y, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE);

        // ===== 使用副本渲染，避免 ConcurrentModificationException =====

        // 墙壁（复制一份）
        List<Wall> wallsCopy = new ArrayList<>(walls);
        for (Wall wall : wallsCopy) {
            wall.draw(g);
        }

        // 敌人（复制一份）
        List<Tank> enemiesCopy = new ArrayList<>(enemies);
        if (!pvpMode) {
            for (Tank enemy : enemiesCopy) {
                enemy.draw(g);
            }
        }

        // 玩家
        if (player1 != null) player1.draw(g);
        if (player2 != null) player2.draw(g);

        // 子弹 - 收集所有子弹
        List<Bullet> allBullets = new ArrayList<>();
        if (player1 != null) allBullets.addAll(player1.bullets);
        if (player2 != null) allBullets.addAll(player2.bullets);
        if (!pvpMode) {
            for (Tank enemy : enemiesCopy) {
                allBullets.addAll(enemy.bullets);
            }
        }
        for (Bullet b : allBullets) {
            b.draw(g);
        }

        // 爆炸（复制一份）
        List<Explosion> explosionsCopy = new ArrayList<>(explosions);
        for (Explosion e : explosionsCopy) {
            e.draw(g);
        }

        // UI
        drawUI(g);
    }

    private void drawUI(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        // 字体在方法内部创建
        Font fontNormal = new Font("Consolas", Font.PLAIN, 18);
        Font fontBig = new Font("Consolas", Font.BOLD, 36);
        Font fontSmall = new Font("Consolas", Font.PLAIN, 14);

        int uiY = 10;

        // Mode
        String modeText = pvpMode ? "PVP" : "PVE";
        g.setColor(pvpMode ? COLOR_PVP : COLOR_TEXT);
        g.setFont(fontNormal);
        g.drawString(modeText, 10, uiY + 14);
        uiY += 26;

        // Score (PVE only)
        if (!pvpMode) {
            g.setColor(COLOR_TEXT);
            g.setFont(fontNormal);
            g.drawString("Score: " + score, 10, uiY + 14);
            uiY += 26;
        }

        // P1 Lives
        g.setColor(COLOR_PLAYER1);
        g.setFont(fontNormal);
        String p1Text = "P1: ";
        for (int i = 0; i < (player1 != null ? player1.lives : 0); i++) {
            p1Text += "[] ";
        }
        g.drawString(p1Text, 10, uiY + 14);
        uiY += 26;

        // P2 Lives
        g.setColor(COLOR_PLAYER2);
        String p2Text = "P2: ";
        for (int i = 0; i < (player2 != null ? player2.lives : 0); i++) {
            p2Text += "[] ";
        }
        g.drawString(p2Text, 10, uiY + 14);
        uiY += 26;

        // Enemies (PVE only)
        if (!pvpMode) {
            g.setColor(COLOR_TEXT);
            g.drawString("Enemy: " + enemies.size() + "/" + ENEMY_COUNT, 10, uiY + 14);
        }

        // Map name
        g.setColor(COLOR_TEXT_DIM);
        g.setFont(fontSmall);
        String[] mapNames = {"Empty", "Cross", "Maze", "Bunker", "Sym"};
        g.drawString("Map: " + mapNames[currentMap], 10, WINDOW_HEIGHT - 20);

        // Pause
        if (paused) {
            g.setColor(Color.WHITE);
            g.setFont(fontBig);
            g.drawString("PAUSED", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2);
        }

        // Game Over
        if (gameOver) {
            g.setColor(new Color(255, 50, 50));
            g.setFont(fontBig);
            g.drawString("GAME OVER", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 20);
            g.setColor(COLOR_TEXT);
            g.setFont(fontNormal);
            g.drawString("Press R to restart", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 40);
        }

        // Controls
        g.setColor(COLOR_TEXT_DIM);
        g.setFont(fontSmall);
        String[] controls = {
            "P1: Arrows + SPACE",
            "P2: WASD + J",
            "G: PVP/PVE  P: Pause  R: Restart",
            "ESC: Exit"
        };
        for (int i = 0; i < controls.length; i++) {
            g.drawString(controls[i], WINDOW_WIDTH - 200, 10 + i * 20 + 14);
        }
    }

    public boolean isPaused() {
        return paused;
    }

    public void setPaused(boolean p) {
        paused = p;
    }

    public boolean isGameOver() {
        return gameOver;
    }
}