// src/script/ScriptEngine.cpp
#include "ScriptEngine.h"
#include "../Game.h"
#include "../Tank.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Game* ScriptEngine::gameInstance = nullptr;

ScriptEngine& ScriptEngine::getInstance() {
    static ScriptEngine instance;
    return instance;
}

ScriptEngine::ScriptEngine()
    : luaState(nullptr), scriptsDir("Scripts"), initialized(false) {}

ScriptEngine::~ScriptEngine() {
    if (luaState) {
        lua_close(luaState);
    }
}

// ============================================================
// 初始化
// ============================================================

void ScriptEngine::init(Game* game) {
    if (initialized) return;
    
    gameInstance = game;
    
    luaState = luaL_newstate();
    if (!luaState) {
        std::cerr << "[Lua] 创建状态失败" << std::endl;
        return;
    }
    
    luaL_openlibs(luaState);
    registerAllFunctions();
    
    if (!fs::exists(scriptsDir)) {
        fs::create_directories(scriptsDir);
        createExampleScript();
    }
    
    loadAllScripts();
    initialized = true;
    
    callFunction("onGameStart");
    std::cout << "[Lua] 脚本引擎初始化完成，加载了 " << loadedScripts.size() << " 个脚本" << std::endl;
}

// ============================================================
// 注册所有 Lua C 函数
// ============================================================

void ScriptEngine::registerAllFunctions() {
    if (!luaState) return;
    
    // ===== 游戏函数 =====
    lua_register(luaState, "getPlayer1", lua_getPlayer1);
    lua_register(luaState, "getPlayer2", lua_getPlayer2);
    lua_register(luaState, "getEnemies", lua_getEnemies);
    lua_register(luaState, "getScore", lua_getScore);
    lua_register(luaState, "setScore", lua_setScore);
    lua_register(luaState, "getLevel", lua_getLevel);
    lua_register(luaState, "getEnemyCount", lua_getEnemyCount);
    lua_register(luaState, "isGameOver", lua_isGameOver);
    lua_register(luaState, "isPaused", lua_isPaused);
    lua_register(luaState, "togglePause", lua_togglePause);
    lua_register(luaState, "restartLevel", lua_restartLevel);
    lua_register(luaState, "setEnemyCount", lua_setEnemyCount);
    lua_register(luaState, "addScore", lua_addScore);
    
    // 作弊函数
    lua_register(luaState, "healPlayer1", lua_healPlayer1);
    lua_register(luaState, "healPlayer2", lua_healPlayer2);
    lua_register(luaState, "clearEnemies", lua_clearEnemies);
    lua_register(luaState, "godMode", lua_godMode);
    
    // ===== Tank 元表 =====
    luaL_newmetatable(luaState, "Tank");
    lua_pushstring(luaState, "__index");
    lua_pushvalue(luaState, -2);
    lua_settable(luaState, -3);
    
    lua_pushcfunction(luaState, lua_tank_getX);
    lua_setfield(luaState, -2, "getX");
    lua_pushcfunction(luaState, lua_tank_getY);
    lua_setfield(luaState, -2, "getY");
    lua_pushcfunction(luaState, lua_tank_setPosition);
    lua_setfield(luaState, -2, "setPosition");
    lua_pushcfunction(luaState, lua_tank_getLives);
    lua_setfield(luaState, -2, "getLives");
    lua_pushcfunction(luaState, lua_tank_setLives);
    lua_setfield(luaState, -2, "setLives");
    lua_pushcfunction(luaState, lua_tank_addLives);
    lua_setfield(luaState, -2, "addLives");
    lua_pushcfunction(luaState, lua_tank_isAlive);
    lua_setfield(luaState, -2, "isAlive");
    lua_pushcfunction(luaState, lua_tank_setAlive);
    lua_setfield(luaState, -2, "setAlive");
    lua_pushcfunction(luaState, lua_tank_setSpeed);
    lua_setfield(luaState, -2, "setSpeed");
    lua_pushcfunction(luaState, lua_tank_isBoss);
    lua_setfield(luaState, -2, "isBoss");
    lua_pushcfunction(luaState, lua_tank_addEffect);
    lua_setfield(luaState, -2, "addEffect");
    lua_pushcfunction(luaState, lua_tank_shoot);
    lua_setfield(luaState, -2, "shoot");
    lua_pushcfunction(luaState, lua_tank_getBullets);
    lua_setfield(luaState, -2, "getBullets");
    lua_pushcfunction(luaState, lua_tank_getEffect);
    lua_setfield(luaState, -2, "getEffect");
    
    lua_pop(luaState, 1);
    
    std::cout << "[Lua] 所有 API 已注册" << std::endl;
}

// ============================================================
// 游戏函数实现
// ============================================================

