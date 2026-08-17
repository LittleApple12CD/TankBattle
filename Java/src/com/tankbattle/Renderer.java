package com.tankbattle;

import java.awt.*;
import java.awt.image.BufferedImage;
import com.tankbattle.resource.TextureManager;
import static com.tankbattle.Utils.*;

/**
 * 轻量级渲染器 - 只负责贴图实体的绘制
 * 不包含 UI、HUD、爆炸、痕迹等
 */
public class Renderer {

    private static Renderer instance;

    private Renderer() {}

    public static Renderer getInstance() {
        if (instance == null) {
            instance = new Renderer();
        }
        return instance;
    }

    // ============================================================
    // 坦克绘制
    // ============================================================

    /**
     * 绘制坦克（贴图版本）
     * @param g Graphics2D
     * @param tank 坦克对象
     * @param entityId 贴图ID
     * @param wDraw 绘制宽度
     * @param hDraw 绘制高度
     * @param xDraw 绘制X
     * @param yDraw 绘制Y
     * @return 是否使用了贴图
     */
    public boolean drawTankWithTexture(Graphics2D g, Tank tank, String entityId, 
                                        int wDraw, int hDraw, int xDraw, int yDraw) {
        BufferedImage tex = TextureManager.getEntityTexture(entityId);
        if (tex == null) return false;

        Graphics2D g2d = (Graphics2D) g.create();
        
        double angle = Math.atan2(tank.dir.y, tank.dir.x) + Math.PI / 2;
        double cx = tank.x + tank.w / 2.0;
        double cy = tank.y + tank.h / 2.0;
        g2d.translate(cx, cy);
        g2d.rotate(angle);
        
        int drawX = -wDraw / 2;
        int drawY = -hDraw / 2;
        g2d.drawImage(tex, drawX, drawY, wDraw, hDraw, null);
        
        g2d.dispose();
        return true;
    }

    /**
     * 绘制坦克（内置版本）
     */
    public void drawTankBuiltin(Graphics2D g, Tank tank, Color color,
                                 int wDraw, int hDraw, int xDraw, int yDraw) {
        // 主体
        g.setColor(color);
        g.fillRoundRect(xDraw, yDraw, wDraw, hDraw, 6, 6);

        // 边框
        if (tank.isProtected()) {
            g.setColor(Color.WHITE);
            g.setStroke(new BasicStroke(3));
            g.drawRoundRect(xDraw, yDraw, wDraw, hDraw, 6, 6);
            g.setStroke(new BasicStroke(1));
        } else {
            g.setColor(Color.WHITE);
            g.drawRoundRect(xDraw, yDraw, wDraw, hDraw, 6, 6);
        }

        // 炮塔
        int cx = (int)(tank.x + tank.w / 2.0);
        int cy = (int)(tank.y + tank.h / 2.0);
        g.setColor(Color.WHITE);
        g.fillOval(cx - tank.w / 6, cy - tank.h / 6, tank.w / 3, tank.h / 3);

        // 炮管
        int endX = cx + (int)(tank.dir.x * (tank.w / 2.0 + 2));
        int endY = cy + (int)(tank.dir.y * (tank.h / 2.0 + 2));
        g.setStroke(new BasicStroke(4));
        g.drawLine(cx, cy, endX, endY);
        g.setStroke(new BasicStroke(1));
    }

    // ============================================================
    // Boss 血条
    // ============================================================

    public void drawBossHealthBar(Graphics2D g, Tank tank, int maxHp) {
        int barWidth = tank.w + 10;
        int barHeight = 6;
        int barX = (int)(tank.x + tank.w / 2.0 - barWidth / 2.0);
        int barY = (int)(tank.y - 12);
        float hpRatio = (float) tank.lives / maxHp;

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

    // ============================================================
    // 子弹绘制
    // ============================================================

    /**
     * 绘制子弹（贴图版本）
     */
    public boolean drawBulletWithTexture(Graphics2D g, Bullet bullet, String entityId) {
        BufferedImage tex = TextureManager.getEntityTexture(entityId);
        if (tex == null) return false;

        int cx = (int)(bullet.x + bullet.w / 2.0);
        int cy = (int)(bullet.y + bullet.h / 2.0);
        g.drawImage(tex, cx - bullet.w / 2, cy - bullet.h / 2, bullet.w, bullet.h, null);
        return true;
    }

    /**
     * 绘制子弹（内置版本）
     */
    public void drawBulletBuiltin(Graphics2D g, Bullet bullet) {
        int cx = (int)(bullet.x + bullet.w / 2.0);
        int cy = (int)(bullet.y + bullet.h / 2.0);
        g.setColor(bullet.color);
        g.fillOval(cx - bullet.w / 2, cy - bullet.h / 2, bullet.w, bullet.h);
        g.setColor(Color.WHITE);
        g.fillOval(cx - bullet.w / 4, cy - bullet.h / 4, bullet.w / 2, bullet.h / 2);
    }

    // ============================================================
    // 墙壁绘制
    // ============================================================

    /**
     * 绘制墙壁（贴图版本）
     */
    public boolean drawWallWithTexture(Graphics2D g, Wall wall, String entityId) {
        BufferedImage tex = TextureManager.getEntityTexture(entityId);
        if (tex == null) return false;
        g.drawImage(tex, wall.x, wall.y, wall.w, wall.h, null);
        return true;
    }

    /**
     * 绘制墙壁（内置版本）
     */
    public void drawWallBuiltin(Graphics2D g, Wall wall) {
        Color color = wall.isSteel ? COLOR_STEEL : COLOR_WALL;
        g.setColor(color);
        g.fillRoundRect(wall.x, wall.y, wall.w, wall.h, 4, 4);
        g.setColor(Color.WHITE);
        g.drawRoundRect(wall.x, wall.y, wall.w, wall.h, 4, 4);
    }

    // ============================================================
    // 道具绘制
    // ============================================================

    private static final Color[] POWERUP_COLORS = {
        new Color(50, 150, 255),   // S - Speed
        new Color(255, 255, 255),  // P - Protection
        new Color(255, 50, 50),    // H - Health
        new Color(255, 150, 50)    // T - Strength
    };

    /**
     * 绘制道具（贴图版本）
     */
    public boolean drawPowerUpWithTexture(Graphics2D g, PowerUp powerup, String entityId) {
        BufferedImage tex = TextureManager.getEntityTexture(entityId);
        if (tex == null) return false;
        g.drawImage(tex, powerup.x, powerup.y, powerup.w, powerup.h, null);
        return true;
    }

    /**
     * 绘制道具（内置版本）
     */
    public void drawPowerUpBuiltin(Graphics2D g, PowerUp powerup) {
        Color color;
        switch (powerup.type) {
            case 'S': color = POWERUP_COLORS[0]; break;
            case 'P': color = POWERUP_COLORS[1]; break;
            case 'H': color = POWERUP_COLORS[2]; break;
            case 'T': color = POWERUP_COLORS[3]; break;
            default: color = Color.WHITE;
        }
        g.setColor(color);
        g.fillRect(powerup.x, powerup.y, powerup.w, powerup.h);
        g.setColor(Color.WHITE);
        g.drawRect(powerup.x, powerup.y, powerup.w, powerup.h);
        g.setColor(Color.BLACK);
        g.setFont(new Font("Consolas", Font.BOLD, 14));
        g.drawString(String.valueOf(powerup.type), powerup.x + 7, powerup.y + 17);
    }
}