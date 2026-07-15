cmake_minimum_required(VERSION 3.10)
project(TankBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加资源文件
set(RESOURCES src/icon.rc)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Tank.cpp
    src/Bullet.cpp
    src/Wall.cpp
    src/Explosion.cpp
    src/EnemyAI.cpp
    src/MapGenerator.cpp
    ${RESOURCES}
)

add_executable(TankBattle WIN32 ${SOURCES})

target_link_libraries(TankBattle
    sfml-graphics
    sfml-window
    sfml-system
)

target_include_directories(TankBattle PRIVATE src)