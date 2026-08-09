// src/com/tankbattle/script/ScriptEngine.java
package com.tankbattle.script;

import com.tankbattle.Game;
import com.tankbattle.Tank;
import com.tankbattle.Bullet;
import com.tankbattle.PowerUp;
import com.tankbattle.Wall;
import com.tankbattle.Explosion;
import com.tankbattle.Utils;

import javax.script.*;
import java.io.*;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.*;

public class ScriptEngine {

    private static final String SCRIPTS_DIR = "Scripts";
    private static ScriptEngineManager manager;
    private static javax.script.ScriptEngine luaEngine;
    private static List<ScriptContext> loadedScripts = new ArrayList<>();
    private static boolean scriptAvailable = false;
    private static Game gameInstance;
    private static Timer timer;
    private static LuaAPI luaAPI;

    // ============================================================
    // 核心 API 类
    // ============================================================

    public static class LuaAPI {
        // ===== 游戏控制 =====
        
        public String getGameState() {
            if (gameInstance == null) return "null";
            try {
                Field mode = Game.class.getDeclaredField("gameMode");
                mode.setAccessible(true);
                Field level = Game.class.getDeclaredField("level");
                level.setAccessible(true);
                Field score = Game.class.getDeclaredField("score");
                score.setAccessible(true);
                return String.format("Mode:%s Level:%d Score:%d",
                    mode.get(gameInstance), level.getInt(gameInstance), score.getInt(gameInstance));
            } catch (Exception e) { return "Error"; }
        }

        public int getScore() {
            try {
                Field score = Game.class.getDeclaredField("score");
                score.setAccessible(true);
                return score.getInt(gameInstance);
            } catch (Exception e) { return 0; }
        }

        public int getLevel() {
            try {
                Field level = Game.class.getDeclaredField("level");
                level.setAccessible(true);
                return level.getInt(gameInstance);
            } catch (Exception e) { return 1; }
        }

        public int getEnemyCount() {
            try {
                Field enemies = Game.class.getDeclaredField("enemies");
                enemies.setAccessible(true);
                List<?> list = (List<?>) enemies.get(gameInstance);
                return list.size();
            } catch (Exception e) { return 0; }
        }

        public boolean isGameOver() {
            try {
                Field f = Game.class.getDeclaredField("gameOver");
                f.setAccessible(true);
                return f.getBoolean(gameInstance);
            } catch (Exception e) { return false; }
        }

        public boolean isPaused() {
            try {
                Field f = Game.class.getDeclaredField("paused");
                f.setAccessible(true);
                return f.getBoolean(gameInstance);
            } catch (Exception e) { return false; }
        }

        public void setPaused(boolean paused) {
            try {
                Field f = Game.class.getDeclaredField("paused");
                f.setAccessible(true);
                f.setBoolean(gameInstance, paused);
            } catch (Exception e) {}
        }

        public void togglePause() {
            try {
                Field f = Game.class.getDeclaredField("paused");
                f.setAccessible(true);
                f.setBoolean(gameInstance, !f.getBoolean(gameInstance));
            } catch (Exception e) {}
        }

        public void restartLevel() {
            try {
                Method m = Game.class.getDeclaredMethod("initLevel");
                m.setAccessible(true);
                m.invoke(gameInstance);
            } catch (Exception e) {}
        }

        public void setEnemyCount(int count) {
            try {
                Field f = Game.class.getDeclaredField("enemyCount");
                f.setAccessible(true);
                f.setInt(gameInstance, count);
            } catch (Exception e) {}
        }

        public void addScore(int points) {
            try {
                Field f = Game.class.getDeclaredField("score");
                f.setAccessible(true);
                f.setInt(gameInstance, f.getInt(gameInstance) + points);
            } catch (Exception e) {}
        }

        // ===== 玩家操作 =====

        public LuaTank getPlayer1() {
            try {
                Field f = Game.class.getDeclaredField("player1");
                f.setAccessible(true);
                Object p = f.get(gameInstance);
                return p != null ? new LuaTank(p) : null;
            } catch (Exception e) { return null; }
        }

        public LuaTank getPlayer2() {
            try {
                Field f = Game.class.getDeclaredField("player2");
                f.setAccessible(true);
                Object p = f.get(gameInstance);
                return p != null ? new LuaTank(p) : null;
            } catch (Exception e) { return null; }
        }

