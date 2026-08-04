package com.tankbattle;

import java.awt.*;

/**
 * 爆炸效果类
 */
public class Explosion {
    public double x, y;
    public double radius;
    public double maxRadius;
    public double age;
    public double lifetime;
    public boolean alive;

    private static final Color[] COLORS = {
            new Color(255, 255, 200),
            new Color(255, 200, 100),
            new Color(255, 150, 50),
            new Color(255, 100, 20),
            new Color(200, 50, 0)
    };

    public Explosion(double x, double y) {
        this.x = x;
        this.y = y;
        this.radius = 5;
        this.maxRadius = 30;
        this.age = 0;
        this.lifetime = 0.5;
        this.alive = true;
    }

    public void update(double dt) {
        age += dt;
        double progress = age / lifetime;
        radius = maxRadius * progress;
        if (age >= lifetime) {
            alive = false;
        }
    }

    public void draw(Graphics2D g) {
        if (!alive) return;
        double progress = age / lifetime;
        int r = (int) radius;
        int idx = Math.min((int) (progress * COLORS.length), COLORS.length - 1);
        g.setColor(COLORS[idx]);
        g.fillOval((int) (x - r), (int) (y - r), r * 2, r * 2);
    }
}