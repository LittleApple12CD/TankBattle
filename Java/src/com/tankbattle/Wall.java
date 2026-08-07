package com.tankbattle;

import java.awt.*;

/**
 * 墙壁类
 */
public class Wall {
    public int x, y, w, h;
    public boolean isSteel;
    public boolean alive;

    public Wall(int x, int y, int w, int h, boolean isSteel) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.isSteel = isSteel;
        this.alive = true;
    }

    public Rectangle getRect() {
        return new Rectangle(x, y, w, h);
    }

    public void draw(Graphics2D g) {
        if (!alive) return;

        String entityId = isSteel ? "wall_steel" : "wall_brick";
        java.awt.image.BufferedImage tex = com.tankbattle.resource.TextureManager.getEntityTexture(entityId);
    
        if (tex != null) {
            g.drawImage(tex, x, y, w, h, null);
            return;
        }

        Color color = isSteel ? Utils.COLOR_STEEL : Utils.COLOR_WALL;
        g.setColor(color);
        g.fillRoundRect(x, y, w, h, 4, 4);
        g.setColor(Color.WHITE);
        g.drawRoundRect(x, y, w, h, 4, 4);
    }
}