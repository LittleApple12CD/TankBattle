package com.tankbattle;

import java.awt.*;

import static com.tankbattle.Utils.*;

/**
 * 子弹类
 */
public class Bullet {
    public double x, y;
    public int w, h;
    public Utils.Vec2 dir;
    public double speed;
    public boolean isPlayer;
    public int playerId;
    public Color color;
    public boolean alive;

    public Bullet(double x, double y, Utils.Vec2 dir, boolean isPlayer, int playerId, Color color) {
        this.x = x - BULLET_SIZE / 2.0;
        this.y = y - BULLET_SIZE / 2.0;
        this.w = BULLET_SIZE;
        this.h = BULLET_SIZE;
        this.dir = dir;
        this.speed = BULLET_SPEED;
        this.isPlayer = isPlayer;
        this.playerId = playerId;
        this.color = color;
        this.alive = true;
    }

    public Rectangle getRect() {
        return new Rectangle((int) x, (int) y, w, h);
    }

    public void update(double dt) {
        x += dir.x * speed * dt;
        y += dir.y * speed * dt;

        if (x < -20 || x > WINDOW_WIDTH + 20 || y < -20 || y > WINDOW_HEIGHT + 20) {
            alive = false;
        }
    }

    public void draw(Graphics2D g) {
        if (!alive) return;
        g.setColor(color);
        int cx = (int) (x + w / 2.0);
        int cy = (int) (y + h / 2.0);
        g.fillOval(cx - w / 2, cy - h / 2, w, h);
        g.setColor(Color.WHITE);
        g.fillOval(cx - w / 4, cy - h / 4, w / 2, h / 2);
    }
}