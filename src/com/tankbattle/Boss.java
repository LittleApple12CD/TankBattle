package com.tankbattle;

import java.awt.*;
import static com.tankbattle.Utils.*;

/**
 * Boss 坦克
 */
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

        // 重新设置尺寸
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

        int xDraw = (int)x;
        int yDraw = (int)y;

        // Boss 主体
        g.setColor(new Color(180, 50, 200));
        g.fillRoundRect(xDraw, yDraw, w, h, 6, 6);
        g.setColor(new Color(255, 215, 0));
        g.setStroke(new BasicStroke(3));
        g.drawRoundRect(xDraw, yDraw, w, h, 6, 6);
        g.setStroke(new BasicStroke(1));

        // Boss 标志 - 星星
        Vec2 center = getCenter();
        int cx = (int)center.x;
        int cy = (int)center.y;
        g.setColor(new Color(255, 215, 0));
        g.setFont(new Font("Consolas", Font.BOLD, 20));
        g.drawString("★", cx - 10, cy + 8);

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

        // 血条（居中）
        int barWidth = w + 10;
        int barHeight = 6;
        int barX = cx - barWidth / 2;
        int barY = yDraw - 12;
        double hpRatio = (double) lives / maxHp;

        g.setColor(new Color(60, 60, 60));
        g.fillRect(barX, barY, barWidth, barHeight);
        if (hpRatio > 0.5) {
            g.setColor(new Color(0, 200, 0));
        } else if (hpRatio > 0.25) {
            g.setColor(new Color(200, 200, 0));
        } else {
            g.setColor(new Color(200, 50, 50));
        }
        g.fillRect(barX, barY, (int)(barWidth * hpRatio), barHeight);
    }
}