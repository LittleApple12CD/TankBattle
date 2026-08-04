# ========================================
# 坦克大战 - 声音管理器 (v1.7)
# ========================================

import pygame
import os
from config import *

class SoundManager:
    """声音管理器 - 单例模式"""
    
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance
    
    def __init__(self):
        if self._initialized:
            return
        
        self._initialized = True
        self.sounds = {}
        self.volume = 0.8
        self.muted = False
        self._loaded = False
        
        # 音效文件映射
        self.sound_files = {
            'shoot': 'shoot.wav',
            'explode': 'explode.wav',
            'powerup': 'powerup.wav',
            'victory': 'victory.wav',
            'gameover': 'gameover.wav',
        }
        
        # 尝试初始化 pygame.mixer
        try:
            pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=512)
            self._loaded = True
        except Exception as e:
            print(f"⚠️ 声音系统初始化失败: {e}")
            self._loaded = False
    
    def load_sounds(self):
        """加载所有音效"""
        if not self._loaded:
            return
        
        sound_dir = os.path.join(os.path.dirname(__file__), 'assets', 'sounds')
        
        for name, filename in self.sound_files.items():
            path = os.path.join(sound_dir, filename)
            try:
                if os.path.exists(path):
                    sound = pygame.mixer.Sound(path)
                    sound.set_volume(self.volume if not self.muted else 0)
                    self.sounds[name] = sound
                else:
                    print(f"⚠️ 音效文件不存在: {path}")
            except Exception as e:
                print(f"⚠️ 加载音效失败 {filename}: {e}")
    
    def play(self, name, volume=None):
        """播放音效"""
        if not self._loaded or self.muted:
            return
        
        sound = self.sounds.get(name)
        if sound:
            # 设置音量
            vol = volume if volume is not None else self.volume
            sound.set_volume(vol)
            sound.play()
    
    def set_volume(self, volume):
        """设置音量 (0.0 ~ 1.0)"""
        self.volume = max(0.0, min(1.0, volume))
        for sound in self.sounds.values():
            sound.set_volume(self.volume if not self.muted else 0)
    
    def toggle_mute(self):
        """切换静音"""
        self.muted = not self.muted
        vol = 0 if self.muted else self.volume
        for sound in self.sounds.values():
            sound.set_volume(vol)
        return self.muted
    
    def is_muted(self):
        return self.muted