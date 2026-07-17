# ========================================
# 坦克大战 - 敌人AI
# ========================================

import random
import math
from config import *


class EnemyAI:
    def __init__(self, tank, game):
        self.tank = tank
        self.game = game
        self.dir_change_timer = 0
        self.shoot_timer = 0
        self.direction = (0, 1)

    def update(self, dt):
        # 更新坦克（包括子弹）
        self.tank.update(dt)

        # 改变方向
        self.dir_change_timer -= dt
        if self.dir_change_timer <= 0:
            self._change_direction()
            self.dir_change_timer = AI_DIRECTION_CHANGE + random.random() * 1.0

        # 移动
        self.tank.move(self.direction[0], self.direction[1], self.game.walls, dt)

        # 射击
        self.shoot_timer -= dt
        if self.shoot_timer <= 0:
            # 检查是否能看见玩家
            if self._can_see_any_player():
                if random.random() < AI_SHOOT_CHANCE:
                    self._shoot_at_nearest_player()
            self.shoot_timer = 0.8 + random.random() * 0.5

    def _change_direction(self):
        dirs = [(0, -1), (0, 1), (-1, 0), (1, 0)]
        self.direction = random.choice(dirs)
        self.tank.dir = self.direction

    def _can_see_any_player(self):
        """检查是否能看见任意玩家"""
        for player in [self.game.player1, self.game.player2]:
            if player and player.alive:
                if self._can_see_player(player):
                    return True
        return False

    def _can_see_player(self, player):
        """检查是否能看见特定玩家"""
        tx, ty = self.tank.get_center()
        px, py = player.get_center()
        return math.sqrt((px-tx)**2 + (py-ty)**2) < 300

    def _shoot_at_nearest_player(self):
        """向最近的玩家射击"""
        nearest = None
        min_dist = float('inf')
        
        for player in [self.game.player1, self.game.player2]:
            if player and player.alive:
                tx, ty = self.tank.get_center()
                px, py = player.get_center()
                dist = math.sqrt((px-tx)**2 + (py-ty)**2)
                if dist < min_dist:
                    min_dist = dist
                    nearest = player
        
        if nearest:
            self._shoot_at_player(nearest)

    def _shoot_at_player(self, player):
        """向指定玩家射击"""
        if not player or not player.alive:
            return
        px, py = player.get_center()
        tx, ty = self.tank.get_center()
        dx = px - tx
        dy = py - ty
        length = math.sqrt(dx*dx + dy*dy)
        if length > 0:
            self.tank.dir = (dx/length, dy/length)
        self.tank.shoot()