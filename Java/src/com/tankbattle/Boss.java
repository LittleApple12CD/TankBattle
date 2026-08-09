// src/com/tankbattle/Boss.java
package com.tankbattle;

import java.awt.*;
import java.awt.image.BufferedImage;
import com.tankbattle.resource.TextureManager;
import static com.tankbattle.Utils.*;

public class Boss extends Tank {

    private int maxHp;
    private double sizeMult;
    private double speedMult;
    private int bulletDamage;
    private double bulletSpeedMult;
    public boolean isBoss;

    public Boss(double x, double y, int hp, double sizeMult, double speedMult,
                int bulletDamage, double bulletSpeedMult) {
        super(x, y, new Color(200, 50, 200), TANK_SPEED * speedMult, false, 0);
        this.maxHp = hp;
        this.lives = hp;
        this.sizeMult = sizeMult;
        this.speedMult = speedMult;
        this.bulletDamage = bulletDamage;
        this.bulletSpeedMult = bulletSpeedMult;
        this.isBoss = true;

        this.w = (int)(TANK_SIZE * sizeMult);
        this.h = (int)(TANK_SIZE * sizeMult);
        this.x = x;
        this.y = y;
    }

    @Override
    public Vec2 getFirePoint() {
        Vec2 center = getCenter();
        double offset = w / 2.0 + 2;
        return new Vec2(center.x + dir.x * offset, center.y + dir.y * offset);
    }

    @Override
    public Bullet shoot() {
        if (cooldown > 0) return null;
        if (bullets.size() >= MAX_BULLETS) return null;

        cooldown = SHOT_COOLDOWN * 0.7;
        Vec2 firePoint = getFirePoint();

        int bulletSize = (int)(BULLET_SIZE * 1.5);
        double bulletSpeed = BULLET_SPEED * bulletSpeedMult;

        Bullet bullet = new Bullet(firePoint.x, firePoint.y, dir, false, 0, COLOR_BULLET_ENEMY);
        bullet.w = bulletSize;
        bullet.h = bulletSize;
        bullet.speed = bulletSpeed;
        bullet.damage = bulletDamage;
        bullets.add(bullet);
        return bullet;
    }

    @Override
    public void draw(Graphics2D g) {
        if (!alive) return;

        // ===== 尝试获取贴图 =====
        BufferedImage tex = TextureManager.getEntityTexture("tank_boss");

        if (tex != null) {
            Graphics2D g2d = (Graphics2D) g.create();
            
            double angle = Math.atan2(dir.y, dir.x) + Math.PI / 2;
            double cx = x + w / 2.0;
            double cy = y + h / 2.0;
            g2d.translate(cx, cy);
            g2d.rotate(angle);
            
            int drawX = -w / 2;
            int drawY = -h / 2;
            g2d.drawImage(tex, drawX, drawY, w, h, null);
            
            g2d.dispose();
            
            drawBossHealthBar(g);
            return;
        }

        // ===== 无贴图：回退到内置绘制 =====
        int cx = (int)(x + w / 2.0);
        int cy = (int)(y + h / 2.0);
        int xDraw = (int)x;
        int yDraw = (int)y;

        // Boss 主体
        g.setColor(new Color(180, 50, 200));
        g.fillRoundRect(xDraw, yDraw, w, h, 6, 6);
        g.setColor(new Color(255, 215, 0));
        g.setStroke(new BasicStroke(3));
        g.drawRoundRect(xDraw, yDraw, w, h, 6, 6);
        g.setStroke(new BasicStroke(1));

        // 炮塔
        g.setColor(new Color(255, 215, 0));
        g.fillOval(cx - w / 6, cy - h / 6, w / 3, h / 3);

        // 炮管
        int endX = cx + (int)(dir.x * (w / 2.0 + 2));
        int endY = cy + (int)(dir.y * (h / 2.0 + 2));
        g.setStroke(new BasicStroke(4));
        g.setColor(new Color(255, 215, 0));
        g.drawLine(cx, cy, endX, endY);
        g.setStroke(new BasicStroke(1));

        drawBossHealthBar(g);
    }

    @Override
    public void drawBossHealthBar(Graphics2D g) {
        int barWidth = w + 10;
        int barHeight = 6;
        int barX = (int)(x + w/2.0 - barWidth/2.0);
        int barY = (int)(y - 12);
        float hpRatio = (float) lives / maxHp;

        g.setColor(new Color(60, 60, 60));
        g.fillRect(barX, barY, barWidth, barHeight);
        
        if (hpRatio > 0.5f) {
            g.setColor(new Color(0, 200, 0));
        } else if (hpRatio > 0.25f) {
            g.setColor(new Color(200, 200, 0));
        } else {
            g.setColor(new Color(200, 50, 50));
        }
        g.fillRect(barX, barY, (int)(barWidth * hpRatio), barHeight);
    }
}