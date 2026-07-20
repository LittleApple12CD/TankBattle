# ========================================
# 坦克大战 - 游戏逻辑（PVP/PVE双模式）
# ========================================

import pygame
import random
import math
from config import *
from entities import Tank, Wall, Explosion, PowerUp
from ai import EnemyAI


class Game:
    def __init__(self):
        self.walls = []
        self.explosions = []
        self.player1 = None
        self.player2 = None
        self.enemies = []
        self.enemy_ais = []
        self.enemy_spawn_timer = 0
        self.score = 0
        self.game_over = False
        self.paused = False
        self.current_map = 0
        self.pvp_mode = False          # False=PVE合作, True=PVP对战
        self.single_mode = False  # False=双人, True=单人
        self.enemy_count = ENEMY_COUNT  # 单人时敌人翻倍
        self.menu = None
        self.state = "playing"
        self.return_to_menu = False  # 游戏内按 ESC 回到菜单
        self.powerups = []
        self.powerup_timer = 0.0
        self.powerup_interval = 10.0
        self.max_powerups = 3
        
        self._init_fonts()
        self._init_level()

    def init_menu(self):
        from menu import Menu
        self.menu = Menu(self.screen)
        self.state = "menu"

    def start_game(self, mode, game_type):
        """mode: 'single_player', 'pvp', 'pve'"""
        # 清理旧游戏状态
        self._cleanup()
        # 根据模式设置
        if mode == "single_player":
            self.single_mode = True
            self.pvp_mode = False
            self.enemy_count = ENEMY_COUNT * 2
        elif mode == "pvp":
            self.single_mode = False
            self.pvp_mode = True
            self.enemy_count = 0  # PVP 没有敌人
        elif mode == "pve":
            self.single_mode = False
            self.pvp_mode = False
            self.enemy_count = ENEMY_COUNT
        self.state = "playing"
        self.initLevel()
    
    def toggle_single_mode(self):
        """切换单双人模式"""
        self.single_mode = not self.single_mode
        # 单人模式强制禁用 PVP
        if self.single_mode:
            self.pvp_mode = False
            self.enemy_count = ENEMY_COUNT
        else:
            self.enemy_count = ENEMY_COUNT
        self._init_level()

    def _init_fonts(self):
        font_names = [
            "SimHei", "Microsoft YaHei", "PingFang SC",
            "Arial", "Consolas", "DejaVu Sans",
            "Noto Sans CJK SC", "WenQuanYi Micro Hei"
        ]
        
        self.font_small = None
        self.font_normal = None
        self.font_big = None
        
        for name in font_names:
            try:
                self.font_small = pygame.font.SysFont(name, 14)
                self.font_normal = pygame.font.SysFont(name, 18)
                self.font_big = pygame.font.SysFont(name, 36)
                break
            except:
                continue
        
        if self.font_small is None:
            self.font_small = pygame.font.Font(None, 14)
            self.font_normal = pygame.font.Font(None, 18)
            self.font_big = pygame.font.Font(None, 36)

    def toggle_pvp_mode(self):
        """切换PVP/PVE模式"""
        self.pvp_mode = not self.pvp_mode
        self._init_level()  # 重置关卡

    def _init_level(self):
        # 根据当前模式设置敌人数量
        if self.pvp_mode:
            self.enemy_count = 0
        else:
            self.enemy_count = ENEMY_COUNT
    
        self.game_over = False
        self.paused = False
        self.score = 0
        self.enemy_spawn_timer = 0
        self.walls.clear()
        self.explosions.clear()
        self.enemies.clear()
        self.enemy_ais.clear()
        self.powerups.clear()
        self.powerup_timer = 0.0

        self.current_map = random.randint(0, 4)
        self._build_map(self.current_map)  # ← 这里不递归，调用 _build_map

        # P1 出生
        p1_x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
        p1_y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
        if self.player1 is None:
            self.player1 = Tank(p1_x, p1_y, COLORS['player1'], is_player=True, player_id=1)
        else:
            self.player1.x = p1_x
            self.player1.y = p1_y
            self.player1.alive = True
            self.player1.lives = PLAYER_LIVES
            self.player1.effects.clear()

        # P2 仅在双人模式存在
        if not self.single_mode:
            p2_x = GRID_OFFSET_X + (GRID_SIZE - 2) * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            p2_y = GRID_OFFSET_Y + CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            if self.player2 is None:
                self.player2 = Tank(p2_x, p2_y, COLORS['player2'], is_player=True, player_id=2)
            else:
                self.player2.x = p2_x
                self.player2.y = p2_y
                self.player2.alive = True
                self.player2.lives = PLAYER_LIVES
                self.player2.effects.clear()
        else:
            self.player2 = None

        # 生成敌人
        if not self.pvp_mode:
            for i in range(self.enemy_count):
                self._spawn_enemy()

    def _build_map(self, map_id):
        self._build_border()
        map_generators = {
            0: self._map_empty,
            1: self._map_cross,
            2: self._map_maze,
            3: self._map_bunker,
            4: self._map_symmetric,
        }
        map_generators.get(map_id, self._map_empty)()

    def _build_border(self):
        margin = CELL_SIZE // 2
        wall_thick = margin
        
        for i in range(GRID_SIZE):
            x = GRID_OFFSET_X + i * CELL_SIZE
            y = GRID_OFFSET_Y + i * CELL_SIZE
            self.walls.append(Wall(x, GRID_OFFSET_Y, CELL_SIZE, wall_thick))
            self.walls.append(Wall(x, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE - wall_thick, CELL_SIZE, wall_thick))
            self.walls.append(Wall(GRID_OFFSET_X, y, wall_thick, CELL_SIZE))
            self.walls.append(Wall(GRID_OFFSET_X + GRID_SIZE * CELL_SIZE - wall_thick, y, wall_thick, CELL_SIZE))

    def _add_wall(self, col, row, w, h, is_steel=False):
        x = GRID_OFFSET_X + col * CELL_SIZE
        y = GRID_OFFSET_Y + row * CELL_SIZE
        wall = Wall(x, y, w * CELL_SIZE, h * CELL_SIZE, is_steel)
        
        for existing in self.walls:
            if existing.alive and wall.rect.colliderect(existing.rect):
                return False
        
        self.walls.append(wall)
        return True

    def _add_brick(self, col, row, w=1, h=1):
        return self._add_wall(col, row, w, h, is_steel=False)

    def _add_steel(self, col, row, w=1, h=1):
        return self._add_wall(col, row, w, h, is_steel=True)

    # ===== 地图布局 =====
    def _map_empty(self):
        pass

    def _map_cross(self):
        for col in range(2, 11):
            self._add_brick(col, 6, 1, 1)
        for row in range(2, 11):
            self._add_brick(6, row, 1, 1)
        self._add_steel(6, 6, 1, 1)
        self._add_brick(3, 3, 1, 1)
        self._add_brick(9, 3, 1, 1)
        self._add_brick(3, 9, 1, 1)
        self._add_brick(9, 9, 1, 1)

    def _map_maze(self):
        maze_walls = [
            (2, 2, 1, 3), (8, 2, 1, 3),
            (3, 4, 3, 1), (7, 4, 3, 1),
            (2, 5, 1, 3), (8, 5, 1, 3),
            (4, 7, 1, 2), (6, 7, 1, 2),
            (3, 8, 3, 1), (7, 8, 3, 1),
            (2, 9, 1, 2), (8, 9, 1, 2),
        ]
        for col, row, w, h in maze_walls:
            self._add_brick(col, row, w, h)
        self._add_steel(5, 5, 1, 1)
        self._add_steel(7, 5, 1, 1)
        self._add_steel(5, 7, 1, 1)
        self._add_steel(7, 7, 1, 1)

    def _map_bunker(self):
        bunkers = [
            (2, 2, 2, 1), (10, 2, 2, 1),
            (2, 4, 1, 2), (11, 4, 1, 2),
            (2, 8, 2, 1), (10, 8, 2, 1),
            (2, 10, 1, 2), (11, 10, 1, 2),
            (4, 3, 1, 1), (9, 3, 1, 1),
            (4, 9, 1, 1), (9, 9, 1, 1),
        ]
        for col, row, w, h in bunkers:
            self._add_brick(col, row, w, h)
        self._add_steel(6, 11, 1, 1)
        self._add_steel(7, 11, 1, 1)
        self._add_steel(5, 11, 1, 1)
        self._add_steel(8, 11, 1, 1)

    def _map_symmetric(self):
        for i in range(2, 5):
            self._add_brick(i, 3, 1, 1)
            self._add_brick(12 - i, 3, 1, 1)
            self._add_brick(i, 9, 1, 1)
            self._add_brick(12 - i, 9, 1, 1)
        for i in range(4, 7):
            self._add_brick(3, i, 1, 1)
            self._add_brick(10, i, 1, 1)
            self._add_brick(3, 12 - i, 1, 1)
            self._add_brick(10, 12 - i, 1, 1)
        self._add_steel(6, 5, 1, 1)
        self._add_steel(7, 5, 1, 1)
        self._add_steel(6, 7, 1, 1)
        self._add_steel(7, 7, 1, 1)
        self._add_brick(5, 6, 1, 1)
        self._add_brick(8, 6, 1, 1)

    def _spawn_enemy(self):
        if len(self.enemies) >= ENEMY_COUNT:
            return

        spawn_positions = [
            (GRID_SIZE - 2, 1),
            (GRID_SIZE // 2, 1),
            (1, 1),
        ]
        col, row = random.choice(spawn_positions)
        x = GRID_OFFSET_X + col * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
        y = GRID_OFFSET_Y + row * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2

        test_rect = pygame.Rect(x, y, TANK_SIZE, TANK_SIZE)
        for enemy in self.enemies:
            if test_rect.colliderect(enemy.rect):
                return
        for player in [self.player1, self.player2]:
            if player and player.alive and test_rect.colliderect(player.rect):
                return

        tank = Tank(x, y, COLORS['enemy'], TANK_SPEED * 0.7, is_player=False)
        self.enemies.append(tank)
        self.enemy_ais.append(EnemyAI(tank, self))

    def update(self, dt):
        if self.state != "playing":
            return

        if self.game_over or self.paused:
            return

        if self.player1 and self.player1.alive:
            self.player1.update(dt)

        # 单人模式：P2 不存在，跳过
        if not self.single_mode and self.player2 and self.player2.alive:
            self.player2.update(dt)

        # 道具更新
        self._update_powerups(dt)

        # PVE模式：更新敌人
        if not self.pvp_mode:
            for ai in self.enemy_ais[:]:
                ai.update(dt)

            # 清除死亡敌人
            for i in range(len(self.enemies) - 1, -1, -1):
                if not self.enemies[i].alive:
                    self.enemies.pop(i)
                    self.enemy_ais.pop(i)

            # 敌人生成
            self.enemy_spawn_timer -= dt
            if self.enemy_spawn_timer <= 0 and len(self.enemies) < ENEMY_COUNT:
                self._spawn_enemy()
                self.enemy_spawn_timer = ENEMY_SPAWN_INTERVAL

        # 子弹碰撞
        self._handle_bullet_collisions()

        # 爆炸效果
        for e in self.explosions[:]:
            e.update(dt)
            if not e.alive:
                self.explosions.remove(e)

        # 游戏结束检查
        if self.pvp_mode:
            # PVP模式：任一玩家死亡则结束
            if (self.player1 and not self.player1.alive) or (self.player2 and not self.player2.alive):
                self.game_over = True
        else:
            # PVE模式：两个玩家都死亡才结束
            if self.player1 and not self.player1.alive and self.player2 and not self.player2.alive:
                self.game_over = True

        if self.single_mode:
            # 单人模式：P1 死亡则游戏结束
            if self.player1 and not self.player1.alive:
                self.game_over = True
        else:
            # 双人模式：两个玩家都死亡才结束
            if self.player1 and not self.player1.alive and self.player2 and not self.player2.alive:
                self.game_over = True

    def _handle_bullet_collisions(self):
        all_bullets = []
        if self.player1:
            all_bullets.extend(self.player1.bullets)
        if self.player2:
            all_bullets.extend(self.player2.bullets)
        if not self.pvp_mode:
            for enemy in self.enemies:
                all_bullets.extend(enemy.bullets)

        for bullet in all_bullets[:]:
            if not bullet.alive:
                continue

            # 子弹 vs 墙壁
            bullet_hit = False
            for wall in self.walls:
                if not wall.alive:
                    continue
                if bullet.rect.colliderect(wall.rect):
                    # Strength 子弹爆炸范围
                    if bullet.damage == 2 and not bullet.is_player:
                        # 敌人子弹触发爆炸，但正常逻辑只有玩家子弹才需要爆炸？实际上 Strength 对所有坦克生效，包括敌人，但敌人子弹有 Strength 也会爆炸。
                        # 我们统一：子弹 damage == 2 触发范围爆炸
                        self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                    else:
                        bullet.alive = False
                        bullet_hit = True
                        if not wall.is_steel:
                            wall.alive = False
                        self._add_explosion(bullet.x + bullet.w//2, bullet.y + bullet.h//2)
                    break

            if bullet_hit or not bullet.alive:
                continue

            # ---- PVP模式：玩家子弹互伤 ----
            if self.pvp_mode:
                # P1子弹 vs P2
                if bullet.is_player and bullet.player_id == 1:
                    if self.player2 and self.player2.alive and bullet.rect.colliderect(self.player2.rect):
                        if bullet.damage == 2:
                            self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                        else:
                            bullet.alive = False
                            if 'protection' in self.player2.effects:
                                del self.player2.effects['protection']
                            else:
                                self.player2.lives -= 1
                                self._add_explosion(self.player2.x + self.player2.w//2, self.player2.y + self.player2.h//2)
                            if self.player2.lives <= 0:
                                self.player2.alive = False
                        continue

                # P2子弹 vs P1
                if bullet.is_player and bullet.player_id == 2:
                    if self.player1 and self.player1.alive and bullet.rect.colliderect(self.player1.rect):
                        if bullet.damage == 2:
                            self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                        else:
                            bullet.alive = False
                            if 'protection' in self.player1.effects:
                                del self.player1.effects['protection']
                            else:
                                self.player1.lives -= 1
                                self._add_explosion(self.player1.x + self.player1.w//2, self.player1.y + self.player1.h//2)
                            if self.player1.lives <= 0:
                                self.player1.alive = False
                        continue

            # ---- PVE模式 ----
            else:
                if bullet.is_player:
                    # 玩家子弹 vs 敌人
                    for enemy in self.enemies[:]:
                        if enemy.alive and bullet.rect.colliderect(enemy.rect):
                            if bullet.damage == 2:
                                self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                            else:
                                bullet.alive = False
                                if 'protection' in enemy.effects:
                                    del enemy.effects['protection']
                                else:
                                    enemy.lives -= 1
                                if enemy.lives <= 0:
                                    enemy.alive = False
                                    self.score += 10
                                    self._add_explosion(enemy.x + enemy.w//2, enemy.y + enemy.h//2)
                            break
                else:
                    # 敌人子弹 vs 玩家
                    for player in [self.player1, self.player2]:
                        if player and player.alive and bullet.rect.colliderect(player.rect):
                            if bullet.damage == 2:
                                self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                            else:
                                bullet.alive = False
                                if 'protection' in player.effects:
                                    del player.effects['protection']
                                else:
                                    player.lives -= 1
                                    self._add_explosion(player.x + player.w//2, player.y + player.h//2)
                                if player.lives <= 0:
                                    player.alive = False
                            break

        # 清理子弹
        if self.player1:
            self.player1.bullets = [b for b in self.player1.bullets if b.alive]
        if self.player2:
            self.player2.bullets = [b for b in self.player2.bullets if b.alive]
        if not self.pvp_mode:
            for enemy in self.enemies:
                enemy.bullets = [b for b in enemy.bullets if b.alive]

    def _add_explosion(self, x, y):
        for _ in range(4):
            self.explosions.append(Explosion(
                x + random.randint(-15, 15),
                y + random.randint(-15, 15)
            ))

    def _update_powerups(self, dt):
        self.powerup_timer += dt
        if self.powerup_timer >= self.powerup_interval and len(self.powerups) < self.max_powerups:
            self._spawn_powerup()
            self.powerup_timer = 0

        for p in self.powerups[:]:
            if not p.alive:
                self.powerups.remove(p)
                continue

            # 检查所有坦克
            all_tanks = []
            if self.player1 and self.player1.alive:
                all_tanks.append(self.player1)
            if not self.single_mode and self.player2 and self.player2.alive:
                all_tanks.append(self.player2)
            for enemy in self.enemies:
                if enemy.alive:
                    all_tanks.append(enemy)

            for tank in all_tanks:
                if p.get_rect().colliderect(tank.rect):
                    self._apply_powerup(tank, p)
                    p.alive = False
                    self.powerups.remove(p)
                    break

    def _spawn_powerup(self):
        # 随机位置（游戏区域内，避开墙壁）
        margin = 40
        for _ in range(20):  # 最多尝试20次
            x = random.randint(GRID_OFFSET_X + margin, GRID_OFFSET_X + GRID_SIZE * CELL_SIZE - margin - 24)
            y = random.randint(GRID_OFFSET_Y + margin, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE - margin - 24)
            test_rect = pygame.Rect(x, y, 24, 24)
            blocked = False
            for wall in self.walls:
                if wall.alive and test_rect.colliderect(wall.rect):
                    blocked = True
                    break
            if not blocked:
                ptype = random.choice(['S', 'P', 'H', 'T'])
                self.powerups.append(PowerUp(x, y, ptype))
                return

    def _apply_powerup(self, tank, powerup):
        ptype = powerup.ptype
    
        if ptype == 'H':
            tank.lives += 1
            tank.lives = min(tank.lives, 5)
        
        elif ptype == 'S':
            tank.effects['speed'] = 5.0
            # 显示效果文字（可选）
            
        elif ptype == 'P':
            tank.effects['protection'] = 5.0
            
        elif ptype == 'T':
            tank.effects['strength'] = 5.0
    
    def move_player1(self, dx, dy, dt):
        if self.player1 and self.player1.alive:
            self.player1.dir = (dx, dy)
            # 应用 Speed 效果
            speed = self.player1.speed
            if 'speed' in self.player1.effects:
                speed = self.player1.speed * 1.5
            self.player1.move(dx, dy, self.walls, dt)

    def move_player2(self, dx, dy, dt):
        if self.player2 and self.player2.alive:
            self.player2.dir = (dx, dy)
            # 应用 Speed 效果
            speed = self.player2.speed
            if 'speed' in self.player2.effects:
                speed = self.player2.speed * 1.5
            self.player2.move(dx, dy, self.walls, dt)

    def player1_shoot(self):
        if self.player1 and self.player1.alive:
            self.player1.shoot()

    def player2_shoot(self):
        if self.player2 and self.player2.alive:
            self.player2.shoot()

    def draw(self, screen):
        screen.fill(COLORS['bg'])

        # 网格
        for i in range(GRID_SIZE + 1):
            x = GRID_OFFSET_X + i * CELL_SIZE
            y = GRID_OFFSET_Y + i * CELL_SIZE
            pygame.draw.line(screen, COLORS['grid'], (x, GRID_OFFSET_Y), (x, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE), 1)
            pygame.draw.line(screen, COLORS['grid'], (GRID_OFFSET_X, y), (GRID_OFFSET_X + GRID_SIZE * CELL_SIZE, y), 1)

        pygame.draw.rect(screen, (60, 60, 80), 
                        (GRID_OFFSET_X, GRID_OFFSET_Y, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), 2)

        for wall in self.walls:
            wall.draw(screen)

        if not self.pvp_mode:
            for enemy in self.enemies:
                enemy.draw(screen)

        if self.player1:
            self.player1.draw(screen)
        if self.player2:
            self.player2.draw(screen)

        if not self.pvp_mode:
            for enemy in self.enemies:
                for b in enemy.bullets:
                    b.draw(screen)
        if self.player1:
            for b in self.player1.bullets:
                b.draw(screen)
        if self.player2:
            for b in self.player2.bullets:
                b.draw(screen)

        for ex in self.explosions:
            ex.draw(screen)
        
        for p in self.powerups:
            p.draw(screen)

        self._draw_ui(screen)

    def handle_key(self, key):
        if self.state != "playing":
            return self.menu.handle_event(pygame.event.Event(pygame.KEYDOWN, key=key))

    def _draw_ui(self, screen):
        font = pygame.font.SysFont("Consolas", 18)
        font_big = pygame.font.SysFont("Consolas", 36)
        font_small = pygame.font.SysFont("Consolas", 14)
        ui_y = 10

        # ===== 在函数开头定义 map_names =====
        map_names = ["Empty", "Cross", "Maze", "Bunker", "Sym"]
        # ====================================

        # Mode
        mode_text = "PVP" if self.pvp_mode else "PVE"
        text = font.render(mode_text, True, COLORS['text'])
        screen.blit(text, (10, ui_y))
        ui_y += 25

        # Score (PVE only)
        if not self.pvp_mode:
            text = font.render(f"Score: {self.score}", True, COLORS['text'])
            screen.blit(text, (10, ui_y))
            ui_y += 25

        # P1 Lives
        p1_text = "P1: "
        for i in range(self.player1.lives if self.player1 else 0):
            p1_text += "[] "
        text = font.render(p1_text, True, COLORS['player1'])
        screen.blit(text, (10, ui_y))
        ui_y += 25

        # P2 Lives
        p2_text = "P2: "
        for i in range(self.player2.lives if self.player2 else 0):
            p2_text += "[] "
        text = font.render(p2_text, True, COLORS['player2'])
        screen.blit(text, (10, ui_y))
        ui_y += 25

        # Enemies (PVE only)
        if not self.pvp_mode:
            text = font.render(f"Enemy: {len(self.enemies)}/{ENEMY_COUNT}", True, COLORS['text'])
            screen.blit(text, (10, ui_y))

        # Map name
        text_small = font_small.render(f"Map: {map_names[self.current_map]}", True, COLORS['text_dim'])
        screen.blit(text_small, (10, WINDOW_HEIGHT - 25))

        # Pause
        if self.paused:
            text = font_big.render("PAUSED", True, (255, 255, 255))
            screen.blit(text, (WINDOW_WIDTH // 2 - 80, WINDOW_HEIGHT // 2 - 30))

        # Game Over
        if self.game_over:
            text = font_big.render("GAME OVER", True, (255, 50, 50))
            screen.blit(text, (WINDOW_WIDTH // 2 - 80, WINDOW_HEIGHT // 2 - 50))
            text2 = font.render("Press R to restart", True, COLORS['text'])
            screen.blit(text2, (WINDOW_WIDTH // 2 - 80, WINDOW_HEIGHT // 2 + 40))

        # Controls
        controls = [
            "P1: Arrows + SPACE",
            "P2: WASD + J",
            "G: PVP/PVE  P: Pause  R: Restart",
            "ESC: Exit"
        ]
        for i, c in enumerate(controls):
            text = font_small.render(c, True, COLORS['text_dim'])
            screen.blit(text, (WINDOW_WIDTH - 200, 10 + i * 20))

    def _explode_at(self, x, y, is_player):
        """Strength 子弹爆炸，范围 TANK_SIZE * 2，伤害 2，摧毁普通墙"""
        radius = TANK_SIZE * 2
        center = (x, y)
    
        # 对所有敌人
        for enemy in self.enemies[:]:
            if not enemy.alive:
                continue
            # 计算距离
            ex, ey = enemy.get_center()
            dist = math.hypot(ex - x, ey - y)
            if dist <= radius:
                if 'protection' in enemy.effects:
                    del enemy.effects['protection']
                else:
                    enemy.lives -= 2
                    if enemy.lives <= 0:
                        enemy.alive = False
                        self.score += 10
                        self._add_explosion(enemy.x + enemy.w//2, enemy.y + enemy.h//2)
    
        # 对玩家（PVP模式下才伤害玩家）
        if self.pvp_mode:
            for player in [self.player1, self.player2]:
                if player and player.alive:
                    px, py = player.get_center()
                    dist = math.hypot(px - x, py - y)
                    if dist <= radius:
                        if 'protection' in player.effects:
                            del player.effects['protection']
                        else:
                            player.lives -= 2
                            self._add_explosion(player.x + player.w//2, player.y + player.h//2)
                            if player.lives <= 0:
                                player.alive = False
        
        # 摧毁普通墙壁
        for wall in self.walls[:]:
            if not wall.alive:
                continue
            if wall.is_steel:
                continue
            # 检测墙壁是否在爆炸范围内
            wx, wy = wall.x + wall.w//2, wall.y + wall.h//2
            if math.hypot(wx - x, wy - y) <= radius:
                wall.alive = False
    
        # 爆炸特效
        for _ in range(8):
            self._add_explosion(x + random.randint(-radius//2, radius//2),
                               y + random.randint(-radius//2, radius//2))
            
    def player1_shoot(self):
        print("shoot called")  # 调试
        if self.player1 and self.player1.alive:
            self.player1.shoot()