        public void healPlayer1(int amount) {
            LuaTank p = getPlayer1();
            if (p != null) {
                p.addLives(amount);
                System.out.println("[Lua] Healed P1 by " + amount + " (now " + p.getLives() + ")");
            }
        }

        public void healPlayer2(int amount) {
            LuaTank p = getPlayer2();
            if (p != null) {
                p.addLives(amount);
                System.out.println("[Lua] Healed P2 by " + amount + " (now " + p.getLives() + ")");
            }
        }

        public void healAllPlayers(int amount) {
            healPlayer1(amount);
            healPlayer2(amount);
        }

        public List<LuaTank> getEnemies() {
            List<LuaTank> result = new ArrayList<>();
            try {
                Field f = Game.class.getDeclaredField("enemies");
                f.setAccessible(true);
                List<?> enemies = (List<?>) f.get(gameInstance);
                for (Object e : enemies) {
                    result.add(new LuaTank(e));
                }
            } catch (Exception e) {}
            return result;
        }

        public List<LuaBullet> getAllBullets() {
            List<LuaBullet> result = new ArrayList<>();
            try {
                // 收集所有子弹
                LuaTank p1 = getPlayer1();
                LuaTank p2 = getPlayer2();
                if (p1 != null) result.addAll(p1.getBullets());
                if (p2 != null) result.addAll(p2.getBullets());
                for (LuaTank enemy : getEnemies()) {
                    result.addAll(enemy.getBullets());
                }
            } catch (Exception e) {}
            return result;
        }
    }

    // ============================================================
    // Lua 坦克对象
    // ============================================================

    public static class LuaTank {
        private Object tank;

        public LuaTank(Object tank) { this.tank = tank; }

        public double getX() {
            try { Field f = Tank.class.getDeclaredField("x"); f.setAccessible(true); return f.getDouble(tank);
            } catch (Exception e) { return 0; }
        }

        public double getY() {
            try { Field f = Tank.class.getDeclaredField("y"); f.setAccessible(true); return f.getDouble(tank);
            } catch (Exception e) { return 0; }
        }

        public void setPosition(double x, double y) {
            try { Field fx = Tank.class.getDeclaredField("x"); fx.setAccessible(true); fx.setDouble(tank, x);
                  Field fy = Tank.class.getDeclaredField("y"); fy.setAccessible(true); fy.setDouble(tank, y);
            } catch (Exception e) {}
        }

        public int getLives() {
            try { Field f = Tank.class.getDeclaredField("lives"); f.setAccessible(true); return f.getInt(tank);
            } catch (Exception e) { return 0; }
        }

        public void setLives(int lives) {
            try { Field f = Tank.class.getDeclaredField("lives"); f.setAccessible(true); f.setInt(tank, lives);
            } catch (Exception e) {}
        }

        public void addLives(int amount) {
            setLives(getLives() + amount);
        }

        public boolean isAlive() {
            try { Field f = Tank.class.getDeclaredField("alive"); f.setAccessible(true); return f.getBoolean(tank);
            } catch (Exception e) { return false; }
        }

        public void setAlive(boolean alive) {
            try { Field f = Tank.class.getDeclaredField("alive"); f.setAccessible(true); f.setBoolean(tank, alive);
            } catch (Exception e) {}
        }

        public void setSpeed(double speed) {
            try { Field f = Tank.class.getDeclaredField("speed"); f.setAccessible(true); f.setDouble(tank, speed);
            } catch (Exception e) {}
        }

        public boolean isBoss() {
            try { Field f = Tank.class.getDeclaredField("isBoss"); f.setAccessible(true); return f.getBoolean(tank);
            } catch (Exception e) { return false; }
        }

        public void addEffect(String type, float duration) {
            try { Field f = Tank.class.getDeclaredField("effects"); f.setAccessible(true);
                  Map<String, Float> map = (Map<String, Float>) f.get(tank);
                  map.put(type, duration);
            } catch (Exception e) {}
        }

        public void shoot() {
            try { Method m = Tank.class.getDeclaredMethod("shoot"); m.setAccessible(true); m.invoke(tank);
            } catch (Exception e) {}
        }

