# ========================================
# 坦克大战 - 主程序
# ========================================

import pygame
import sys
from config import *
from game import Game
from menu import Menu


class TankBattle:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption("Tank Battle - Python")
        self.clock = pygame.time.Clock()
        
        # 游戏状态：menu, playing
        self.state = "menu"
        self.menu = Menu(self.screen)
        self.game = Game()
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
        """启动游戏"""
        # 重置游戏状态
        self.game = Game()
        
        # 根据模式设置
        if mode == "single_player":
            self.game.single_mode = True
            self.game.pvp_mode = False
            self.game.enemy_count = ENEMY_COUNT * 2
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

    def run(self):
        while self.running:
            dt = self.clock.tick(FPS) / 1000.0
            if dt > 0.05:
                dt = 0.05
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        if self.state == "playing":
                            # 游戏中 → 返回主菜单
                            self.state = "menu"
                            self.menu.reset()
                            self.game.paused = False
                            continue
                        else:
                            # 菜单中 → 交给 menu 处理（子菜单返回上一级，主菜单退出）
                            menu_result = self.menu.handle_event(event)
                            continue

                    # ===== 菜单模式 =====
                    if self.state == "menu":
                        result = self.menu.handle_event(event)
                        if result == "single_player":
                            self.start_game("single_player")
                        elif result == "pvp":
                            self.start_game("pvp")
                        elif result == "pve":
                            self.start_game("pve")
                        continue

                    # ===== 游戏模式（禁用 O 和 G） =====
                    if event.key == pygame.K_o or event.key == pygame.K_g:
                        continue

                    if event.key == pygame.K_p:
                        self.game.paused = not self.game.paused
                    elif event.key == pygame.K_r:
                        self.game = Game()
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

            # ---- 持续移动 P1 ----
            if self.state == "playing" and not self.game.paused and not self.game.game_over:
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

            # ---- 持续移动 P2 ----
            if self.state == "playing" and not self.game.paused and not self.game.game_over:
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

            # ---- 更新 ----
            if self.state == "playing":
                self.game.update(dt)

            # ---- 渲染 ----
            if self.state == "menu":
                self.menu.draw()
            else:
                self.game.draw(self.screen)
            
            pygame.display.flip()

        pygame.quit()
        sys.exit(0)


if __name__ == "__main__":
    game = TankBattle()
    game.run()