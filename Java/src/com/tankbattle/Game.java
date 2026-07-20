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
    private boolean singleMode;
    private int enemyCount;
    private java.util.List<PowerUp> powerups = new java.util.ArrayList<>();
    private float powerupTimer = 0;
    private float powerupInterval = 10.0f;
    private int maxPowerups = 3;
    private Random random;
    private Font fontNormal;
    private Font fontBig;
    private Font fontSmall;

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
        singleMode = false;
        enemyCount = ENEMY_COUNT;

        fontNormal = new Font("Consolas", Font.PLAIN, 18);
        fontBig = new Font("Consolas", Font.BOLD, 36);
        fontSmall = new Font("Consolas", Font.PLAIN, 14);

        initLevel();
    }

    public List<Wall> getWalls() { return walls; }
    public Tank getPlayer1() { return player1; }
    public Tank getPlayer2() { return player2; }
    public boolean isPaused() { return paused; }
    public boolean isGameOver() { return gameOver; }
    public void setPaused(boolean p) { paused = p; }
    public void setPvpMode(boolean b) { pvpMode = b; }
    public void setSingleMode(boolean b) { 
        singleMode = b; 
        if (singleMode) pvpMode = false;
        initLevel(); 
    }

    public void togglePvpMode() {
        pvpMode = !pvpMode;
        initLevel();
    }

    public void toggleSingleMode() {
        singleMode = !singleMode;
        if (singleMode) pvpMode = false;
        initLevel();
    }

    public void initLevel() {
        gameOver = false;
        paused = false;
        score = 0;
        enemySpawnTimer = 0;
        enemyCount = ENEMY_COUNT;
        powerups.clear();
        powerupTimer = 0;

        walls.clear();
        explosions.clear();
        enemies.clear();
        enemyAIs.clear();

        currentMap = random.nextInt(5);
        MapGenerator.generateMap(currentMap, walls);

        // P1 出生在左下
        float p1x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE / 2.0f - TANK_SIZE / 2.0f;
        float p1y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE / 2.0f - TANK_SIZE / 2.0f;
        if (player1 == null) {
            player1 = new Tank(p1x, p1y, COLOR_PLAYER1, TANK_SPEED, true, 1);
        } else {
            player1.x = p1x;
            player1.y = p1y;
            player1.alive = true;
            player1.lives = PLAYER_LIVES;
            player1.effects.clear();
        }

        // P2：只在双人模式创建
        if (!singleMode) {
            float p2x = GRID_OFFSET_X + (GRID_SIZE - 2) * CELL_SIZE + CELL_SIZE / 2.0f - TANK_SIZE / 2.0f;
            float p2y = GRID_OFFSET_Y + CELL_SIZE + CELL_SIZE / 2.0f - TANK_SIZE / 2.0f;
            if (player2 == null) {
                player2 = new Tank(p2x, p2y, COLOR_PLAYER2, TANK_SPEED, true, 2);
            } else {
                player2.x = p2x;
                player2.y = p2y;
                player2.alive = true;
                player2.lives = PLAYER_LIVES;
                player2.effects.clear();
            }
        } else {
            player2 = null;
        }

        if (!pvpMode) {
            for (int i = 0; i < enemyCount; i++) {
                spawnEnemy();
            }
        }
    }

    private void spawnEnemy() {
        if (enemies.size() >= enemyCount) return;

        int[][] spawnPos = {{GRID_SIZE - 2, 1}, {GRID_SIZE / 2, 1}, {1, 1}};
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

        if (player1 != null && player1.alive) player1.update(dt);
        if (!singleMode && player2 != null && player2.alive) player2.update(dt);

        updatePowerups((float)dt);

        if (!pvpMode) {
            for (EnemyAI ai : enemyAIs) {
                ai.update(dt);
            }

            for (int i = enemies.size() - 1; i >= 0; i--) {
                if (!enemies.get(i).alive) {
                    enemies.remove(i);
                    enemyAIs.remove(i);
                }
            }

            enemySpawnTimer -= dt;
            if (enemySpawnTimer <= 0 && enemies.size() < enemyCount) {
                spawnEnemy();
                enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
            }
        }

        handleBulletCollisions();

        for (int i = explosions.size() - 1; i >= 0; i--) {
            explosions.get(i).update(dt);
            if (!explosions.get(i).alive) {
                explosions.remove(i);
            }
        }

        if (pvpMode) {
            if ((player1 != null && !player1.alive) || (player2 != null && !player2.alive)) {
                gameOver = true;
            }
        } else {
            if (singleMode) {
                if (player1 != null && !player1.alive) gameOver = true;
            } else {
                if (player1 != null && !player1.alive && player2 != null && !player2.alive) {
                    gameOver = true;
                }
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

        for (int i = allBullets.size() - 1; i >= 0; i--) {
            Bullet bullet = allBullets.get(i);
            if (!bullet.alive) continue;

            boolean bulletHit = false;

            // 子弹 vs 墙壁
            for (Wall wall : walls) {
                if (!wall.alive) continue;
                if (bullet.getRect().intersects(wall.getRect())) {
                    if (bullet.damage == 2 && bullet.isPlayer) {
                        explodeAt((int)bullet.x + bullet.w / 2, (int)bullet.y + bullet.h / 2);
                        bullet.alive = false;
                        bulletHit = true;
                        if (!wall.isSteel) wall.alive = false;
                        addExplosion(bullet.x + bullet.w / 2, bullet.y + bullet.h / 2);
                    } else {
                        bullet.alive = false;
                        bulletHit = true;
                        if (!wall.isSteel) wall.alive = false;
                        addExplosion(bullet.x + bullet.w / 2, bullet.y + bullet.h / 2);
                    }
                    break;
                }
            }

            if (bulletHit || !bullet.alive) continue;

            if (pvpMode) {
                // P1 vs P2
                if (bullet.isPlayer && bullet.playerId == 1) {
                    if (player2 != null && player2.alive && bullet.getRect().intersects(player2.getRect())) {
                        if (bullet.damage == 2 && bullet.isPlayer) {
                            explodeAt((int)bullet.x + bullet.w / 2, (int)bullet.y + bullet.h / 2);
                            bullet.alive = false;
                        } else {
                            bullet.alive = false;
                            if (player2.effects.containsKey("protection")) {
                                player2.effects.remove("protection");
                            } else {
                                player2.lives -= bullet.damage;
                                addExplosion(player2.x + player2.w / 2, player2.y + player2.h / 2);
                            }
                            if (player2.lives <= 0) player2.alive = false;
                        }
                        continue;
                    }
                }
                if (bullet.isPlayer && bullet.playerId == 2) {
                    if (player1 != null && player1.alive && bullet.getRect().intersects(player1.getRect())) {
                        if (bullet.damage == 2 && bullet.isPlayer) {
                            explodeAt((int)bullet.x + bullet.w / 2, (int)bullet.y + bullet.h / 2);
                            bullet.alive = false;
                        } else {
                            bullet.alive = false;
                            if (player1.effects.containsKey("protection")) {
                                player1.effects.remove("protection");
                            } else {
                                player1.lives -= bullet.damage;
                                addExplosion(player1.x + player1.w / 2, player1.y + player1.h / 2);
                            }
                            if (player1.lives <= 0) player1.alive = false;
                        }
                        continue;
                    }
                }
            } else {
                // PVE 模式
                if (bullet.isPlayer) {
                    for (Tank enemy : enemies) {
                        if (!enemy.alive) continue;
                        if (bullet.getRect().intersects(enemy.getRect())) {
                            if (bullet.damage == 2 && bullet.isPlayer) {
                                explodeAt((int)bullet.x + bullet.w / 2, (int)bullet.y + bullet.h / 2);
                                bullet.alive = false;
                            } else {
                                bullet.alive = false;
                                if (enemy.effects.containsKey("protection")) {
                                    enemy.effects.remove("protection");
                                } else {
                                    enemy.lives -= bullet.damage;
                                }
                                if (enemy.lives <= 0) {
                                    enemy.alive = false;
                                    score += 10;
                                    addExplosion(enemy.x + enemy.w / 2, enemy.y + enemy.h / 2);
                                }
                            }
                            break;
                        }
                    }
                } else {
                    for (Tank player : new Tank[]{player1, player2}) {
                        if (player == null || !player.alive) continue;
                        if (bullet.getRect().intersects(player.getRect())) {
                            if (bullet.damage == 2 && bullet.isPlayer) {
                                explodeAt((int)bullet.x + bullet.w / 2, (int)bullet.y + bullet.h / 2);
                                bullet.alive = false;
                            } else {
                                bullet.alive = false;
                                if (player.effects.containsKey("protection")) {
                                    player.effects.remove("protection");
                                } else {
                                    player.lives -= bullet.damage;
                                    addExplosion(player.x + player.w / 2, player.y + player.h / 2);
                                }
                                if (player.lives <= 0) player.alive = false;
                            }
                            break;
                        }
                    }
                }
            }
        }

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

    public void draw(Graphics2D g) {
        g.setColor(COLOR_BG);
        g.fillRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        g.setColor(COLOR_GRID);
        for (int i = 0; i <= GRID_SIZE; i++) {
            int x = GRID_OFFSET_X + i * CELL_SIZE;
            int y = GRID_OFFSET_Y + i * CELL_SIZE;
            g.drawLine(x, GRID_OFFSET_Y, x, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE);
            g.drawLine(GRID_OFFSET_X, y, GRID_OFFSET_X + GRID_SIZE * CELL_SIZE, y);
        }

        g.setColor(new Color(60, 60, 80));
        g.drawRect(GRID_OFFSET_X, GRID_OFFSET_Y, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE);

        List<Wall> wallsCopy = new ArrayList<>(walls);
        for (Wall wall : wallsCopy) wall.draw(g);

        List<Tank> enemiesCopy = new ArrayList<>(enemies);
        if (!pvpMode) {
            for (Tank enemy : enemiesCopy) enemy.draw(g);
        }

        if (player1 != null) player1.draw(g);
        if (player2 != null) player2.draw(g);

        List<Bullet> allBullets = new ArrayList<>();
        if (player1 != null) allBullets.addAll(player1.bullets);
        if (player2 != null) allBullets.addAll(player2.bullets);
        if (!pvpMode) {
            for (Tank enemy : enemiesCopy) {
                allBullets.addAll(enemy.bullets);
            }
        }
        for (Bullet b : allBullets) b.draw(g);

        List<Explosion> explosionsCopy = new ArrayList<>(explosions);
        for (Explosion e : explosionsCopy) e.draw(g);

        for (PowerUp p : powerups) p.draw(g);

        drawUI(g);
    }

    private void drawUI(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

        int uiY = 10;

        String modeText = pvpMode ? "PVP" : "PVE";
        g.setColor(pvpMode ? COLOR_PVP : COLOR_TEXT);
        g.setFont(fontNormal);
        g.drawString(modeText, 10, uiY + 14);
        uiY += 26;

        if (!pvpMode) {
            g.setColor(COLOR_TEXT);
            g.setFont(fontNormal);
            g.drawString("Score: " + score, 10, uiY + 14);
            uiY += 26;
        }

        g.setColor(COLOR_PLAYER1);
        g.setFont(fontNormal);
        String p1Text = "P1: ";
        for (int i = 0; i < (player1 != null ? player1.lives : 0); i++) p1Text += "[] ";
        g.drawString(p1Text, 10, uiY + 14);
        uiY += 26;

        g.setColor(COLOR_PLAYER2);
        String p2Text = "P2: ";
        for (int i = 0; i < (player2 != null ? player2.lives : 0); i++) p2Text += "[] ";
        g.drawString(p2Text, 10, uiY + 14);
        uiY += 26;

        if (!pvpMode) {
            g.setColor(COLOR_TEXT);
            g.drawString("Enemy: " + enemies.size() + "/" + enemyCount, 10, uiY + 14);
        }

        g.setColor(COLOR_TEXT_DIM);
        g.setFont(fontSmall);
        String[] mapNames = {"Empty", "Cross", "Maze", "Bunker", "Sym"};
        g.drawString("Map: " + mapNames[currentMap], 10, WINDOW_HEIGHT - 20);

        if (paused) {
            g.setColor(Color.WHITE);
            g.setFont(fontBig);
            g.drawString("PAUSED", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2);
        }

        if (gameOver) {
            g.setColor(new Color(255, 50, 50));
            g.setFont(fontBig);
            g.drawString("GAME OVER", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 20);
            g.setColor(COLOR_TEXT);
            g.setFont(fontNormal);
            g.drawString("Press R to restart", WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 40);
        }

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

    private void updatePowerups(float dt) {
        powerupTimer += dt;
        if (powerupTimer >= powerupInterval && powerups.size() < maxPowerups) {
            spawnPowerup();
            powerupTimer = 0;
        }

        for (int i = powerups.size() - 1; i >= 0; i--) {
            PowerUp p = powerups.get(i);
            if (!p.alive) {
                powerups.remove(i);
                continue;
            }
            for (Tank tank : getAllTanks()) {
                if (tank.alive && p.getRect().intersects(tank.getRect())) {
                    applyPowerup(tank, p);
                    p.alive = false;
                    powerups.remove(i);
                    break;
                }
            }
        }
    }

    private java.util.List<Tank> getAllTanks() {
        java.util.List<Tank> list = new java.util.ArrayList<>();
        if (player1 != null) list.add(player1);
        if (player2 != null) list.add(player2);
        list.addAll(enemies);
        return list;
    }

    private void spawnPowerup() {
        int margin = 40;
        char[] types = {'S', 'P', 'H', 'T'};
        for (int attempt = 0; attempt < 20; attempt++) {
            int x = GRID_OFFSET_X + margin + (int)(Math.random() * (GRID_SIZE * CELL_SIZE - margin * 2 - 24));
            int y = GRID_OFFSET_Y + margin + (int)(Math.random() * (GRID_SIZE * CELL_SIZE - margin * 2 - 24));
            Rectangle testRect = new Rectangle(x, y, 24, 24);
            boolean blocked = false;
            for (Wall wall : walls) {
                if (wall.alive && testRect.intersects(wall.getRect())) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                char type = types[(int)(Math.random() * types.length)];
                powerups.add(new PowerUp(x, y, type));
                return;
            }
        }
    }

    private void applyPowerup(Tank tank, PowerUp powerup) {
        char type = powerup.type;
        switch (type) {
            case 'H':
                tank.lives = Math.min(tank.lives + 1, 5);
                break;
            case 'S':
                tank.effects.put("speed", 5.0f);
                break;
            case 'P':
                tank.effects.put("protection", 5.0f);
                break;
            case 'T':
                tank.effects.put("strength", 5.0f);
                break;
        }
    }

    private void explodeAt(int x, int y) {
        int radius = TANK_SIZE * 2;

        for (Tank enemy : enemies) {
            if (!enemy.alive) continue;
            double dist = Math.hypot(enemy.x + enemy.w/2 - x, enemy.y + enemy.h/2 - y);
            if (dist <= radius) {
                if (enemy.effects.containsKey("protection")) {
                    enemy.effects.remove("protection");
                } else {
                    enemy.lives -= 2;
                    if (enemy.lives <= 0) {
                        enemy.alive = false;
                        score += 10;
                        addExplosion(enemy.x + enemy.w/2, enemy.y + enemy.h/2);
                    }
                }
            }
        }

        if (pvpMode) {
            for (Tank player : new Tank[]{player1, player2}) {
                if (player == null || !player.alive) continue;
                double dist = Math.hypot(player.x + player.w/2 - x, player.y + player.h/2 - y);
                if (dist <= radius) {
                    if (player.effects.containsKey("protection")) {
                        player.effects.remove("protection");
                    } else {
                        player.lives -= 2;
                        addExplosion(player.x + player.w/2, player.y + player.h/2);
                        if (player.lives <= 0) player.alive = false;
                    }
                }
            }
        }

        for (Wall wall : walls) {
            if (!wall.alive || wall.isSteel) continue;
            double dist = Math.hypot(wall.x + wall.w/2 - x, wall.y + wall.h/2 - y);
            if (dist <= radius) {
                wall.alive = false;
            }
        }

        for (int i = 0; i < 8; i++) {
            addExplosion(x + (int)(Math.random() * radius - radius/2),
                        y + (int)(Math.random() * radius - radius/2));
        }
    }
}