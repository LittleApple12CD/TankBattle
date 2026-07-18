# ========================================
# 坦克大战 - 配置文件
# ========================================

# 窗口
WINDOW_WIDTH = 1600
WINDOW_HEIGHT = 900

# 网格布局
GRID_SIZE = 13
CELL_SIZE = 50
GRID_OFFSET_X = (WINDOW_WIDTH - GRID_SIZE * CELL_SIZE) // 2
GRID_OFFSET_Y = (WINDOW_HEIGHT - GRID_SIZE * CELL_SIZE) // 2

FPS = 60

# 颜色
COLORS = {
    'bg': (20, 20, 30),
    'grid': (30, 30, 40),
    'player1': (0, 200, 80),      # P1: 绿色
    'player2': (255, 180, 50),    # P2: 金色
    'enemy': (200, 50, 50),
    'bullet_p1': (100, 255, 100),
    'bullet_p2': (255, 220, 100),
    'bullet_enemy': (255, 100, 100),
    'wall': (150, 150, 150),
    'steel': (180, 180, 200),
    'base': (255, 200, 50),
    'explosion': [(255, 200, 50), (255, 150, 0), (255, 50, 0)],
    'text': (255, 255, 255),
    'text_dim': (150, 150, 160),
    'pvp_indicator': (255, 100, 100),
}

# 坦克（使用你的参数）
TANK_SIZE = 36
TANK_SPEED = 384
PLAYER_LIVES = 3
SHOT_COOLDOWN = 0.5

# 子弹（使用你的参数）
BULLET_SIZE = 8
BULLET_SPEED = 360
MAX_BULLETS = 3

# 敌人（PVE模式）
ENEMY_COUNT = 4
ENEMY_SPAWN_INTERVAL = 4.0
AI_DIRECTION_CHANGE = 2.0
AI_SHOOT_CHANCE = 0.4

# 步进移动
MOVE_STEP = 2.0