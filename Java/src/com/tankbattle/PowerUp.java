package com.tankbattle;

import java.awt.*;

import com.tankbattle.resource.TextureManager;

import static com.tankbattle.Utils.*;

public class PowerUp {
    public int x, y, w, h;
    public char type;  // 'S', 'P', 'H', 'T'
    public boolean alive;

    private static final Color[] COLORS = {
        new Color(50, 150, 255),   // S - Speed
        new Color(255, 255, 255),  // P - Protection
        new Color(255, 50, 50),    // H - Health
        new Color(255, 150, 50)    // T - Strength
    };

    public PowerUp(int x, int y, char type) {
        this.x = x;
        this.y = y;
        this.w = 24;
        this.h = 24;
        this.type = type;
        this.alive = true;
    }

    public Rectangle getRect() {
        return new Rectangle(x, y, w, h);
    }

    public void draw(Graphics2D g) {
        if (!alive) return;

        String entityId = "powerup_" + type;
        Renderer renderer = Renderer.getInstance();

        if (renderer.drawPowerUpWithTexture(g, this, entityId)) {
            return;
        }

        renderer.drawPowerUpBuiltin(g, this);
    }
}