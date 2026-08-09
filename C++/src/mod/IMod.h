// src/mod/IMod.h
#pragma once
#include <string>

class Game;

class IMod {
public:
    virtual ~IMod() = default;

    virtual void onLoad(Game* game) = 0;
    virtual void onUnload() {}
    virtual void onUpdate(float dt) {}

    virtual std::string getModName() const { return "Unknown Mod"; }
    virtual std::string getModVersion() const { return "1.0.0"; }
    
    // 添加游戏版本要求
    virtual std::string getGameVersion() const { return "1.9.0"; }

    virtual std::string getModDescription() const { return ""; }
};

typedef IMod* (*CreateModFunc)();
typedef void (*DestroyModFunc)(IMod*);