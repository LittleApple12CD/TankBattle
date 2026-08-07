package com.tankbattle.mod;

import com.tankbattle.Game;

/**
 * 模组接口 - 所有JAR模组必须实现此接口
 */
public interface IMod {
    
    /**
     * 模组加载时调用
     * @param game 游戏实例，用于注册事件或修改游戏状态
     */
    void onLoad(Game game);
    
    /**
     * 模组卸载时调用（如果支持热卸载）
     */
    default void onUnload() {}
    
    /**
     * 每帧更新（可选）
     */
    default void onUpdate(double dt) {}
    
    /**
     * 获取模组名称
     */
    default String getModName() {
        return "Unknown Mod";
    }
    
    /**
     * 获取模组版本
     */
    default String getModVersion() {
        return "1.0.0";
    }
}