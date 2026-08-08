// src/script/ScriptEngine.cpp
#include "ScriptEngine.h"
#include "../Game.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

ScriptEngine& ScriptEngine::getInstance() {
    static ScriptEngine instance;
    return instance;
}

ScriptEngine::ScriptEngine()
    : luaState(nullptr), scriptsDir("Scripts"), initialized(false) {}

ScriptEngine::~ScriptEngine() {
    if (luaState) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

void ScriptEngine::init(Game* game) {
    if (initialized) return;

    // 创建 Lua 状态
    luaState = luaL_newstate();
    if (!luaState) {
        std::cerr << "创建 Lua 状态失败" << std::endl;
        return;
    }

    luaL_openlibs(luaState);

    // 注册游戏 API
    // TODO: 注册 C++ 函数供 Lua 调用

    // 创建脚本目录
    if (!fs::exists(scriptsDir)) {
        fs::create_directories(scriptsDir);
        createExampleScript();
        std::cout << "创建 Scripts 目录" << std::endl;
    }

    loadAllScripts();
    initialized = true;

    // 调用 onGameStart
    callFunction("onGameStart");

    std::cout << "Lua 脚本引擎初始化完成，加载了 " << loadedScripts.size() << " 个脚本" << std::endl;
}

void ScriptEngine::loadAllScripts() {
    for (const auto& entry : fs::directory_iterator(scriptsDir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".lua") continue;

        loadScript(entry.path().string());
    }
}

void ScriptEngine::loadScript(const std::string& filePath) {
    if (!luaState) return;

    if (luaL_dofile(luaState, filePath.c_str()) != LUA_OK) {
        std::cerr << "脚本错误 " << fs::path(filePath).filename().string()
                  << ": " << lua_tostring(luaState, -1) << std::endl;
        lua_pop(luaState, 1);
        return;
    }

    loadedScripts.push_back(filePath);
    std::cout << "加载脚本: " << fs::path(filePath).filename().string() << std::endl;
}

void ScriptEngine::reloadAll() {
    if (!luaState) return;

    loadedScripts.clear();
    lua_close(luaState);
    luaState = luaL_newstate();
    luaL_openlibs(luaState);

    loadAllScripts();
    callFunction("onGameStart");

    std::cout << "重新加载所有脚本完成" << std::endl;
}

void ScriptEngine::callFunction(const std::string& functionName, ...) {
    if (!luaState) return;

    lua_getglobal(luaState, functionName.c_str());
    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        return;
    }

    // 处理参数
    va_list args;
    va_start(args, functionName);
    int argCount = 0;

    // 简单参数处理：支持数字和字符串
    // 实际使用需要更复杂的实现
    while (true) {
        // 这里简化处理，不解析 va_list
        // 实际项目可以使用模板或反射
        break;
    }
    va_end(args);

    if (lua_pcall(luaState, argCount, 0, 0) != LUA_OK) {
        std::cerr << "调用函数 " << functionName << " 失败: "
                  << lua_tostring(luaState, -1) << std::endl;
        lua_pop(luaState, 1);
    }
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

    float result = 1.0f;
    if (lua_isnumber(luaState, -1)) {
        result = (float)lua_tonumber(luaState, -1);
    }
    lua_pop(luaState, 1);

    return result;
}

void ScriptEngine::createExampleScript() {
    std::string path = scriptsDir + "/example.lua";
    std::ofstream file(path);
    if (!file.is_open()) return;

    file << "-- 示例Lua脚本\n";
    file << "function onGameStart()\n";
    file << "    print('Hello from Lua!')\n";
    file << "end\n";
    file << "function getEnemySpeed()\n";
    file << "    return 1.0\n";
    file << "end\n";
    file << "function onEnemySpawn(enemy)\n";
    file << "    -- 可以修改敌人属性\n";
    file << "    return enemy\n";
    file << "end\n";
    file.close();

    std::cout << "创建示例脚本: " << path << std::endl;
}