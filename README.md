# 🎮 坦克大战

**Tank Battle** 是一款经典的双人对战坦克游戏，支持 **PVE 合作模式** 和 **PVP 对战模式**。本项目用 **Python、Java、C++** 三种语言实现

## 🎯 游戏模式

- **PVE 合作模式** 🤝：P1 + P2 联手对抗红色 AI 坦克，消灭敌人获得分数
- **PVP 对战模式** ⚔️：P1 与 P2 互相射击，先消灭对方者获胜
- **5 种随机地图** 🗺️：每次按 `R` 键随机切换地图布局

---

## 🎮 控制方式

| 按键 | 功能 |
|------|------|
| **↑/↓/←/→** | P1 移动 |
| **空格键** | P1 射击 |
| **W/A/S/D** | P2 移动 |
| **J 键** | P2 射击 |
| **G 键** | 切换 PVP/PVE 模式 |
| **P 键** | 暂停/继续 |
| **R 键** | 重新开始（随机换地图） |
| **ESC 键** | 退出游戏 |

---

## 编译和运行
运行自带的编译脚本（.sh脚本需用MSYS2 UCRT64运行）

## 📁 项目结构
TankBattle/
├── Python/ # Python 版本（Pygame）
│ ├── main.py
│ ├── game.py
│ ├── entities.py
│ ├── ai.py
│ ├── config.py
│ └── package.bat
│
├── Java/ # Java 版本（Swing）
│ ├── src/com/tankbattle/
│ │ ├── Main.java
│ │ ├── Game.java
│ │ ├── Tank.java
│ │ ├── Bullet.java
│ │ ├── Wall.java
│ │ ├── Explosion.java
│ │ ├── EnemyAI.java
│ │ ├── MapGenerator.java
│ │ └── Utils.java
│ ├── compile.bat
│ └── build.bat
│
└── C++/ # C++ 版本（SFML 3.0）
├── src/
│ ├── main.cpp
│ ├── Game.cpp/.h
│ ├── Tank.cpp/.h
│ ├── Bullet.cpp/.h
│ ├── Wall.cpp/.h
│ ├── Explosion.cpp/.h
│ ├── EnemyAI.cpp/.h
│ ├── MapGenerator.cpp/.h
│ └── Utils.h
├── build.sh
└── CMakeLists.txt