int ScriptEngine::lua_getPlayer1(lua_State* L) {
    if (!gameInstance) { lua_pushnil(L); return 1; }
    Tank* p1 = gameInstance->getPlayer1();
    if (p1) {
        lua_pushlightuserdata(L, p1);
        luaL_getmetatable(L, "Tank");
        lua_setmetatable(L, -2);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int ScriptEngine::lua_getPlayer2(lua_State* L) {
    if (!gameInstance) { lua_pushnil(L); return 1; }
    Tank* p2 = gameInstance->getPlayer2();
    if (p2) {
        lua_pushlightuserdata(L, p2);
        luaL_getmetatable(L, "Tank");
        lua_setmetatable(L, -2);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int ScriptEngine::lua_getEnemies(lua_State* L) {
    if (!gameInstance) { lua_pushnil(L); return 1; }
    auto& enemies = gameInstance->getEnemies();
    lua_newtable(L);
    int index = 1;
    for (Tank* enemy : enemies) {
        lua_pushnumber(L, index);
        lua_pushlightuserdata(L, enemy);
        luaL_getmetatable(L, "Tank");
        lua_setmetatable(L, -2);
        lua_settable(L, -3);
        index++;
    }
    return 1;
}

int ScriptEngine::lua_getScore(lua_State* L) {
    if (!gameInstance) { lua_pushnumber(L, 0); return 1; }
    lua_pushnumber(L, gameInstance->getScore());
    return 1;
}

int ScriptEngine::lua_setScore(lua_State* L) {
    if (!gameInstance) return 0;
    int score = (int)lua_tonumber(L, 1);
    gameInstance->setScore(score);
    return 0;
}

int ScriptEngine::lua_getLevel(lua_State* L) {
    if (!gameInstance) { lua_pushnumber(L, 1); return 1; }
    lua_pushnumber(L, gameInstance->getCurrentLevel());
    return 1;
}

int ScriptEngine::lua_getEnemyCount(lua_State* L) {
    if (!gameInstance) { lua_pushnumber(L, 0); return 1; }
    lua_pushnumber(L, (int)gameInstance->getEnemies().size());
    return 1;
}

int ScriptEngine::lua_isGameOver(lua_State* L) {
    if (!gameInstance) { lua_pushboolean(L, 0); return 1; }
    lua_pushboolean(L, gameInstance->isGameOver());
    return 1;
}

int ScriptEngine::lua_isPaused(lua_State* L) {
    if (!gameInstance) { lua_pushboolean(L, 0); return 1; }
    lua_pushboolean(L, gameInstance->isPaused());
    return 1;
}

int ScriptEngine::lua_togglePause(lua_State* L) {
    if (!gameInstance) return 0;
    gameInstance->setPaused(!gameInstance->isPaused());
    return 0;
}

int ScriptEngine::lua_restartLevel(lua_State* L) {
    if (!gameInstance) return 0;
    gameInstance->initLevel();
    return 0;
}

int ScriptEngine::lua_setEnemyCount(lua_State* L) {
    if (!gameInstance) return 0;
    int count = (int)lua_tonumber(L, 1);
    gameInstance->setEnemyCount(count);
    return 0;
}

int ScriptEngine::lua_addScore(lua_State* L) {
    if (!gameInstance) return 0;
    int points = (int)lua_tonumber(L, 1);
    gameInstance->addScore(points);
    return 0;
}

// ============================================================
// 作弊函数实现
// ============================================================

int ScriptEngine::lua_healPlayer1(lua_State* L) {
    if (!gameInstance) return 0;
    int amount = (int)lua_tonumber(L, 1);
    Tank* p1 = gameInstance->getPlayer1();
    if (p1) {
        p1->setLives(p1->getLives() + amount);
    }
    return 0;
}

int ScriptEngine::lua_healPlayer2(lua_State* L) {
    if (!gameInstance) return 0;
    int amount = (int)lua_tonumber(L, 1);
    Tank* p2 = gameInstance->getPlayer2();
    if (p2) {
        p2->setLives(p2->getLives() + amount);
    }
    return 0;
}

int ScriptEngine::lua_clearEnemies(lua_State* L) {
    if (!gameInstance) return 0;
    for (Tank* enemy : gameInstance->getEnemies()) {
        enemy->setAlive(false);
    }
    return 0;
}

int ScriptEngine::lua_godMode(lua_State* L) {
    if (!gameInstance) return 0;
    Tank* p1 = gameInstance->getPlayer1();
    if (p1) {
        p1->setLives(999);
    }
    return 0;
}

// ============================================================
// Tank 方法实现
// ============================================================

static Tank* checkTank(lua_State* L, int index) {
    return (Tank*)lua_touserdata(L, index);
}

int ScriptEngine::lua_tank_getX(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) lua_pushnumber(L, tank->getX());
    else lua_pushnil(L);
    return 1;
}

int ScriptEngine::lua_tank_getY(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) lua_pushnumber(L, tank->getY());
    else lua_pushnil(L);
    return 1;
}

int ScriptEngine::lua_tank_setPosition(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        float x = (float)lua_tonumber(L, 2);
        float y = (float)lua_tonumber(L, 3);
        tank->setPosition(x, y);
    }
    return 0;
}

int ScriptEngine::lua_tank_getLives(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) lua_pushnumber(L, tank->getLives());
    else lua_pushnil(L);
    return 1;
}

