# ========================================
# 坦克大战 - 游戏逻辑（PVP/PVE双模式）
# ========================================

import pygame
import random
import math
from config import *
from entities import Tank, Wall, Explosion, PowerUp
from ai import EnemyAI
from level_data import MAX_LEVEL, get_level_config, get_map_id, ENEMY_TYPES, BOSS_TYPES
from save_manager import save_progress, load_progress, has_save
from level_state import LevelState, LevelController
from boss import Boss
from menu import Menu
from sound_manager import SoundManager

class Game:
    def __init__(self, screen=None):
        self.screen = screen
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
        self.pvp_mode = False
        self.single_mode = False
        self.enemy_count = ENEMY_COUNT
        self.menu = None
        self.state = "playing"
        self.return_to_menu = False
        self.powerups = []
        self.powerup_timer = 0.0
        self.powerup_interval = 10.0
        self.max_powerups = 3
        self.game_mode = "endless"
        self.level_controller = None
        self.level = 1
        self.show_message = None
        self.waiting_for_enter = False
        self.sound_manager = SoundManager()
        self.sound_manager.load_sounds()
        self._gameover_played = False
        
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

    def start_level_mode(self, level):
        """关卡模式启动"""
        from level_data import get_map_id
        from level_state import LevelController
        from save_manager import save_progress

        self.game_mode = "level"
        self.level = level
        self.single_mode = True  # 关卡模式强制单人

        # 初始化关卡控制器
        self.level_controller = LevelController()
        self.level_controller.start_level(level)

        # 重置玩家
        if self.player1:
            self.player1.lives = PLAYER_LIVES
            self.player1.effects.clear()
            self.player1.x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.alive = True

        # 关卡模式没有 P2
        self.player2 = None

        # 设置地图
        self.current_map = get_map_id(level)
        self._init_level()

        # 保存进度
        save_progress(level)

    def _init_level_for_level_mode(self):
        """关卡模式初始化"""
        from level_data import get_map_id
        # 重置玩家状态
        if self.player1:
            self.player1.lives = PLAYER_LIVES
            self.player1.effects.clear()
        if self.player2:
            self.player2.lives = PLAYER_LIVES
            self.player2.effects.clear()

        # 设置地图（轮换）
        self.current_map = get_map_id(self.level)
        # 重置敌人、墙壁等
        self._init_level()

    def _update_level_state(self, dt):
        """更新关卡状态"""
        if not self.level_controller:
            return

        # 获取当前存活的敌人数量
        enemies_alive = len([e for e in self.enemies if e.alive])

        result = self.level_controller.update(dt, enemies_alive)

        if result == 'spawn_enemy':
            self._spawn_enemy_for_level()
        elif result == 'spawn_boss':
            self._spawn_boss()
        elif result == 'level_cleared':
            self._on_level_cleared()
        elif result == 'boss_defeated':
            self._on_boss_defeated()
        elif result == 'game_victory':
            self._on_game_victory()
        elif result == 'next_level':
            self._go_to_next_level()
        elif result == 'gameover':
            self._on_level_failed()
        elif result == 'victory_done':
            # 通关后返回主菜单
            self.state = "menu"
            self.menu = Menu(self.screen)
    
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

        # 生成地图
        if self.game_mode == "level":
            from level_data import get_map_id
            self.current_map = get_map_id(self.level)
        else:
            self.current_map = random.randint(0, 4)
        self._build_map(self.current_map)

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

        # 关卡模式：没有 P2
        if self.game_mode == "level":
            self.player2 = None
            self.single_mode = True
        elif not self.single_mode:
            # 双人模式：创建 P2
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

        if self.game_mode == "endless" and not self.pvp_mode:
            for i in range(ENEMY_COUNT):
                self._spawn_enemy()

    def _spawn_enemy_for_level(self):
        """关卡模式生成敌人"""
        from level_data import get_level_config, ENEMY_TYPES
        config = get_level_config(self.level)
        if not config:
            return

        enemy_type = config.get('enemy_type', 'normal')
        type_data = ENEMY_TYPES.get(enemy_type, ENEMY_TYPES['normal'])

        # 获取敌人属性
        hp = type_data['hp']
        speed_mult = type_data['speed_mult']
        color = type_data['color']

        # 生成位置
        spawn_positions = [
            (GRID_SIZE - 2, 1),
            (GRID_SIZE // 2, 1),
            (1, 1),
        ]
        col, row = random.choice(spawn_positions)
        x = GRID_OFFSET_X + col * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
        y = GRID_OFFSET_Y + row * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2

        # 检查位置是否被占用
        test_rect = pygame.Rect(x, y, TANK_SIZE, TANK_SIZE)
        for enemy in self.enemies:
            if test_rect.colliderect(enemy.rect):
                return

        # 创建敌人
        tank = Tank(x, y, color, TANK_SPEED * speed_mult, is_player=False)
        tank.lives = hp  # 设置血量
        tank.enemy_type = enemy_type
        self.enemies.append(tank)
        self.enemy_ais.append(EnemyAI(tank, self))

    def _spawn_boss(self):
        from level_data import BOSS_TYPES
        from boss import Boss

        boss_config = BOSS_TYPES.get(self.level, BOSS_TYPES.get(5))
        if not boss_config:
            return

        boss_size = int(TANK_SIZE * boss_config['size_mult'])
        x = GRID_OFFSET_X + GRID_SIZE * CELL_SIZE // 2 - boss_size // 2
        y = GRID_OFFSET_Y + CELL_SIZE * 2

        boss = Boss(
            x, y,
            hp=boss_config['hp'],
            size_mult=boss_config['size_mult'],
            speed_mult=boss_config['speed_mult'],
            bullet_damage=boss_config['bullet_damage'],
            bullet_speed_mult=boss_config['bullet_speed_mult']
        )
        # 确保 Boss 的位置和血量正确
        boss.x = x
        boss.y = y
        boss.lives = boss_config['hp']  # 确保血量设置
        boss.max_hp = boss_config['hp']

        self.enemies.append(boss)
        self.enemy_ais.append(EnemyAI(boss, self))

        if self.level_controller:
            self.level_controller.on_boss_spawned()

    def _update_level_enemy_spawn(self, dt):
        """关卡模式的敌人生成控制"""
        if not self.level_controller:
            return

        # 获取当前存活的敌人数量
        enemies_alive = len([e for e in self.enemies if e.alive])
        result = self.level_controller.update(dt, enemies_alive)

        if result == 'spawn_enemy':
            self._spawn_enemy_for_level()
        elif result == 'spawn_boss':
            self._spawn_boss()
        elif result == 'level_cleared':
            self._on_level_cleared()
        elif result == 'boss_defeated':
            self._on_boss_defeated()
        elif result == 'game_victory':
            self._on_game_victory()
        elif result == 'next_level':
            self._go_to_next_level()
        elif result == 'gameover':
            self._on_level_failed()
        elif result == 'victory_done':
            self._on_victory_done()

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
        """无尽模式生成敌人"""
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

        # ===== 更新玩家 =====
        if self.player1 and self.player1.alive:
            self.player1.update(dt)
        if not self.single_mode and self.player2 and self.player2.alive:
            self.player2.update(dt)

        # ===== 道具更新 =====
        self._update_powerups(dt)

        # ===== 敌人更新 =====
        if not self.pvp_mode:
            # 更新敌人 AI
            for ai in self.enemy_ais[:]:
                ai.update(dt)

            # 清除死亡敌人
            for i in range(len(self.enemies) - 1, -1, -1):
                if not self.enemies[i].alive:
                    self.enemies.pop(i)
                    self.enemy_ais.pop(i)

            # ===== 敌人生成 =====
            if self.game_mode == "endless":
                self.enemy_spawn_timer -= dt
                if self.enemy_spawn_timer <= 0 and len(self.enemies) < ENEMY_COUNT:
                    self._spawn_enemy()
                    self.enemy_spawn_timer = ENEMY_SPAWN_INTERVAL
            else:
                self._update_level_enemy_spawn(dt)

        # ===== 子弹碰撞 =====
        self._handle_bullet_collisions()

        # ===== 爆炸效果 =====
        for e in self.explosions[:]:
            e.update(dt)
            if not e.alive:
                self.explosions.remove(e)

        # 游戏结束检查
        if self.pvp_mode:
            # PVP模式：任一玩家死亡则结束
            if (self.player1 and not self.player1.alive) or (self.player2 and not self.player2.alive):
                if not self._gameover_played:
                    self.sound_manager.play('gameover')
                    self._gameover_played = True
                self.game_over = True
        else:
            # PVE模式：两个玩家都死亡才结束
            if self.player1 and not self.player1.alive and self.player2 and not self.player2.alive:
                self.game_over = True

        if self.single_mode:
            # 单人模式：P1 死亡则游戏结束
            if self.player1 and not self.player1.alive:
                if not self._gameover_played:
                    self.sound_manager.play('gameover')
                    self._gameover_played = True
                self.game_over = True
        else:
            # 双人模式：两个玩家都死亡才结束
            if self.player1 and not self.player1.alive and self.player2 and not self.player2.alive:
                if not self._gameover_played:
                    self.sound_manager.play('gameover')
                    self._gameover_played = True
                self.game_over = True

    def _on_level_cleared(self):
        """过关"""
        self.sound_manager.play('victory')
        print(f"Stage {self.level} Clear!")
        self.level_controller.state = LevelState.CLEARED
        self.level_controller.wait_timer = 20.0
        self.show_message = "STAGE CLEAR!"
        self.waiting_for_enter = True

    def _on_boss_defeated(self):
        """Boss 击败（非第10关）"""
        self.sound_manager.play('victory')
        print(f"Boss Defeated! Level {self.level}")
        self.show_message = "BOSS DEFEATED!"
        self.waiting_for_enter = True
        if self.level_controller:
            self.level_controller.state = LevelState.CLEARED
            self.level_controller.wait_timer = 999.0

    def _on_game_victory(self):
        """第10关通关胜利"""
        self.sound_manager.play('victory')
        print("Game Victory!")
        self.show_message = "GAME VICTORY!"
        self.waiting_for_enter = True
        if self.level_controller:
            self.level_controller.state = LevelState.VICTORY
            self.level_controller.wait_timer = 999.0
        from save_manager import save_progress
        save_progress(10)

    def _on_level_failed(self):
        """关卡失败"""
        print("Game Over!")
        self.game_over = True

    def continue_to_next(self):
        """玩家按 Enter 继续"""
        if not self.waiting_for_enter:
            return

        self.waiting_for_enter = False
        self.show_message = None

        # 检查是否是第10关胜利
        if self.level == 10:
            self._on_victory_done()
            return

        if self.level >= MAX_LEVEL:
            self._on_victory_done()
            return

        self.level += 1
        save_progress(self.level)

        # 重置关卡
        self.level_controller.start_level(self.level)

        # 重置玩家状态
        if self.player1:
            self.player1.lives = PLAYER_LIVES
            self.player1.effects.clear()
            self.player1.x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.alive = True
            self.player1.w = TANK_SIZE
            self.player1.h = TANK_SIZE

        # 清除敌人
        self.enemies.clear()
        self.enemy_ais.clear()

        # 重新初始化关卡
        self.current_map = get_map_id(self.level)
        self._init_level()

        # 设置关卡模式状态
        self.game_mode = "level"

    def _go_to_next_level(self):
        """进入下一关"""
        from level_data import MAX_LEVEL
        from save_manager import save_progress

        next_level = self.level + 1
        if next_level > MAX_LEVEL:
            self._on_game_victory()
            return

        self.level = next_level
        save_progress(self.level)
        self.level_controller.start_level(self.level)

        # 重置玩家状态
        if self.player1:
            self.player1.lives = PLAYER_LIVES
            self.player1.effects.clear()
            self.player1.x = GRID_OFFSET_X + CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.y = GRID_OFFSET_Y + (GRID_SIZE - 3) * CELL_SIZE + CELL_SIZE // 2 - TANK_SIZE // 2
            self.player1.alive = True
        if self.player2:
            self.player2.lives = PLAYER_LIVES
            self.player2.effects.clear()

        # 重置关卡
        self._init_level()

    def _on_victory_done(self):
        """通关后返回主菜单"""
        print("Returning to main menu")
        self.state = "menu"
        from menu import Menu
        self.menu = Menu(self.screen)
        self.game_over = False
        self.paused = False
        self.show_message = None
        self.waiting_for_enter = False

    def _handle_bullet_collisions(self):
        """处理子弹碰撞"""
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

            bullet_hit = False

            # ===== 子弹 vs 墙壁 =====
            for wall in self.walls[:]:
                if not wall.alive:
                    continue
                if bullet.rect.colliderect(wall.rect):
                    # Strength 子弹：范围爆炸
                    if bullet.damage >= 2 and bullet.is_player:
                        self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                        bullet.alive = False
                        bullet_hit = True
                        if not wall.is_steel:
                            wall.alive = False
                        self._add_explosion(bullet.x + bullet.w//2, bullet.y + bullet.h//2)
                    else:
                        bullet.alive = False
                        bullet_hit = True
                        if not wall.is_steel:
                            wall.alive = False
                        self._add_explosion(bullet.x + bullet.w//2, bullet.y + bullet.h//2)
                    break

            if bullet_hit or not bullet.alive:
                continue

            # ===== PVP 模式 =====
            if self.pvp_mode:
                if bullet.is_player and bullet.player_id == 1:
                    if self.player2 and self.player2.alive and bullet.rect.colliderect(self.player2.rect):
                        if bullet.damage >= 2:
                            # Strength 子弹：范围爆炸
                            self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                            bullet.alive = False
                        else:
                            bullet.alive = False
                            if 'protection' in self.player2.effects:
                                del self.player2.effects['protection']
                            else:
                                self.player2.lives -= bullet.damage
                                self._add_explosion(self.player2.x + self.player2.w//2, self.player2.y + self.player2.h//2)
                            if self.player2.lives <= 0:
                                self.player2.alive = False
                        continue

                if bullet.is_player and bullet.player_id == 2:
                    if self.player1 and self.player1.alive and bullet.rect.colliderect(self.player1.rect):
                        if bullet.damage >= 2:
                            # Strength 子弹：范围爆炸
                            self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                            bullet.alive = False
                        else:
                            bullet.alive = False
                            if 'protection' in self.player1.effects:
                                del self.player1.effects['protection']
                            else:
                                self.player1.lives -= bullet.damage
                                self._add_explosion(self.player1.x + self.player1.w//2, self.player1.y + self.player1.h//2)
                            if self.player1.lives <= 0:
                                self.player1.alive = False
                        continue

            # ===== PVE 模式 =====
            else:
                if bullet.is_player:
                    # 玩家子弹 vs 敌人
                    for enemy in self.enemies[:]:
                        if not enemy.alive:
                            continue
                        if bullet.rect.colliderect(enemy.rect):
                            # 检查是否为 Boss
                            if hasattr(enemy, 'is_boss') and enemy.is_boss:
                                # Boss 特殊处理
                                enemy.lives -= bullet.damage
                                bullet.alive = False
                                if enemy.lives <= 0:
                                    enemy.alive = False
                                    self.score += 50
                                    self._add_explosion(enemy.x + enemy.w//2, enemy.y + enemy.h//2)
                                    if self.level == 10:
                                        self._on_game_victory()
                                    else:
                                        self._on_boss_defeated()
                            else:
                                # 普通敌人
                                if bullet.damage >= 2:
                                    # Strength 子弹：范围爆炸
                                    self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                                    bullet.alive = False
                                else:
                                    bullet.alive = False
                                    if 'protection' in enemy.effects:
                                        del enemy.effects['protection']
                                    else:
                                        enemy.lives -= bullet.damage
                                    if enemy.lives <= 0:
                                        enemy.alive = False
                                        self.score += 10
                                        self._add_explosion(enemy.x + enemy.w//2, enemy.y + enemy.h//2)
                            break
                else:
                    # 敌人子弹 vs 玩家
                    for player in [self.player1, self.player2]:
                        if player is None or not player.alive:
                            continue
                        if bullet.rect.colliderect(player.rect):
                            if bullet.damage >= 2:
                                # Strength 子弹：范围爆炸
                                self._explode_at(bullet.x + bullet.w//2, bullet.y + bullet.h//2, bullet.is_player)
                                bullet.alive = False
                            else:
                                bullet.alive = False
                                if 'protection' in player.effects:
                                    del player.effects['protection']
                                else:
                                    player.lives -= bullet.damage
                                    self._add_explosion(player.x + player.w//2, player.y + player.h//2)
                                if player.lives <= 0:
                                    player.alive = False
                            break

        # 清理死亡子弹
        if self.player1:
            self.player1.bullets = [b for b in self.player1.bullets if b.alive]
        if self.player2:
            self.player2.bullets = [b for b in self.player2.bullets if b.alive]
        if not self.pvp_mode:
            for enemy in self.enemies:
                enemy.bullets = [b for b in enemy.bullets if b.alive]

    def _add_explosion(self, x, y):
        self.sound_manager.play('explode')
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
        self.sound_manager.play('powerup')
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
            self.sound_manager.play('shoot')

    def player2_shoot(self):
        if self.player2 and self.player2.alive:
            self.player2.shoot()
            self.sound_manager.play('shoot')

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

        self._draw_ui(screen)

        # ===== 字幕显示 =====
        if hasattr(self, 'show_message') and self.show_message:
            # 半透明背景
            overlay = pygame.Surface((WINDOW_WIDTH, WINDOW_HEIGHT), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 150))
            screen.blit(overlay, (0, 0))

            # 主标题
            font_big = pygame.font.Font(None, 72)
            text = font_big.render(self.show_message, True, (255, 255, 100))
            text_rect = text.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 - 40))
            screen.blit(text, text_rect)

            # 副标题（提示按 Enter）
            font_small = pygame.font.Font(None, 28)
            if self.waiting_for_enter:
                hint = font_small.render("Press ENTER to continue", True, (200, 200, 200))
                hint_rect = hint.get_rect(center=(WINDOW_WIDTH // 2, WINDOW_HEIGHT // 2 + 40))
                screen.blit(hint, hint_rect)

        for ex in self.explosions:
            ex.draw(screen)
        
        for p in self.powerups:
            p.draw(screen)

    def handle_key(self, key):
        if self.state != "playing":
            return self.menu.handle_event(pygame.event.Event(pygame.KEYDOWN, key=key))

    def _draw_ui(self, screen):
        font = pygame.font.SysFont("Consolas", 18)
        font_big = pygame.font.SysFont("Consolas", 36)
        font_small = pygame.font.SysFont("Consolas", 14)
        ui_y = 10

        map_names = ["Empty", "Cross", "Maze", "Bunker", "Sym"]

        # ===== Mode / Level 显示 =====
        if self.game_mode == "level":
            mode_text = f"Level {self.level}/{MAX_LEVEL}"
        elif self.pvp_mode:
            mode_text = "PVP"
        else:
            mode_text = "PVE"
    
        text = font.render(mode_text, True, COLORS['text'])
        screen.blit(text, (10, ui_y))
        ui_y += 25

        # Score (PVE only)
        if not self.pvp_mode and self.game_mode == "endless":
            text = font.render(f"Score: {self.score}", True, COLORS['text'])
            screen.blit(text, (10, ui_y))
            ui_y += 25

        # ===== 关卡模式：显示进度 =====
        if self.game_mode == "level" and self.level_controller:
            progress_text = f"Enemy: {len([e for e in self.enemies if e.alive])} / {self.level_controller.enemies_total}"
            text = font.render(progress_text, True, COLORS['text'])
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
        if not self.pvp_mode and self.game_mode == "endless":
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
        """Strength 子弹爆炸 - 范围伤害"""
        radius = TANK_SIZE * 2

        # ===== 1. 伤害所有敌人 =====
        for enemy in self.enemies[:]:
            if not enemy.alive:
                continue
            ex, ey = enemy.get_center()
            dist = math.hypot(ex - x, ey - y)
            if dist <= radius:
                # 检查是否有保护
                if 'protection' in enemy.effects:
                    del enemy.effects['protection']
                else:
                    enemy.lives -= 2
                    if enemy.lives <= 0:
                        enemy.alive = False
                        self.score += 10
                        self._add_explosion(ex, ey)

        # ===== 2. PVP 模式：伤害玩家 =====
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
                            self._add_explosion(px, py)
                            if player.lives <= 0:
                                player.alive = False

        # ===== 3. 摧毁普通墙壁 =====
        for wall in self.walls[:]:
            if not wall.alive or wall.is_steel:
                continue
            wx, wy = wall.x + wall.w//2, wall.y + wall.h//2
            if math.hypot(wx - x, wy - y) <= radius:
                wall.alive = False

        # ===== 4. 爆炸特效 =====
        for _ in range(8):
            self._add_explosion(
                x + random.randint(-radius//2, radius//2),
                y + random.randint(-radius//2, radius//2)
            )