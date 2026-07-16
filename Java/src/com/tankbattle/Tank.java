package com.tankbattle;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Iterator;

import static com.tankbattle.Utils.*;

/**
 * 坦克类（P1/P2/敌人共用）
 */
public class Tank {
    public double x, y;
    public int w, h;
    public Color color;
    public double speed;
    public boolean isPlayer;
    public int playerId;
    public Utils.Vec2 dir;
    public int lives;
    public double cooldown;
    public boolean alive;
    public List<Bullet> bullets;

    public Tank(double x, double y, Color color, double speed, boolean isPlayer, int playerId) {
        this.x = x;
        this.y = y;
        this.w = TANK_SIZE;
        this.h = TANK_SIZE;
        this.color = color;
        this.speed = speed;
        this.isPlayer = isPlayer;
        this.playerId = playerId;
        this.dir = new Utils.Vec2(0, -1);
        this.lives = isPlayer ? PLAYER_LIVES : 1;
        this.cooldown = 0;
        this.alive = true;
        this.bullets = new ArrayList<>();
        trailPoints = new ArrayList<>();
    }

    private ArrayList<TrailPoint> trailPoints;
    private int frameCounter = 0;

    private class TrailPoint {
        double x, y, age;
    }

    public Rectangle getRect() {
        return new Rectangle((int) x, (int) y, w, h);
    }

    public Utils.Vec2 getCenter() {
        return new Utils.Vec2(x + w / 2.0, y + h / 2.0);
    }

    public Utils.Vec2 getFirePoint() {
        Utils.Vec2 center = getCenter();
        double offset = w / 2.0 + 2;
        return new Utils.Vec2(center.x + dir.x * offset, center.y + dir.y * offset);
    }

    public void move(int dx, int dy, java.util.List<Wall> walls) {
        if (dx == 0 && dy == 0) return;

        double step = MOVE_STEP;
        double stepDx = dx * step;
        double stepDy = dy * step;

        int totalSteps = (int) (speed / step);
        if (totalSteps < 1) totalSteps = 1;

        for (int i = 0; i < totalSteps; i++) {
            double newX = x + stepDx;
            double newY = y + stepDy;

            int margin = 2;
            double maxX = WINDOW_WIDTH - w - margin;
            double maxY = WINDOW_HEIGHT - h - margin;
            newX = clamp(newX, margin, maxX);
            newY = clamp(newY, margin, maxY);

            Rectangle testRect = new Rectangle((int) newX, (int) newY, w, h);
            boolean blocked = false;
            for (Wall wall : walls) {
                if (wall.alive && testRect.intersects(wall.getRect())) {
                    blocked = true;
                    break;
                }
            }

            if (!blocked) {
                x = newX;
                y = newY;
            } else {
                // 单轴尝试
                if (dx != 0) {
                    testRect = new Rectangle((int) newX, (int) y, w, h);
                    blocked = false;
                    for (Wall wall : walls) {
                        if (wall.alive && testRect.intersects(wall.getRect())) {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked) {
                        x = newX;
                    }
                }
                if (dy != 0) {
                    testRect = new Rectangle((int) x, (int) newY, w, h);
                    blocked = false;
                    for (Wall wall : walls) {
                        if (wall.alive && testRect.intersects(wall.getRect())) {
                            blocked = true;
                            break;
                        }
                    }
                    if (!blocked) {
                        y = newY;
                    }
                }
                break;
            }
        }
    }

    public Bullet shoot() {
        if (cooldown > 0) return null;
        if (bullets.size() >= MAX_BULLETS) return null;

        cooldown = SHOT_COOLDOWN;
        Utils.Vec2 firePoint = getFirePoint();

        Color bulletColor;
        if (isPlayer) {
            bulletColor = (playerId == 1) ? COLOR_BULLET_P1 : COLOR_BULLET_P2;
        } else {
            bulletColor = COLOR_BULLET_ENEMY;
        }

        Bullet bullet = new Bullet(firePoint.x, firePoint.y, dir, isPlayer, playerId, bulletColor);
        bullets.add(bullet);
        return bullet;
    }

    public void update(double dt) {
        if (cooldown > 0) {
            cooldown -= dt;
        }

        for (int i = bullets.size() - 1; i >= 0; i--) {
            Bullet b = bullets.get(i);
            b.update(dt);
            if (!b.alive) {
                bullets.remove(i);
            }
        }

        frameCounter++;
        if (frameCounter % 3 == 0 && alive) {
            TrailPoint tp = new TrailPoint();
            tp.x = this.x + this.w/2;
            tp.y = this.y + this.h/2;
            tp.age = 0;
            trailPoints.add(tp);
        }

        Iterator<TrailPoint> it = trailPoints.iterator();
        while (it.hasNext()) {
            TrailPoint tp = it.next();
            tp.age += dt;
            if (tp.age >= 1.0) {
                it.remove();
            }
        }
    }

    public void draw(Graphics2D g) {
        // ===== 行驶痕迹（用同步块 + 复制） =====
        ArrayList<TrailPoint> trailCopy;
        synchronized (trailPoints) {
            trailCopy = new ArrayList<>(trailPoints);
        }
    
        for (TrailPoint tp : trailCopy) {
            double alpha = 60 * (1 - tp.age / 1.0);
            if (alpha > 5) {
                int a = (int) alpha;
                g.setColor(new Color(0, 0, 0, Math.min(a, 255)));
                g.fillRect((int) (tp.x - w / 2.0), (int) (tp.y - h / 2.0), w, h);
            }
        }

    if (!alive) return;

        // 坦克主体
        g.setColor(color);
        g.fillRoundRect((int) x, (int) y, w, h, 6, 6);
        g.setColor(Color.WHITE);
        g.drawRoundRect((int) x, (int) y, w, h, 6, 6);

        // 炮塔
        Utils.Vec2 center = getCenter();
        int cx = (int) center.x;
        int cy = (int) center.y;
        g.setColor(Color.WHITE);
        g.fillOval(cx - w / 6, cy - w / 6, w / 3, h / 3);

        // 炮管
        int endX = cx + (int) (dir.x * (w / 2.0 + 2));
        int endY = cy + (int) (dir.y * (h / 2.0 + 2));
        g.setStroke(new BasicStroke(4));
        g.drawLine(cx, cy, endX, endY);
        g.setStroke(new BasicStroke(1));

        // 玩家编号
        if (isPlayer) {
            g.setColor(Color.BLACK);
            g.setFont(new Font("Consolas", Font.BOLD, 14));
            String label = String.valueOf(playerId);
            FontMetrics fm = g.getFontMetrics();
            int tw = fm.stringWidth(label);
            g.drawString(label, (int) (x + w / 2 - tw / 2), (int) (y + h / 2 + 5));
        }
    }
}