        public List<LuaBullet> getBullets() {
            List<LuaBullet> result = new ArrayList<>();
            try { Field f = Tank.class.getDeclaredField("bullets"); f.setAccessible(true);
                  List<?> bullets = (List<?>) f.get(tank);
                  for (Object b : bullets) {
                      result.add(new LuaBullet(b));
                  }
            } catch (Exception e) {}
            return result;
        }
    }

    // ============================================================
    // Lua 子弹对象
    // ============================================================

    public static class LuaBullet {
        private Object bullet;

        public LuaBullet(Object bullet) { this.bullet = bullet; }

        public double getX() {
            try { Field f = Bullet.class.getDeclaredField("x"); f.setAccessible(true); return f.getDouble(bullet);
            } catch (Exception e) { return 0; }
        }

        public double getY() {
            try { Field f = Bullet.class.getDeclaredField("y"); f.setAccessible(true); return f.getDouble(bullet);
            } catch (Exception e) { return 0; }
        }

        public boolean isAlive() {
            try { Field f = Bullet.class.getDeclaredField("alive"); f.setAccessible(true); return f.getBoolean(bullet);
            } catch (Exception e) { return false; }
        }

        public void setAlive(boolean alive) {
            try { Field f = Bullet.class.getDeclaredField("alive"); f.setAccessible(true); f.setBoolean(bullet, alive);
            } catch (Exception e) {}
        }
    }

    // ============================================================
    // 初始化
    // ============================================================

    public static void init(Game game) {
        gameInstance = game;
        System.out.println("[Lua] Initializing...");

        manager = new ScriptEngineManager();
        luaEngine = manager.getEngineByName("lua");
        if (luaEngine == null) {
            luaEngine = manager.getEngineByName("luaj");
        }
        if (luaEngine == null) {
            System.err.println("[Lua] Lua engine not found!");
            scriptAvailable = false;
            return;
        }
        scriptAvailable = true;
        System.out.println("[Lua] Engine found: " + luaEngine.getClass().getName());

        // 注册所有 API
        luaAPI = new LuaAPI();
        luaEngine.put("api", luaAPI);
        luaEngine.put("game", luaAPI);      // 别名
        luaEngine.put("heal", new HealFunction());
        luaEngine.put("print", new PrintFunction());

        // 加载脚本
        File scriptDir = new File(SCRIPTS_DIR);
        if (!scriptDir.exists()) {
            scriptDir.mkdirs();
            createExampleScript();
            return;
        }

        loadAllScripts();

        // 延迟调用 onGameStart
        new Timer(true).schedule(new TimerTask() {
            @Override
            public void run() {
                System.out.println("[Lua] Calling onGameStart...");
                callFunction("onGameStart");
                startAutoUpdate();
            }
        }, 200);
    }

    // ============================================================
    // 辅助函数
    // ============================================================

    public static class HealFunction implements javax.script.Invocable {
        public String call(int amount) {
            if (luaAPI == null) return "API not ready";
            luaAPI.healAllPlayers(amount);
            return "Healed all players by " + amount;
        }
        public Object invokeMethod(Object thiz, String name, Object... args) { return null; }
        public Object invokeFunction(String name, Object... args) { return null; }
        public <T> T getInterface(Class<T> clasz) { return null; }
        public <T> T getInterface(Object thiz, Class<T> clasz) { return null; }
    }

    public static class PrintFunction implements javax.script.Invocable {
        public void call(String msg) {
            System.out.println("[Lua] " + msg);
        }
        public Object invokeMethod(Object thiz, String name, Object... args) { return null; }
        public Object invokeFunction(String name, Object... args) { return null; }
        public <T> T getInterface(Class<T> clasz) { return null; }
        public <T> T getInterface(Object thiz, Class<T> clasz) { return null; }
    }

