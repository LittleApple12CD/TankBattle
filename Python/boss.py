# ========================================
# 坦克大战 - Boss
# ========================================

import pygame
from entities import Tank
from config import COLORS, TANK_SIZE, TANK_SPEED
from config import MAX_BULLETS, BULLET_SIZE, SHOT_COOLDOWN, BULLET_SPEED, COLORS
from entities import Bullet


class Boss(Tank):
    """Boss 坦克"""

    def __init__(self, x, y, hp, size_mult, speed_mult, bullet_damage=2, bullet_speed_mult=1.0):
        # 调用父类构造函数
        super().__init__(x, y, (200, 50, 200), TANK_SPEED * speed_mult, is_player=False, player_id=0)

        # Boss 特殊属性
        self.max_hp = hp
        self.lives = hp  # 用 lives 作为当前血量
        self.size_mult = size_mult
        self.speed_mult = speed_mult
        self.bullet_damage = bullet_damage
        self.bullet_speed_mult = bullet_speed_mult

        # 视觉属性
        self.color = (200, 50, 200)  # 紫色
        self.w = int(TANK_SIZE * size_mult)
        self.h = int(TANK_SIZE * size_mult)
        self.original_speed = TANK_SPEED * speed_mult
        self.speed = self.original_speed

        # Boss 特殊标记
        self.x = x
        self.y = y
        self.is_boss = True

    def draw(self, screen):
        if not self.alive:
            return

        # 直接使用 self.x 和 self.y
        rect = pygame.Rect(self.x, self.y, self.w, self.h)

        # Boss 主体
        color1 = (180, 50, 200)
        pygame.draw.rect(screen, color1, rect, border_radius=6)
        pygame.draw.rect(screen, (255, 215, 0), rect, 3, border_radius=6)

        # Boss 标志 - 星星
        cx, cy = self.get_center()
        font = pygame.font.Font(None, 24)
        star = font.render("⭐", True, (255, 215, 0))
        screen.blit(star, (cx - 12, cy - 12))

        # 炮塔（金色）
        pygame.draw.circle(screen, (255, 215, 0), (cx, cy), self.w // 5)

        # 炮管
        end_x = cx + self.dir[0] * (self.w // 2 + 2)
        end_y = cy + self.dir[1] * (self.h // 2 + 2)
        pygame.draw.line(screen, (255, 215, 0), (cx, cy), (end_x, end_y), 6)

        # ===== 血条（在头顶，居中） =====
        bar_width = self.w + 10
        bar_height = 6
        # 血条居中：x = Boss中心 - 血条宽度的一半
        bar_x = cx - bar_width // 2
        bar_y = self.y - 12
        hp_ratio = self.lives / self.max_hp

        # 背景
        pygame.draw.rect(screen, (60, 60, 60), (bar_x, bar_y, bar_width, bar_height))
        # 血量（颜色随血量变化）
        if hp_ratio > 0.5:
            color = (0, 200, 0)
        elif hp_ratio > 0.25:
            color = (200, 200, 0)
        else:
            color = (200, 50, 50)
        pygame.draw.rect(screen, color, (bar_x, bar_y, int(bar_width * hp_ratio), bar_height))
    
    def get_fire_point(self):
        """Boss 的炮口位置"""
        cx = self.x + self.w // 2
        cy = self.y + self.h // 2
        offset = self.w // 2 + 2
        return (cx + self.dir[0] * offset, cy + self.dir[1] * offset)

    def shoot(self):
        """Boss 射击 - 特殊炮弹"""
        if self.cooldown > 0:
            return None
        if len(self.bullets) >= MAX_BULLETS:
            return None

        self.cooldown = SHOT_COOLDOWN * 0.7  # Boss 射击更快
        fx, fy = self.get_fire_point()

        # Boss 炮弹：力量 + 速度
        bullet_size = int(BULLET_SIZE * 1.5)
        bullet_speed = BULLET_SPEED * self.bullet_speed_mult
        bullet_damage = self.bullet_damage

        bullet = Bullet(fx, fy, self.dir, is_player=False,
                       color=COLORS['bullet_enemy'],
                       speed=bullet_speed,
                       size=bullet_size,
                       damage=bullet_damage)
        self.bullets.append(bullet)
        return bullet