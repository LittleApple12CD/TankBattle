import pygame
import sys
from config import WINDOW_WIDTH, WINDOW_HEIGHT


class Menu:
    def __init__(self, screen):
        self.screen = screen
        self.font_title = pygame.font.Font(None, 72)
        self.font_item = pygame.font.Font(None, 36)
        self.font_sub = pygame.font.Font(None, 28)

        self.main_items = ["Single Player", "Multiplayer", "Mod", "Settings","Exit"]
        self.multi_items = ["Local", "Lan", "Online", "Back"]
        self.mode_items = ["PVP", "PVE", "Back"]

        self.state = "main"
        self.selected = 0
        self.items = self.main_items

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

        if self.state == "main":
            if choice == "Single Player":
                return "single_player"
            elif choice == "Multiplayer":
                self.state = "multiplayer"
                self.items = self.multi_items
                self.selected = 0
                return None
            elif choice == "Mod":
                print("Mod mode - reserved for future")
                return None
            elif choice == "Exit":
                pygame.quit()
                sys.exit()

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
        if self.state == "multiplayer":
            self.state = "main"
            self.items = self.main_items
            self.selected = 0
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

        title = self.font_title.render("Tank Battle", True, (255, 255, 255))
        title_rect = title.get_rect(center=(WINDOW_WIDTH // 2, 200))
        self.screen.blit(title, title_rect)

        y_start = 350
        for i, item in enumerate(self.items):
            color = (255, 255, 255) if i == self.selected else (150, 150, 160)
            text = self.font_item.render(item, True, color)
            rect = text.get_rect(center=(WINDOW_WIDTH // 2, y_start + i * 50))
            self.screen.blit(text, rect)

        if self.state == "multiplayer":
            hint = self.font_sub.render("Select a multiplayer mode", True, (100, 100, 120))
        elif self.state == "mode":
            hint = self.font_sub.render("Select game mode", True, (100, 100, 120))
        else:
            hint = self.font_sub.render("Use UP/DOWN to navigate, ENTER to select, ESC to exit", True, (100, 100, 120))
        self.screen.blit(hint, (20, WINDOW_HEIGHT - 40))

        ver = self.font_sub.render("v1.4", True, (80, 80, 90))
        self.screen.blit(ver, (WINDOW_WIDTH - 80, WINDOW_HEIGHT - 30))