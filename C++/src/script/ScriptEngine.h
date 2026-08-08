// src/script/ScriptEngine.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

class Game;

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

/**
 * Lua 脚本引擎
 * 支持 Lua 脚本加载和执行
 */
class ScriptEngine {
public:
    static ScriptEngine& getInstance();

    // 禁止拷贝
    ScriptEngine(const ScriptEngine&) = delete;
    ScriptEngine& operator=(const ScriptEngine&) = delete;

    /**
     * 初始化脚本引擎
     */
    void init(Game* game);

    /**
     * 重新加载所有脚本
     */
    void reloadAll();

    /**
     * 调用 Lua 函数
     */
    void callFunction(const std::string& functionName, ...);

    void update(float dt) {
        callFunction("onUpdate", dt);
    }

    /**
     * 获取敌人速度倍数（从 Lua 脚本读取）
     */
    float getEnemySpeedMultiplier();

    /**
     * 获取脚本数量
     */
    int getScriptCount() const { return (int)loadedScripts.size(); }

    /**
     * 检查脚本引擎是否可用
     */
    bool isAvailable() const { return luaState != nullptr; }

private:
    ScriptEngine();
    ~ScriptEngine();

    void loadAllScripts();
    void loadScript(const std::string& filePath);
    void createExampleScript();

    lua_State* luaState;
    std::vector<std::string> loadedScripts;
    std::string scriptsDir;
    bool initialized;
};