    private static void createExampleScript() {
        File example = new File(SCRIPTS_DIR, "example.lua");
        try (FileWriter fw = new FileWriter(example)) {
            fw.write("-- Example Lua Script\n");
            fw.write("function onGameStart()\n");
            fw.write("    print('Hello from Lua!')\n");
            fw.write("    print('Game state: ' .. game:getGameState())\n");
            fw.write("    heal(10)\n");
            fw.write("end\n");
            fw.write("function onUpdate(dt)\n");
            fw.write("    -- Auto heal when low HP\n");
            fw.write("    local p1 = game:getPlayer1()\n");
            fw.write("    if p1 and p1:getLives() < 3 then\n");
            fw.write("        game:healPlayer1(20)\n");
            fw.write("    end\n");
            fw.write("end\n");
            fw.write("function getEnemySpeed()\n");
            fw.write("    return 1.0\n");
            fw.write("end\n");
        } catch (IOException e) {}
    }

    private static void loadAllScripts() {
        File scriptDir = new File(SCRIPTS_DIR);
        File[] files = scriptDir.listFiles((dir, name) -> name.endsWith(".lua"));
        if (files == null || files.length == 0) {
            System.out.println("[Lua] No scripts found");
            return;
        }
        System.out.println("[Lua] Found " + files.length + " scripts");
        for (File file : files) {
            loadScript(file);
        }
    }

    private static void loadScript(File file) {
        try (FileReader reader = new FileReader(file)) {
            System.out.println("[Lua] Loading: " + file.getName());
            luaEngine.eval(reader);
            System.out.println("[Lua] Loaded: " + file.getName());
        } catch (ScriptException e) {
            System.err.println("[Lua] Script error: " + e.getMessage());
            e.printStackTrace();
        } catch (IOException e) {
            System.err.println("[Lua] IO error: " + e.getMessage());
        }
    }

    public static void callFunction(String functionName, Object... args) {
        if (!scriptAvailable || luaEngine == null) return;
        try {
            Object typeCheck = luaEngine.eval("return type(" + functionName + ")");
            if (!"function".equals(String.valueOf(typeCheck))) return;

            StringBuilder call = new StringBuilder();
            call.append("return ").append(functionName).append("(");
            for (int i = 0; i < args.length; i++) {
                if (i > 0) call.append(", ");
                Object arg = args[i];
                if (arg == null) call.append("nil");
                else if (arg instanceof Number || arg instanceof Boolean) call.append(arg);
                else if (arg instanceof String) call.append("\"").append(arg).append("\"");
                else call.append("nil");
            }
            call.append(")");
            luaEngine.eval(call.toString());
        } catch (ScriptException e) {}
    }

    private static void startAutoUpdate() {
        if (timer != null) return;
        System.out.println("[Lua] Starting update timer...");
        timer = new Timer(true);
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                try { callFunction("onUpdate", 0.016f); } catch (Exception e) {}
            }
        }, 1000, 100);
    }

    public static void reloadAll() {
        if (!scriptAvailable) return;
        System.out.println("[Lua] Reloading...");
        loadedScripts.clear();
        luaEngine = manager.getEngineByName("lua");
        if (luaEngine == null) { luaEngine = manager.getEngineByName("luaj"); }
        if (luaEngine == null) { scriptAvailable = false; return; }
        loadAllScripts();
        callFunction("onGameStart");
    }

    public static double getEnemySpeedMultiplier() {
        Object result = callFunctionWithReturn("getEnemySpeed");
        return (result instanceof Number) ? ((Number) result).doubleValue() : 1.0;
    }

    public static Object callFunctionWithReturn(String functionName, Object... args) {
        if (!scriptAvailable || luaEngine == null) return null;
        try {
            Object typeCheck = luaEngine.eval("return type(" + functionName + ")");
            if (!"function".equals(String.valueOf(typeCheck))) return null;
            StringBuilder call = new StringBuilder();
            call.append("return ").append(functionName).append("(");
            for (int i = 0; i < args.length; i++) {
                if (i > 0) call.append(", ");
                Object arg = args[i];
                if (arg == null) call.append("nil");
                else if (arg instanceof Number || arg instanceof Boolean) call.append(arg);
                else if (arg instanceof String) call.append("\"").append(arg).append("\"");
                else call.append("nil");
            }
            call.append(")");
            return luaEngine.eval(call.toString());
        } catch (ScriptException e) { return null; }
    }

    public static boolean isAvailable() { return scriptAvailable; }
    public static int getScriptCount() { return loadedScripts.size(); }

    private static class ScriptContext {
        String fileName;
        long loadTime;
    }
}