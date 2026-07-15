# ========================================
# 坦克大战 - 主程序（双人版 + PVP）
# ========================================

import pygame
import sys
from config import *
from game import Game


class TankBattle:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption("TankBattle - Python")
        self.clock = pygame.time.Clock()
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

    def run(self):
        while self.running:
            dt = self.clock.tick(FPS) / 1000.0

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_ESCAPE:
                        self.running = False
                    elif event.key == pygame.K_p:
                        self.game.paused = not self.game.paused
                    elif event.key == pygame.K_r:
                        self.game = Game()
                    elif event.key == pygame.K_g:
                        self.game.toggle_pvp_mode()
                    elif event.key == pygame.K_o:
                        self.game.toggle_single_mode()
                    
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
                    # P1 释放
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
                    
                    # P2 释放
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
            if not self.game.paused and not self.game.game_over:
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
                    self.game.move_player1(dx, dy)

            # ---- 持续移动 P2 ----
            if not self.game.paused and not self.game.game_over:
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
                    self.game.move_player2(dx, dy)

            self.game.update(dt)
            self.game.draw(self.screen)
            pygame.display.flip()

        pygame.quit()
        sys.exit(0)


if __name__ == "__main__":
    game = TankBattle()
    game.run()