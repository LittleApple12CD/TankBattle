package com.tankbattle;

import java.awt.*;
import java.util.ArrayList;
import java.util.Iterator;

import static com.tankbattle.Utils.*;

/**
 * 子弹类（含拖尾效果）
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

    // ===== 拖尾 =====
    private static class TrailParticle {
        double x, y;
        double size;
        double age;
        double lifetime = 0.25;  // 拖尾寿命（秒）
    }
    private ArrayList<TrailParticle> trail;

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
        this.trail = new ArrayList<>();
    }

    public Rectangle getRect() {
        return new Rectangle((int) x, (int) y, w, h);
    }

    public void update(double dt) {
        // 添加拖尾粒子
        TrailParticle p = new TrailParticle();
        p.x = this.x + this.w / 2.0;
        p.y = this.y + this.h / 2.0;
        p.size = this.w * 0.6;
        p.lifetime = 0.15;
        p.age = 0;
        trail.add(p);

        // 更新位置
        x += dir.x * speed * dt;
        y += dir.y * speed * dt;

        // 更新拖尾
        Iterator<TrailParticle> it = trail.iterator();
        while (it.hasNext()) {
            TrailParticle tp = it.next();
            tp.age += dt;
            tp.size *= (1.0 - dt / tp.lifetime);
            if (tp.age >= tp.lifetime || tp.size < 0.5) {
                it.remove();
            }
        }

        if (x < -20 || x > WINDOW_WIDTH + 20 || y < -20 || y > WINDOW_HEIGHT + 20) {
            alive = false;
        }
    }

    public void draw(Graphics2D g) {
        // ===== 画拖尾（用同步块 + 复制） =====
        ArrayList<TrailParticle> trailCopy;
        synchronized (trail) {
            trailCopy = new ArrayList<>(trail);
        }
    
        for (TrailParticle tp : trailCopy) {
            double alpha = 1.0 - tp.age / tp.lifetime;
            int size = (int) Math.round(tp.size);
            if (size > 0 && alpha > 0.05) {
                int a = (int) (alpha * 200);
                Color col = new Color(
                    color.getRed(),
                    color.getGreen(),
                    color.getBlue(),
                    Math.min(a, 255)
                );
                g.setColor(col);
                g.fillOval(
                    (int) (tp.x - size),
                    (int) (tp.y - size),
                    size * 2,
                    size * 2
                );
            }
        }

        // ===== 画子弹 =====
        if (!alive) return;
        g.setColor(color);
        int cx = (int) (x + w / 2.0);
        int cy = (int) (y + h / 2.0);
        g.fillOval(cx - w / 2, cy - h / 2, w, h);
        g.setColor(Color.WHITE);
        g.fillOval(cx - w / 4, cy - h / 4, w / 2, h / 2);
    }
}