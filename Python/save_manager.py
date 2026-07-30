# ========================================
# 坦克大战 - 存档管理
# ========================================

import os
import json

SAVE_DIR = "PlayerData"
SAVE_FILE = "LevelSaves.dat"
SAVE_PATH = os.path.join(SAVE_DIR, SAVE_FILE)


def ensure_save_dir():
    """确保存档目录存在"""
    if not os.path.exists(SAVE_DIR):
        os.makedirs(SAVE_DIR)


def save_progress(level):
    """保存关卡进度"""
    ensure_save_dir()
    try:
        with open(SAVE_PATH, 'w') as f:
            json.dump({'level': level}, f)
        return True
    except Exception as e:
        print(f"保存失败: {e}")
        return False


def load_progress():
    """读取关卡进度，返回关卡编号，如果没有存档返回 1"""
    ensure_save_dir()
    if not os.path.exists(SAVE_PATH):
        return 1  # 没有存档则从第1关开始
    try:
        with open(SAVE_PATH, 'r') as f:
            data = json.load(f)
            return data.get('level', 1)
    except Exception:
        return 1


def has_save():
    """检查是否有存档"""
    return os.path.exists(SAVE_PATH)