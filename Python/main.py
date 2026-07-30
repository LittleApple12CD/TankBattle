# ========================================
# 坦克大战 - 主程序
# ========================================

import pygame
import sys
from config import *
from game import Game
from menu import Menu
from save_manager import has_save, load_progress


class TankBattle:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption("Tank Battle - Python")
        self.clock = pygame.time.Clock()
        
        self.state = "menu"
        self.menu = Menu(self.screen)
        self.game = None
        self.running = True
        
        # P1 按键状态
        self.p1_keys = {
            'left': False, 'right': False, 'up': False, 'down': False,
            'shoot': False,
        }
        # P2 按键状态
        self.p2_keys = {
            'left': False, 'right': False, 'up': False, 'down': False,
            'shoot': False,
        }

    def start_game(self, mode):
        """无尽模式启动"""
        self.game = Game(self.screen)
        self.game.game_mode = "endless"
        
        if mode == "single_player":
            self.game.single_mode = True
            self.game.pvp_mode = False
            self.game.enemy_count = ENEMY_COUNT
            self.game._init_level()
        elif mode == "pvp":
            self.game.single_mode = False
            self.game.pvp_mode = True
            self.game.enemy_count = 0
            self.game._init_level()
        elif mode == "pve":
            self.game.single_mode = False
            self.game.pvp_mode = False
            self.game.enemy_count = ENEMY_COUNT
            self.game._init_level()
        
        self.state = "playing"

    def start_level_mode(self, level):
        """关卡模式启动"""
        self.game = Game(self.screen)
        self.game.start_level_mode(level)
        self.state = "playing"

    def run(self):
        while self.running:
            dt = self.clock.tick(FPS) / 1000.0
            if dt > 0.05:
                dt = 0.05

            # ===== 事件处理 =====
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        if self.state == "playing":
                            self.state = "menu"
                            self.menu = Menu(self.screen)
                            if self.game:
                                self.game.paused = False
                            continue
                        else:
                            self.running = False
                            continue

                    # 关卡模式按 Enter 继续
                    if event.key == pygame.K_RETURN:
                        if self.state == "playing" and self.game and hasattr(self.game, 'waiting_for_enter') and self.game.waiting_for_enter:
                            self.game.continue_to_next()
                            continue

                    # ===== 菜单模式 =====
                    if self.state == "menu":
                        result = self.menu.handle_event(event)
                        if result == "endless_mode":
                            self.start_game("single_player")
                        elif result == "new_game":
                            self.start_level_mode(1)
                        elif result == "load_game":
                            if has_save():
                                level = load_progress()
                                self.start_level_mode(level)
                            else:
                                print("No save found, starting from level 1")
                                self.start_level_mode(1)
                        elif result == "pvp":
                            self.start_game("pvp")
                        elif result == "pve":
                            self.start_game("pve")
                        continue

                    # ===== 游戏模式（禁用 O 和 G） =====
                    if event.key == pygame.K_o or event.key == pygame.K_g:
                        continue

                    if event.key == pygame.K_p:
                        if self.game:
                            self.game.paused = not self.game.paused
                    elif event.key == pygame.K_r:
                        if self.game:
                            self.game._init_level()
                    
                    # ---- P1 控制 (方向键) ----
                    elif event.key == pygame.K_LEFT:
                        self.p1_keys['left'] = True
                    elif event.key == pygame.K_RIGHT:
                        self.p1_keys['right'] = True
                    elif event.key == pygame.K_UP:
                        self.p1_keys['up'] = True
                    elif event.key == pygame.K_DOWN:
                        self.p1_keys['down'] = True
                    elif event.key == pygame.K_SPACE:
                        self.p1_keys['shoot'] = True
                        if self.game:
                            self.game.player1_shoot()
                    
                    # ---- P2 控制 (WASD) ----
                    elif event.key == pygame.K_a:
                        self.p2_keys['left'] = True
                    elif event.key == pygame.K_d:
                        self.p2_keys['right'] = True
                    elif event.key == pygame.K_w:
                        self.p2_keys['up'] = True
                    elif event.key == pygame.K_s:
                        self.p2_keys['down'] = True
                    elif event.key == pygame.K_j:
                        self.p2_keys['shoot'] = True
                        if self.game:
                            self.game.player2_shoot()

                elif event.type == pygame.KEYUP:
                    # ---- P1 释放 ----
                    if event.key == pygame.K_LEFT:
                        self.p1_keys['left'] = False
                    elif event.key == pygame.K_RIGHT:
                        self.p1_keys['right'] = False
                    elif event.key == pygame.K_UP:
                        self.p1_keys['up'] = False
                    elif event.key == pygame.K_DOWN:
                        self.p1_keys['down'] = False
                    elif event.key == pygame.K_SPACE:
                        self.p1_keys['shoot'] = False
                    
                    # ---- P2 释放 ----
                    elif event.key == pygame.K_a:
                        self.p2_keys['left'] = False
                    elif event.key == pygame.K_d:
                        self.p2_keys['right'] = False
                    elif event.key == pygame.K_w:
                        self.p2_keys['up'] = False
                    elif event.key == pygame.K_s:
                        self.p2_keys['down'] = False
                    elif event.key == pygame.K_j:
                        self.p2_keys['shoot'] = False

            # ===== 检测游戏是否请求返回菜单 =====
            if self.state == "playing" and self.game and hasattr(self.game, 'state') and self.game.state == "menu":
                self.state = "menu"
                self.menu = Menu(self.screen)
                continue

            # ===== 更新 =====
            if self.state == "playing" and self.game:
                if not self.game.paused and not self.game.game_over:
                    # P1 移动
                    dx = dy = 0
                    if self.p1_keys['left']:
                        dx = -1
                    elif self.p1_keys['right']:
                        dx = 1
                    elif self.p1_keys['up']:
                        dy = -1
                    elif self.p1_keys['down']:
                        dy = 1
                    if dx != 0 or dy != 0:
                        self.game.move_player1(dx, dy, dt)

                    # P2 移动
                    dx = dy = 0
                    if self.p2_keys['left']:
                        dx = -1
                    elif self.p2_keys['right']:
                        dx = 1
                    elif self.p2_keys['up']:
                        dy = -1
                    elif self.p2_keys['down']:
                        dy = 1
                    if dx != 0 or dy != 0:
                        self.game.move_player2(dx, dy, dt)

                self.game.update(dt)

            # ===== 渲染 =====
            if self.state == "menu":
                self.menu.draw()
            elif self.state == "playing" and self.game:
                self.game.draw(self.screen)
            
            pygame.display.flip()

        pygame.quit()
        sys.exit(0)


if __name__ == "__main__":
    game = TankBattle()
    game.run()