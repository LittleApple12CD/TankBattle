# 坦克大战

- **Tank Battle** 是一款经典的双人对战坦克游戏，支持 **PVE 合作模式** 和 **PVP 对战模式** 以及 **单人模式** 和 **关卡** 模式。本项目用 **Python、Java、C++** 三种语言实现（Python版已停止更新），支持模组、资源包和脚本加载，Java版使用OpenGL图形API，C++版基于SFML3.0，Java版基于Swing，Python版基于Pygame
- **Tank Battle** is a classic two-player tank game that supports PVE co-op mode, PVP battle mode, as well as single-player mode and level mode.Supports loading mods, resource packs, and scripts. This project is implemented in Python, Java, and C++, using the OpenGL graphics API. The C version is based on SFML 3.0, the Java version is based on Swing, and the Python version is based on Pygame.

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
├── assets/
│   └── sounds/
│       ├── explode.wav
│       ├── gameover.wav
│       ├── powerup.wav
│       ├── shoot.wav
│       └── victory.wav
│       fonts/
│       ├── arial.ttf
│       ├── consola.ttf
│       ├── msyh.ttc
│       └── simhei.ttf
├── src/
│   └── com/
│       └── tankbattle/
│           ├── Boss.java
│           ├── Bullet.java
│           ├── EnemyAI.java
│           ├── Explosion.java
│           ├── Game.java
│           ├── LevelData.java
│           ├── LevelState.java
│           ├── Main.java
│           ├── MapGenerator.java
│           ├── Menu.java
│           ├── PowerUp.java
│           ├── SaveManager.java
│           ├── Settings.java
│           ├── SettingsMenu.java
│           ├── SoundManager.java
│           ├── Renderer.java
│           ├── Tank.java
│           ├── Utils.java
│           ├── Wall.java
│           ├── mod/
│           │   ├── IMod.java
│           │   ├── ModInfo.java
│           │   └── ModLoader.java
│           ├── resource/
│           │   ├── MapLoader.java
│           │   ├── ResourcePackLoader.java
│           │   └── TextureManager.java
│           └── script/
│               └── ScriptEngine.java
├── build.bat
├── compile.bat
└── icon.png

C++/
├── assets/
│   └── sounds/
│       ├── explode.wav
│       ├── gameover.wav
│       ├── powerup.wav
│       ├── shoot.wav
│       └── victory.wav
│       fonts/
│       ├── arial.ttf
│       ├── consola.ttf
│       ├── msyh.ttc
│       └── simhei.ttf
├── src/
│   ├── Boss.cpp
│   ├── Boss.h
│   ├── Bullet.cpp
│   ├── Bullet.h
│   ├── ConfigManager.cpp
│   ├── ConfigManager.h
│   ├── EnemyAI.cpp
│   ├── EnemyAI.h
│   ├── Explosion.cpp
│   ├── Explosion.h
│   ├── Game.cpp
│   ├── Game.h
│   ├── LevelData.cpp
│   ├── LevelData.h
│   ├── LevelState.cpp
│   ├── LevelState.h
│   ├── main.cpp
│   ├── MapGenerator.cpp
│   ├── MapGenerator.h
│   ├── Menu.cpp
│   ├── Menu.h
│   ├── PowerUp.cpp
│   ├── PowerUp.h
│   ├── SaveManager.cpp
│   ├── SaveManager.h
│   ├── SettingsMenu.cpp
│   ├── SettingsMenu.h
│   ├── SoundManager.cpp
│   ├── SoundManager.h
│   ├── Tank.cpp
│   ├── Tank.h
│   ├── Utils.h
│   ├── Wall.cpp
│   ├── Wall.h
│   ├── icon.rc
│   ├── icon.ico
│   ├── mod/
│   │   ├── IMod.h
│   │   ├── ModInfo.h
│   │   └── ModLoader.cpp
│   │   └── ModLoader.h
│   ├── renderer/
│   │   ├── Renderer.h
│   │   ├── Renderer.cpp
│   ├── resource/
│   │   ├── MapLoader.cpp
│   │   ├── MapLoader.h
│   │   ├── ResourcePackLoader.cpp
│   │   ├── ResourcePackLoader.h
│   │   ├── TextureManager.cpp
│   │   └── TextureManager.h
│   └── script/
│       ├── ScriptEngine.cpp
│       └── ScriptEngine.h
├── build.sh
├── CMakeLists.txt
└── icon.png
```
<img width="2001" height="1163" alt="image" src="https://github.com/user-attachments/assets/96f1ed49-6709-4c68-91d2-751ab067ca8b" />
<img width="1602" height="939" alt="image" src="https://github.com/user-attachments/assets/d1276cb2-096f-42e5-a83f-b4f30a266dca" />
<img width="2001" height="1163" alt="image" src="https://github.com/user-attachments/assets/5afc1c93-4fa2-4b08-85b5-5edbc3820dca" />
<img width="2001" height="1163" alt="image" src="https://github.com/user-attachments/assets/642d4f15-22bf-4779-a342-470a822acf55" />


