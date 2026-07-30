# ========================================
# 坦克大战 - 菜单
# ========================================

import pygame
import sys
from config import WINDOW_WIDTH, WINDOW_HEIGHT


class Menu:
    def __init__(self, screen):
        self.screen = screen
        self.font_title = pygame.font.Font(None, 72)
        self.font_item = pygame.font.Font(None, 36)
        self.font_sub = pygame.font.Font(None, 28)

        # ===== 菜单项 =====
        self.main_items = ["Single Player", "Multiplayer", "Mod", "Settings", "Exit"]

        # Single Player 子菜单
        self.single_items = ["Endless Mode", "Level Mode", "Back"]

        # Level Mode 子菜单
        self.level_items = ["Load Game", "New Game", "Back"]

        # Multiplayer 子菜单
        self.multi_items = ["Local", "Lan", "Server", "Online", "Back"]
        self.mode_items = ["PVP", "PVE", "Back"]

        # ===== 状态 =====
        self.state = "main"  # main, single, level, multiplayer, mode
        self.selected = 0
        self.items = self.main_items
        self.return_state = "main"

    def handle_event(self, event):
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP:
                self.selected = (self.selected - 1) % len(self.items)
                return None
            elif event.key == pygame.K_DOWN:
                self.selected = (self.selected + 1) % len(self.items)
                return None
            elif event.key == pygame.K_RETURN:
                return self.select_current()
            elif event.key == pygame.K_ESCAPE:
                if self.state == "main":
                    pygame.quit()
                    sys.exit()
                else:
                    self.go_back()
                    return None
        return None

    def select_current(self):
        choice = self.items[self.selected]

        # ===== 主菜单 =====
        if self.state == "main":
            if choice == "Single Player":
                self.state = "single"
                self.items = self.single_items
                self.selected = 0
                return None
            elif choice == "Multiplayer":
                self.state = "multiplayer"
                self.items = self.multi_items
                self.selected = 0
                return None
            elif choice == "Mod (Reserved)":
                print("Mod mode - reserved for future")
                return None
            elif choice == "Settings (Reserved)":
                print("Settings - reserved for future")
                return None
            elif choice == "Exit":
                pygame.quit()
                sys.exit()

        # ===== Single Player 子菜单 =====
        elif self.state == "single":
            if choice == "Endless Mode":
                return "endless_mode"
            elif choice == "Level Mode":
                self.state = "level"
                self.items = self.level_items
                self.selected = 0
                return None
            elif choice == "Back":
                self.go_back()
                return None

        # ===== Level Mode 子菜单 =====
        elif self.state == "level":
            if choice == "Load Game":
                return "load_game"
            elif choice == "New Game":
                return "new_game"
            elif choice == "Back":
                self.go_back()
                return None

        # ===== Multiplayer 子菜单 =====
        elif self.state == "multiplayer":
            if choice == "Local":
                self.state = "mode"
                self.items = self.mode_items
                self.selected = 0
                return None
            elif choice == "Lan (Reserved)":
                print("Lan mode - reserved for future")
                return None
            elif choice == "Online (Reserved)":
                print("Online mode - reserved for future")
                return None
            elif choice == "Back":
                self.go_back()
                return None

        # ===== Mode 子菜单（PVP/PVE） =====
        elif self.state == "mode":
            if choice == "PVP":
                return "pvp"
            elif choice == "PVE":
                return "pve"
            elif choice == "Back":
                self.go_back()
                return None

        return None

    def go_back(self):
        if self.state == "single":
            self.state = "main"
            self.items = self.main_items
        elif self.state == "level":
            self.state = "single"
            self.items = self.single_items
        elif self.state == "multiplayer":
            self.state = "main"
            self.items = self.main_items
        elif self.state == "mode":
            self.state = "multiplayer"
            self.items = self.multi_items
        self.selected = 0

    def reset(self):
        self.state = "main"
        self.items = self.main_items
        self.selected = 0

    def draw(self):
        self.screen.fill((20, 20, 30))

        # 标题
        title = self.font_title.render("Tank Battle", True, (255, 255, 255))
        title_rect = title.get_rect(center=(WINDOW_WIDTH // 2, 200))
        self.screen.blit(title, title_rect)

        # 菜单项
        y_start = 350
        for i, item in enumerate(self.items):
            color = (255, 255, 255) if i == self.selected else (150, 150, 160)
            text = self.font_item.render(item, True, color)
            rect = text.get_rect(center=(WINDOW_WIDTH // 2, y_start + i * 50))
            self.screen.blit(text, rect)

        # 底部提示
        hints = {
            "main": "Use UP/DOWN to navigate, ENTER to select, ESC to exit",
            "single": "Select game mode",
            "level": "Load saved progress or start fresh",
            "multiplayer": "Select a multiplayer mode",
            "mode": "Select game mode"
        }
        hint = hints.get(self.state, "")
        if hint:
            hint_text = self.font_sub.render(hint, True, (100, 100, 120))
            self.screen.blit(hint_text, (20, WINDOW_HEIGHT - 40))

        # 版本号
        ver = self.font_sub.render("v1.6", True, (80, 80, 90))
        self.screen.blit(ver, (WINDOW_WIDTH - 80, WINDOW_HEIGHT - 30))

        # 当前路径显示
        path = []
        if self.state == "single" or self.state == "level":
            path.append("Single Player")
        if self.state == "level":
            path.append("Level Mode")
        if self.state == "multiplayer" or self.state == "mode":
            path.append("Multiplayer")
        if self.state == "mode":
            path.append("Local")
        if path:
            path_text = self.font_sub.render(" > ".join(path), True, (100, 100, 120))
            self.screen.blit(path_text, (20, 20))