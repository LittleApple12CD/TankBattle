# ========================================
# 坦克大战 - 关卡配置数据
# ========================================

# 敌人类型
ENEMY_TYPES = {
    'normal': {'hp': 1, 'speed_mult': 1.0, 'color': (200, 50, 50), 'label': 'Normal'},
    'tough': {'hp': 2, 'speed_mult': 1.0, 'color': (255, 150, 50), 'label': 'Tough'},
    'elite': {'hp': 3, 'speed_mult': 1.5, 'color': (200, 100, 255), 'label': 'Elite'},
}

# Boss 配置
BOSS_TYPES = {
    5: {'hp': 5, 'size_mult': 1.5, 'speed_mult': 1.5, 'bullet_damage': 2, 'bullet_speed_mult': 1.0},
    10: {'hp': 10, 'size_mult': 2.0, 'speed_mult': 2.0, 'bullet_damage': 2, 'bullet_speed_mult': 1.5},
}

# 10 关配置
LEVELS = {
    1: {'enemy_type': 'normal', 'count': 6, 'spawn_interval': 4.0, 'boss': False},
    2: {'enemy_type': 'normal', 'count': 8, 'spawn_interval': 4.0, 'boss': False},
    3: {'enemy_type': 'normal', 'count': 10, 'spawn_interval': 4.0, 'boss': False},
    4: {'enemy_type': 'tough', 'count': 6, 'spawn_interval': 4.0, 'boss': False},
    5: {'enemy_type': None, 'count': 1, 'spawn_interval': 4.0, 'boss': True, 'boss_id': 5},
    6: {'enemy_type': 'tough', 'count': 8, 'spawn_interval': 3.0, 'boss': False},
    7: {'enemy_type': 'normal', 'count': 10, 'spawn_interval': 4.0, 'boss': False},
    8: {'enemy_type': 'normal', 'count': 12, 'spawn_interval': 4.0, 'boss': False},
    9: {'enemy_type': 'elite', 'count': 8, 'spawn_interval': 4.0, 'boss': False},
    10: {'enemy_type': None, 'count': 1, 'spawn_interval': 4.0, 'boss': True, 'boss_id': 10},
}

MAX_LEVEL = 10

# 地图轮换（5张地图循环）
MAP_ROTATION = [0, 1, 2, 3, 4]  # 对应 MapGenerator 的 0-4

def get_map_id(level):
    """根据关卡获取地图 ID（5张地图循环）"""
    return MAP_ROTATION[(level - 1) % len(MAP_ROTATION)]

def get_level_config(level):
    """获取关卡配置"""
    return LEVELS.get(level, None)

def is_boss_level(level):
    """判断是否为 Boss 关"""
    return LEVELS.get(level, {}).get('boss', False)