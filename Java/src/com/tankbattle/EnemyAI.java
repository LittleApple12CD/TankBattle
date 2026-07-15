package com.tankbattle;

import java.util.Random;

import static com.tankbattle.Utils.*;

/**
 * 敌人AI
 */
public class EnemyAI {
    private Tank tank;
    private Game game;
    private Random random;

    private double dirChangeTimer;
    private double shootTimer;
    private Utils.Vec2 direction;

    public EnemyAI(Tank tank, Game game) {
        this.tank = tank;
        this.game = game;
        this.random = new Random();
        this.direction = new Utils.Vec2(0, 1);
        this.dirChangeTimer = AI_DIRECTION_CHANGE + random.nextDouble();
        this.shootTimer = 0.5 + random.nextDouble();
    }

    public void update(double dt) {
        // 更新坦克
        tank.update(dt);

        // 改变方向
        dirChangeTimer -= dt;
        if (dirChangeTimer <= 0) {
            changeDirection();
            dirChangeTimer = AI_DIRECTION_CHANGE + random.nextDouble() * 1.0;
        }

        // 移动
        tank.move((int) direction.x, (int) direction.y, game.getWalls());

        // 射击
        shootTimer -= dt;
        if (shootTimer <= 0) {
            if (random.nextDouble() < AI_SHOOT_CHANCE) {
                shootAtNearestPlayer();
            }
            shootTimer = 0.8 + random.nextDouble() * 0.5;
        }
    }

    private void changeDirection() {
        Utils.Vec2[] dirs = {
                new Utils.Vec2(0, -1),
                new Utils.Vec2(0, 1),
                new Utils.Vec2(-1, 0),
                new Utils.Vec2(1, 0)
        };
        direction = dirs[random.nextInt(dirs.length)];
        tank.dir = direction;
    }

    private boolean canSeePlayer(Tank player) {
        if (player == null || !player.alive) return false;
        Utils.Vec2 tPos = tank.getCenter();
        Utils.Vec2 pPos = player.getCenter();
        double dist = Math.sqrt(Math.pow(pPos.x - tPos.x, 2) + Math.pow(pPos.y - tPos.y, 2));
        return dist < 300;
    }

    private Tank getNearestPlayer() {
        Tank nearest = null;
        double minDist = Double.MAX_VALUE;

        Tank p1 = game.getPlayer1();
        Tank p2 = game.getPlayer2();

        if (p1 != null && p1.alive) {
            Utils.Vec2 tPos = tank.getCenter();
            Utils.Vec2 pPos = p1.getCenter();
            double dist = Math.sqrt(Math.pow(pPos.x - tPos.x, 2) + Math.pow(pPos.y - tPos.y, 2));
            if (dist < minDist) {
                minDist = dist;
                nearest = p1;
            }
        }

        if (p2 != null && p2.alive) {
            Utils.Vec2 tPos = tank.getCenter();
            Utils.Vec2 pPos = p2.getCenter();
            double dist = Math.sqrt(Math.pow(pPos.x - tPos.x, 2) + Math.pow(pPos.y - tPos.y, 2));
            if (dist < minDist) {
                minDist = dist;
                nearest = p2;
            }
        }

        return nearest;
    }

    private void shootAtNearestPlayer() {
        Tank target = getNearestPlayer();
        if (target != null && target.alive) {
            Utils.Vec2 tPos = tank.getCenter();
            Utils.Vec2 pPos = target.getCenter();
            double dx = pPos.x - tPos.x;
            double dy = pPos.y - tPos.y;
            double len = Math.sqrt(dx * dx + dy * dy);
            if (len > 0) {
                tank.dir = new Utils.Vec2(dx / len, dy / len);
            }
            tank.shoot();
        }
    }
}