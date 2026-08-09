cmake_minimum_required(VERSION 3.10)
project(TankBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories("/ucrt64/lib")
include_directories("/ucrt64/include")

# 源文件
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Tank.cpp
    src/Bullet.cpp
    src/Wall.cpp
    src/Explosion.cpp
    src/EnemyAI.cpp
    src/MapGenerator.cpp
    src/Menu.cpp
    src/PowerUp.cpp
    src/Boss.cpp
    src/LevelData.cpp
    src/LevelState.cpp
    src/SaveManager.cpp
    src/SoundManager.cpp
    src/SettingsMenu.cpp
    src/ConfigManager.cpp
    src/mod/ModLoader.cpp
    src/resource/TextureManager.cpp
    src/resource/ResourcePackLoader.cpp
    src/resource/MapLoader.cpp
    src/script/ScriptEngine.cpp
)

if(WIN32 AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/icon.rc)
    list(APPEND SOURCES src/icon.rc)
endif()

add_executable(TankBattle WIN32 ${SOURCES})

target_include_directories(TankBattle PRIVATE
    src
    src/mod
    src/resource
    src/script
    /ucrt64/include
)

target_link_libraries(TankBattle
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    lua
    zip
)

if(WIN32)
    target_link_libraries(TankBattle
        ws2_32
        ole32
        winmm
        gdi32
        opengl32
    )
endif()