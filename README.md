# 坦克大战

- **Tank Battle** 是一款经典的双人对战坦克游戏，支持 **PVE 合作模式** 和 **PVP 对战模式** 以及 **单人模式** 和 **关卡** 模式。本项目用 **Python、Java、C++** 三种语言实现，使用OpenGL图形API，C++版基于SFML3.0，Java版基于Swing，Python版基于Pygame
- **Tank Battle** is a classic two-player tank game that supports PVE co-op mode, PVP battle mode, as well as single-player mode and level mode. This project is implemented in Python, Java, and C, using the OpenGL graphics API. The C version is based on SFML 3.0, the Java version is based on Swing, and the Python version is based on Pygame.

## 游戏模式

- **关卡模式**：玩家击杀1-10关的敌人和boss，取得胜利
- **Stage Mode**: Players defeat enemies and bosses from levels 1 to 10 to win
- **PVE 合作模式**：P1 + P2 联手对抗红色 AI 坦克，消灭敌人获得分数
- **PVE Co-op Mode**: P1 and P2 team up to fight the red AI tanks and earn points by taking down enemies
- **PVP 对战模式**：P1 与 P2 互相射击，先消灭对方者获胜
- **PVP Battle Mode**: P1 and P2 shoot at each other, and the first to take out the other wins
- **5 种随机地图**：每次按 `R` 键随机切换地图布局
- **5 Random Maps**: Press `R` to randomly switch the map layout each time

---

## 控制方式

| 按键 | 功能 |
|------|------|
| **↑/↓/←/→** | P1 移动/P1 Move |
| **空格键(Space)** | P1 射击 P1 Shoot |
| **W/A/S/D** | P2 移动 P2 Move |
| **J 键** | P2 射击 P2 Shoot |
| **P 键** | 暂停/继续 Pause/Resume |
| **R 键** | 重新开始 Restart |
| **ESC 键** | 退出游戏 Exit Game |
| **Enter 键** | 确定/下一关 Confirm/Continue |

---

## 编译和运行
运行自带的编译脚本
Java版：运行compile.bat编译并启动游戏，运行build.bat来打包JAR
C++版：用MSYS2 UCRT64运行build.sh，选择MinGW编译或CMake编译
Python版：运行main.py启动游戏，使用package.bat来打包EXE

## 项目依赖
必要：Java 21
编译需要：MinGW 15.2.0
CMake 4.3.1
SFML 3.0.2
Python 3.12,Pygame,Pyinstaller
Enigma Virtual Box

## 项目结构

```
TankBattle/
Python/
├── ai.py
├── assets/
│ └── sounds/
│ ├── explode.wav
│ ├── gameover.wav
│ ├── powerup.wav
│ ├── shoot.wav
│ └── victory.wav
├── boss.py
├── config.py
├── entities.py
├── game.py
├── level_data.py
├── level_state.py
├── main.py
├── menu.py
├── package.bat
├── save_manager.py
└── sound_manager.py
Java/
├── assets/sounds/
├── explode.wav
├── gameover.wav
├── powerup.wav
├── shoot.wav
├── victory.wav
├── src/
├── icon.png
├── src/com/tankbattle/
│ ├── Main.java
│ ├── Game.java
│ ├── Tank.java
│ ├── Bullet.java
│ ├── Wall.java
│ ├── Explosion.java
│ ├── EnemyAI.java
│ ├── MapGenerator.java
│ ├── icon.png
│ ├── Menu.java
│ ├── PowerUp.java
│ ├── LevelData.java
│ ├── LevelState.java
│ ├── Boss.java
│ ├── SaveManager.java
│ ├── Settings.java
│ ├── SettingsMenu.java
│ ├── SoundManager.java
│ ├──
│ └── Utils.java
├── compile.bat
└── build.bat
C++/
├── assets/sounds/
├── explode.wav
├── gameover.wav
├── powerup.wav
├── shoot.wav
├── victory.wav
├── src/
│ ├── main.cpp
│ ├── Game.cpp/.h
│ ├── Tank.cpp/.h
│ ├── Bullet.cpp/.h
│ ├── Wall.cpp/.h
│ ├── Explosion.cpp/.h
│ ├── EnemyAI.cpp/.h
│ ├── MapGenerator.cpp/.h
│ ├── Menu.cpp/.h
│ ├── PowerUp.cpp/.h
│ ├── icon.ico
│ ├── LevelData.cpp/h
│ ├── LevelState.cpp/h
│ ├── Boss.cpp/h
│ ├── SaveManager.cpp/h
│ ├── SoundManager.cpp/.h
│ ├── SettingsMenu.cpp/.h
│ ├── ConfigManager.cpp/.h
│ └── Utils.h
├── build.sh
├── icon.png
└── CMakeLists.txt
```
