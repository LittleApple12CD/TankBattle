// src/script/ScriptEngine.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class Game;
class Tank;

class ScriptEngine {
public:
    static ScriptEngine& getInstance();

    ScriptEngine(const ScriptEngine&) = delete;
    ScriptEngine& operator=(const ScriptEngine&) = delete;

    void init(Game* game);
    void reloadAll();
    void update(float dt);
    void callFunction(const std::string& name, ...);
    
    float getEnemySpeedMultiplier();
    bool isAvailable() const { return luaState != nullptr; }
    int getScriptCount() const { return (int)loadedScripts.size(); }

private:
    ScriptEngine();
    ~ScriptEngine();

    void loadAllScripts();
    void loadScript(const std::string& path);
    void createExampleScript();
    void registerAllFunctions();
    
    // 游戏函数声明
    static int lua_getPlayer1(lua_State* L);
    static int lua_getPlayer2(lua_State* L);
    static int lua_getEnemies(lua_State* L);
    static int lua_getScore(lua_State* L);
    static int lua_setScore(lua_State* L);
    static int lua_getLevel(lua_State* L);
    static int lua_getEnemyCount(lua_State* L);
    static int lua_isGameOver(lua_State* L);
    static int lua_isPaused(lua_State* L);
    static int lua_togglePause(lua_State* L);
    static int lua_restartLevel(lua_State* L);
    static int lua_setEnemyCount(lua_State* L);
    static int lua_addScore(lua_State* L);
    
    // 作弊函数声明
    static int lua_healPlayer1(lua_State* L);
    static int lua_healPlayer2(lua_State* L);
    static int lua_clearEnemies(lua_State* L);
    static int lua_godMode(lua_State* L);
    
    // Tank 函数声明
    static int lua_tank_getX(lua_State* L);
    static int lua_tank_getY(lua_State* L);
    static int lua_tank_setPosition(lua_State* L);
    static int lua_tank_getLives(lua_State* L);
    static int lua_tank_setLives(lua_State* L);
    static int lua_tank_addLives(lua_State* L);
    static int lua_tank_isAlive(lua_State* L);
    static int lua_tank_setAlive(lua_State* L);
    static int lua_tank_setSpeed(lua_State* L);
    static int lua_tank_isBoss(lua_State* L);
    static int lua_tank_addEffect(lua_State* L);
    static int lua_tank_shoot(lua_State* L);
    static int lua_tank_getBullets(lua_State* L);
    static int lua_tank_getEffect(lua_State* L);

    lua_State* luaState;
    std::vector<std::string> loadedScripts;
    std::string scriptsDir;
    bool initialized;
    static Game* gameInstance;
};