int ScriptEngine::lua_tank_setLives(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        int lives = (int)lua_tonumber(L, 2);
        tank->setLives(lives);
    }
    return 0;
}

int ScriptEngine::lua_tank_addLives(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        int amount = (int)lua_tonumber(L, 2);
        tank->setLives(tank->getLives() + amount);
    }
    return 0;
}

int ScriptEngine::lua_tank_isAlive(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) lua_pushboolean(L, tank->isAlive());
    else lua_pushnil(L);
    return 1;
}

int ScriptEngine::lua_tank_setAlive(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        bool alive = lua_toboolean(L, 2);
        tank->setAlive(alive);
    }
    return 0;
}

int ScriptEngine::lua_tank_setSpeed(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        float speed = (float)lua_tonumber(L, 2);
        tank->setSpeed(speed);
    }
    return 0;
}

int ScriptEngine::lua_tank_isBoss(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) lua_pushboolean(L, tank->isBoss);
    else lua_pushnil(L);
    return 1;
}

int ScriptEngine::lua_tank_addEffect(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        const char* type = lua_tostring(L, 2);
        float duration = (float)lua_tonumber(L, 3);
        tank->addEffect(type, duration);
    }
    return 0;
}

int ScriptEngine::lua_tank_shoot(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        tank->shoot();
    }
    return 0;
}

int ScriptEngine::lua_tank_getBullets(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (!tank) { lua_pushnil(L); return 1; }
    auto& bullets = tank->getBullets();
    lua_newtable(L);
    int index = 1;
    for (auto& b : bullets) {
        lua_pushnumber(L, index);
        lua_pushlightuserdata(L, &b);
        lua_settable(L, -3);
        index++;
    }
    return 1;
}

int ScriptEngine::lua_tank_getEffect(lua_State* L) {
    Tank* tank = checkTank(L, 1);
    if (tank) {
        const char* type = lua_tostring(L, 2);
        auto it = tank->effects.find(type);
        if (it != tank->effects.end()) {
            lua_pushnumber(L, it->second);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

// ============================================================
// 其他功能
// ============================================================

void ScriptEngine::loadAllScripts() {
    if (!fs::exists(scriptsDir)) {
        fs::create_directories(scriptsDir);
        createExampleScript();
        return;
    }
    for (const auto& entry : fs::recursive_directory_iterator(scriptsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            loadScript(entry.path().string());
        }
    }
}

void ScriptEngine::loadScript(const std::string& path) {
    if (!luaState) return;
    if (luaL_dofile(luaState, path.c_str()) != LUA_OK) {
        std::cerr << "[Lua] 脚本错误: " << lua_tostring(luaState, -1) << std::endl;
        lua_pop(luaState, 1);
        return;
    }
    loadedScripts.push_back(path);
    std::cout << "[Lua] 加载脚本: " << fs::path(path).filename().string() << std::endl;
}

void ScriptEngine::createExampleScript() {
    std::string path = scriptsDir + "/example.lua";
    std::ofstream file(path);
    if (file.is_open()) {
        file << "-- 示例 Lua 脚本\n";
        file << "function onGameStart()\n";
        file << "    print('[Lua] Hello!')\n";
        file << "end\n";
        file << "function getEnemySpeed()\n";
        file << "    return 1.0\n";
        file << "end\n";
        file.close();
    }
}

void ScriptEngine::callFunction(const std::string& name, ...) {
    if (!luaState) return;
    lua_getglobal(luaState, name.c_str());
    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        return;
    }
    if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
        std::cerr << "[Lua] 调用函数 " << name << " 失败: " << lua_tostring(luaState, -1) << std::endl;
        lua_pop(luaState, 1);
    }
}

void ScriptEngine::update(float dt) {
    callFunction("onUpdate", dt);
}

void ScriptEngine::reloadAll() {
    if (!luaState) return;
    loadedScripts.clear();
    lua_close(luaState);
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
    registerAllFunctions();
    loadAllScripts();
    callFunction("onGameStart");
    std::cout << "[Lua] 重新加载完成" << std::endl;
}

float ScriptEngine::getEnemySpeedMultiplier() {
    if (!luaState) return 1.0f;
    lua_getglobal(luaState, "getEnemySpeed");
    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        return 1.0f;
    }
    if (lua_pcall(luaState, 0, 1, 0) != LUA_OK) {
        lua_pop(luaState, 1);
        return 1.0f;
    }
    float result = (float)lua_tonumber(luaState, -1);
    lua_pop(luaState, 1);
    return result;
}
