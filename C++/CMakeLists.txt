cmake_minimum_required(VERSION 3.10)
project(TankBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 硬编码 UCRT64 路径
set(SFML_INCLUDE_DIR "/ucrt64/include")
set(SFML_LIBRARY_DIR "/ucrt64/lib")

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Tank.cpp
    src/Bullet.cpp
    src/Wall.cpp
    src/Explosion.cpp
    src/EnemyAI.cpp
    src/MapGenerator.cpp
)

add_executable(TankBattle WIN32 ${SOURCES})

target_include_directories(TankBattle PRIVATE ${SFML_INCLUDE_DIR})

# 使用 find_library 查找库文件
find_library(SFML_GRAPHICS_LIB NAMES sfml-graphics libsfml-graphics PATHS ${SFML_LIBRARY_DIR})
find_library(SFML_WINDOW_LIB NAMES sfml-window libsfml-window PATHS ${SFML_LIBRARY_DIR})
find_library(SFML_SYSTEM_LIB NAMES sfml-system libsfml-system PATHS ${SFML_LIBRARY_DIR})

target_link_libraries(TankBattle
    ${SFML_GRAPHICS_LIB}
    ${SFML_WINDOW_LIB}
    ${SFML_SYSTEM_LIB}
)