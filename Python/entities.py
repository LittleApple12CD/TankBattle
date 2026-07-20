# ========================================
# 坦克大战 - 实体类
# ========================================

import pygame
import math
from config import *


def clamp(v, min_v, max_v):
    return max(min_v, min(max_v, v))


# ---- 坦克 ----
class Tank:
    def __init__(self, x, y, color, speed=TANK_SPEED, is_player=True, player_id=1):
        self.x = x
        self.y = y
        self.w = TANK_SIZE
        self.h = TANK_SIZE
        self.color = color
        self.speed = speed
        self.is_player = is_player
        self.player_id = player_id  # 1 或 2
        self.dir = (0, -1)
        self.lives = PLAYER_LIVES if is_player else 1
        self.cooldown = 0
        self.alive = True
        self.bullets = []
        self.move_buffer = (0, 0)
        self.effects = {}

        # ===== 行驶痕迹 =====
        self.trail_points = []
        self._frame_counter = 0
        self._last_pos = (x, y)  # 用于判断是否在移动

    @property
    def rect(self):
        return pygame.Rect(self.x, self.y, self.w, self.h)

    def get_center(self):
        return (self.x + self.w // 2, self.y + self.h // 2)

    def get_fire_point(self):
        cx, cy = self.get_center()
        offset = self.w // 2 + 2
        return (cx + self.dir[0] * offset, cy + self.dir[1] * offset)

    def move(self, dx, dy, walls, dt):
        if dx == 0 and dy == 0:
            return

        moved = False
        speed_multiplier = 1.5 if 'speed' in self.effects else 1.0
        current_speed = self.speed * speed_multiplier
        speed_per_frame = current_speed * dt
        total_steps = max(1, int(speed_per_frame / MOVE_STEP))
        step_dx = dx * MOVE_STEP
        step_dy = dy * MOVE_STEP

        for _ in range(total_steps):
            new_x = self.x + step_dx
            new_y = self.y + step_dy

            margin = 2
            max_x = WINDOW_WIDTH - self.w - margin
            max_y = WINDOW_HEIGHT - self.h - margin
            new_x = clamp(new_x, margin, max_x)
            new_y = clamp(new_y, margin, max_y)

            test_rect = pygame.Rect(new_x, new_y, self.w, self.h)
            blocked = False
            for wall in walls:
                if wall.alive and test_rect.colliderect(wall.rect):
                    blocked = True
                    break

            if not blocked:
                self.x = new_x
                self.y = new_y
                moved = True
            else:
                if dx != 0:
                    test_rect = pygame.Rect(new_x, self.y, self.w, self.h)
                    blocked = False
                    for wall in walls:
                        if wall.alive and test_rect.colliderect(wall.rect):
                            blocked = True
                            break
                    if not blocked:
                        self.x = new_x
                        moved = True   # ← 加这行

                if dy != 0:
                    test_rect = pygame.Rect(self.x, new_y, self.w, self.h)
                    blocked = False
                    for wall in walls:
                        if wall.alive and test_rect.colliderect(wall.rect):
                            blocked = True
                            break
                    if not blocked:
                        self.y = new_y
                        moved = True   # ← 加这行
                break

        if moved:
            self._frame_counter += 1
            if self._frame_counter % 3 == 0:
                self.trail_points.append({
                    'x': self.x + self.w / 2,
                    'y': self.y + self.h / 2,
                    'age': 0
                })
        else:
            self._frame_counter = 0

    def shoot(self):
        if self.cooldown > 0:
            return None
        if len(self.bullets) >= MAX_BULLETS:
            return None

        self.cooldown = SHOT_COOLDOWN
        fx, fy = self.get_fire_point()

        # 默认伤害 = 1，Strength 时 = 2
        bullet_damage = 2 if 'strength' in self.effects else 1
        bullet_speed = BULLET_SPEED * (1.5 if 'speed' in self.effects else 1.0)
        bullet_size = int(BULLET_SIZE * (1.5 if 'strength' in self.effects else 1.0))
    
        if self.is_player:
            if self.player_id == 1:
                bullet = Bullet(fx, fy, self.dir, is_player=True, player_id=1, 
                               color=COLORS['bullet_p1'], size=bullet_size, damage=bullet_damage, speed=bullet_speed)
            else:
                bullet = Bullet(fx, fy, self.dir, is_player=True, player_id=2,
                               color=COLORS['bullet_p2'], size=bullet_size, damage=bullet_damage, speed=bullet_speed)
        else:
            bullet = Bullet(fx, fy, self.dir, is_player=False,
                           color=COLORS['bullet_enemy'], size=bullet_size, damage=bullet_damage, speed=bullet_speed)
    
        self.bullets.append(bullet)
        return bullet

    def update(self, dt):
        if self.cooldown > 0:
            self.cooldown -= dt

        for effect in list(self.effects.keys()):
            self.effects[effect] -= dt
            if self.effects[effect] <= 0:
                del self.effects[effect]

        # 更新痕迹（移除超过1秒的）
        for t in self.trail_points[:]:
            t['age'] += dt
            if t['age'] >= 1.0:
                self.trail_points.remove(t)

        for b in self.bullets[:]:
            b.update(dt)
            if not b.alive:
                self.bullets.remove(b)

    def draw(self, screen):
        # ===== 先画行驶痕迹 =====
        for t in self.trail_points:
            alpha = 60 * (1 - t['age'] / 1.0)
            if alpha > 5:
                # 使用半透明黑色矩形
                surf = pygame.Surface((self.w, self.h), pygame.SRCALPHA)
                surf.fill((0, 0, 0, int(alpha)))
                screen.blit(surf, (t['x'] - self.w/2, t['y'] - self.h/2))

        if not self.alive:
            return
        
        # 计算缩放（Strength）
        scale = 1.5 if 'strength' in self.effects else 1.0
        w_draw = int(self.w * scale)
        h_draw = int(self.h * scale)
        x_draw = self.x - (w_draw - self.w) // 2
        y_draw = self.y - (h_draw - self.h) // 2
        rect = pygame.Rect(x_draw, y_draw, w_draw, h_draw)

        # 主体
        pygame.draw.rect(screen, self.color, rect, border_radius=4)
        # 普通边框
        pygame.draw.rect(screen, (255, 255, 255), rect, 1, border_radius=4)

        # Protection：白色边框（加粗）
        if 'protection' in self.effects:
            pygame.draw.rect(screen, (255, 255, 255), rect, 3, border_radius=4)

        # 炮塔、炮管不变（使用原本的 get_center 位置）
        cx, cy = self.get_center()
        pygame.draw.circle(screen, (255, 255, 255), (cx, cy), self.w // 5)
        end_x = cx + self.dir[0] * (self.w // 2 + 2)
        end_y = cy + self.dir[1] * (self.w // 2 + 2)
        pygame.draw.line(screen, (255, 255, 255), (cx, cy), (end_x, end_y), 4)

        # 玩家编号
        if self.is_player:
            font = pygame.font.Font(None, 16)
            label = font.render(str(self.player_id), True, (0, 0, 0))
            screen.blit(label, (self.x + self.w//2 - 5, self.y + self.h//2 - 8))


# ---- 子弹 ----
class Bullet:
    def __init__(self, x, y, direction, is_player=True, player_id=1, color=None,
                 speed=BULLET_SPEED, size=None, damage=1):
        if size is None:
            size = BULLET_SIZE
        self.x = x - size // 2
        self.y = y - size // 2
        self.w = size
        self.h = size
        self.dir = direction
        self.speed = speed
        self.is_player = is_player
        self.player_id = player_id
        self.color = color if color else (255, 255, 200)
        self.alive = True
        self.damage = damage
        self.trail = []

    @property
    def rect(self):
        return pygame.Rect(self.x, self.y, self.w, self.h)

    def update(self, dt):
        # 添加拖尾粒子
        self.trail.append({
            'x': self.x + self.w / 2,
            'y': self.y + self.h / 2,
            'size': self.w * 0.6,
            'age': 0,
            'lifetime': 0.15
        })

        # 更新位置
        self.x += self.dir[0] * self.speed * dt
        self.y += self.dir[1] * self.speed * dt

        # 更新拖尾
        for t in self.trail[:]:
            t['age'] += dt
            t['size'] *= (1 - dt / t['lifetime'])
            if t['age'] >= t['lifetime'] or t['size'] < 0.5:
                self.trail.remove(t)

        if self.x < -20 or self.x > WINDOW_WIDTH + 20 or \
           self.y < -20 or self.y > WINDOW_HEIGHT + 20:
            self.alive = False

    def draw(self, screen):
        # 画拖尾
        for t in self.trail:
            alpha = 255 * (1 - t['age'] / t['lifetime'])
            size = int(t['size'])
            if size > 0 and alpha > 5:
                surf = pygame.Surface((size * 2, size * 2), pygame.SRCALPHA)
                col = self.color
                pygame.draw.circle(surf, (col[0], col[1], col[2], int(alpha)), (size, size), size)
                screen.blit(surf, (int(t['x'] - size), int(t['y'] - size)))

        # 画子弹
        if not self.alive:
            return
        cx, cy = self.x + self.w // 2, self.y + self.h // 2
        pygame.draw.circle(screen, self.color, (cx, cy), self.w // 2)
        pygame.draw.circle(screen, (255, 255, 255), (cx, cy), self.w // 4)

# ---- 墙壁 ----
class Wall:
    def __init__(self, x, y, w, h, is_steel=False):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.is_steel = is_steel
        self.alive = True

    @property
    def rect(self):
        return pygame.Rect(self.x, self.y, self.w, self.h)

    def draw(self, screen):
        if not self.alive:
            return
        color = COLORS['steel'] if self.is_steel else COLORS['wall']
        pygame.draw.rect(screen, color, self.rect, border_radius=2)
        pygame.draw.rect(screen, (255, 255, 255), self.rect, 1, border_radius=2)


# ---- 爆炸 ----
class Explosion:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.radius = 5
        self.max_radius = 30
        self.age = 0
        self.lifetime = 0.5
        self.alive = True

    def update(self, dt):
        self.age += dt
        progress = self.age / self.lifetime
        self.radius = self.max_radius * progress
        if self.age >= self.lifetime:
            self.alive = False

    def draw(self, screen):
        if not self.alive:
            return
        progress = self.age / self.lifetime
        r = int(self.radius)
        colors = [
            (255, 255, 200),
            (255, 200, 100),
            (255, 150, 50),
            (255, 100, 20),
            (200, 50, 0),
        ]
        idx = min(int(progress * len(colors)), len(colors) - 1)
        color = colors[idx]
        pygame.draw.circle(screen, color, (int(self.x), int(self.y)), r)

# ---- 道具 ----
class PowerUp:
    def __init__(self, x, y, ptype):
        self.x = x
        self.y = y
        self.w = 24
        self.h = 24
        self.ptype = ptype  # 'S', 'P', 'H', 'T'
        self.alive = True
        
        self.color_map = {
            'S': (50, 150, 255),   # 蓝色 - Speed
            'P': (255, 255, 255),  # 白色 - Protection
            'H': (255, 50, 50),    # 红色 - Health
            'T': (255, 150, 50)    # 橙色 - Strength
        }

    def get_rect(self):
        return pygame.Rect(self.x, self.y, self.w, self.h)

    def draw(self, screen):
        if not self.alive:
            return
        
        color = self.color_map.get(self.ptype, (200, 200, 200))
        pygame.draw.rect(screen, color, (self.x, self.y, self.w, self.h))
        pygame.draw.rect(screen, (255, 255, 255), (self.x, self.y, self.w, self.h), 1)
    
        font = pygame.font.Font(None, 20)
        text = font.render(self.ptype, True, (0, 0, 0))
        text_rect = text.get_rect(center=(self.x + self.w/2, self.y + self.h/2))
        screen.blit(text, text_rect)