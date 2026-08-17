package com.tankbattle;

import java.awt.*;

import com.tankbattle.resource.TextureManager;

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
        Renderer renderer = Renderer.getInstance();

        if (renderer.drawWallWithTexture(g, this, entityId)) {
            return;
        }

        renderer.drawWallBuiltin(g, this);
    }
}