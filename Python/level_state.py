# ========================================
# 坦克大战 - 关卡状态
# ========================================

from enum import Enum


class LevelState(Enum):
    """关卡状态"""
    IDLE = 0              # 初始状态
    SPAWNING = 1          # 生成敌人中
    WAITING = 2           # 等待敌人被消灭
    CLEARED = 3           # 过关
    GAMEOVER = 4          # 失败
    BOSS_SPAWNING = 5     # Boss 生成中
    BOSS_FIGHT = 6        # Boss 战斗中
    VICTORY = 7           # 通关胜利


class LevelController:
    """关卡控制器"""

    def __init__(self):
        self.current_level = 1
        self.state = LevelState.IDLE
        self.enemies_spawned = 0
        self.enemies_total = 0
        self.enemy_type = 'normal'
        self.spawn_timer = 0
        self.spawn_interval = 4.0
        self.is_boss_level = False
        self.boss_id = None
        self.boss_spawned = False
        self.wait_timer = 0
        self.wait_duration = 2.0  # 过关后等待秒数
        self.game_over = False

    def start_level(self, level):
        """开始关卡"""
        from level_data import get_level_config, is_boss_level
        config = get_level_config(level)
        if not config:
            return

        self.current_level = level
        self.enemies_spawned = 0
        self.enemies_total = config['count']
        self.enemy_type = config.get('enemy_type', 'normal')
        self.spawn_interval = config['spawn_interval']
        self.is_boss_level = config.get('boss', False)
        self.boss_id = config.get('boss_id', None)
        self.boss_spawned = False
        self.state = LevelState.SPAWNING
        self.spawn_timer = 0
        self.game_over = False

    def update(self, dt, enemies_alive):
        """更新关卡状态"""
        if self.state == LevelState.CLEARED:
            self.wait_timer -= dt
            if self.wait_timer <= 0:
                return 'next_level'
            return 'waiting'

        if self.state == LevelState.GAMEOVER:
            return 'gameover'

        if self.state == LevelState.VICTORY:
            self.wait_timer -= dt
            if self.wait_timer <= 0:
                return 'victory_done'
            return 'waiting'

        # 生成敌人
        if self.state == LevelState.SPAWNING:
            if self.is_boss_level and self.boss_spawned:
                return 'boss_spawned'

            # Boss 关：特殊处理
            if self.is_boss_level:
                self.spawn_timer -= dt
                if self.spawn_timer <= 0 and not self.boss_spawned:
                    self.state = LevelState.BOSS_SPAWNING
                    return 'spawn_boss'
                return 'spawning'

            # 普通关：生成敌人
            if self.enemies_spawned < self.enemies_total:
                self.spawn_timer -= dt
                if self.spawn_timer <= 0:
                    self.spawn_timer = self.spawn_interval
                    self.enemies_spawned += 1
                    return 'spawn_enemy'
                return 'spawning'
            else:
                # 所有敌人生成完毕，等待消灭
                self.state = LevelState.WAITING
                return 'waiting'

        if self.state == LevelState.WAITING:
            if enemies_alive <= 0:
                # 所有敌人被消灭
                self.state = LevelState.CLEARED
                self.wait_timer = self.wait_duration
                if self.current_level == 10:
                    return 'game_victory'
                return 'level_cleared'
            return 'waiting'

        return 'idle'

    def on_enemy_killed(self):
        """敌人被消灭时调用"""
        if self.is_boss_level:
            # Boss 被击败
            self.state = LevelState.CLEARED
            self.wait_timer = 3.0
            if self.current_level == 10:
                return 'game_victory'
            return 'boss_defeated'
        return None

    def on_boss_spawned(self):
        """Boss 生成时调用"""
        self.boss_spawned = True
        self.state = LevelState.BOSS_FIGHT

    def get_progress(self):
        """获取进度百分比"""
        if self.enemies_total == 0:
            return 1.0
        return min(1.0, self.enemies_spawned / self.enemies_total)