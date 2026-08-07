package com.tankbattle.script;

import com.tankbattle.Game;

import javax.script.*;
import java.io.*;
import java.util.*;

public class ScriptEngine {

    private static final String SCRIPTS_DIR = "Scripts";
    private static ScriptEngineManager manager;
    private static javax.script.ScriptEngine luaEngine;
    private static List<ScriptContext> loadedScripts = new ArrayList<>();
    private static boolean scriptAvailable = false;

    public static void init(Game game) {
        manager = new ScriptEngineManager();
        luaEngine = manager.getEngineByName("lua");

        if (luaEngine == null) {
            luaEngine = manager.getEngineByName("luaj");
        }

        if (luaEngine == null) {
            System.err.println("未找到 Lua 引擎，脚本功能不可用");
            System.err.println("提示：将 luaj-jse-3.0.2.jar 放在 lib/ 目录");
            scriptAvailable = false;
            return;
        }

        scriptAvailable = true;

        File scriptDir = new File(SCRIPTS_DIR);
        if (!scriptDir.exists()) {
            scriptDir.mkdirs();
            System.out.println("创建 Scripts 目录");
            createExampleScript();
            return;
        }

        loadAllScripts();
    }

    private static void createExampleScript() {
        File example = new File(SCRIPTS_DIR, "example.lua");
        try (FileWriter fw = new FileWriter(example)) {
            fw.write("-- 示例Lua脚本\n");
            fw.write("function onGameStart()\n");
            fw.write("    print('Hello from Lua!')\n");
            fw.write("end\n");
            fw.write("function getEnemySpeed()\n");
            fw.write("    return 1.0\n");
            fw.write("end\n");
            System.out.println("创建示例脚本: Scripts/example.lua");
        } catch (IOException e) {
            System.err.println("创建示例脚本失败: " + e.getMessage());
        }
    }

    private static void loadAllScripts() {
        File scriptDir = new File(SCRIPTS_DIR);
        File[] files = scriptDir.listFiles((dir, name) -> name.endsWith(".lua"));

        if (files == null || files.length == 0) {
            System.out.println("未找到 Lua 脚本");
            return;
        }

        System.out.println("发现 " + files.length + " 个 Lua 脚本");

        for (File file : files) {
            loadScript(file);
        }

        callFunction("onGameStart");
    }

    private static void loadScript(File file) {
        try (FileReader reader = new FileReader(file)) {
            luaEngine.eval(reader);
            ScriptContext ctx = new ScriptContext();
            ctx.fileName = file.getName();
            ctx.loadTime = System.currentTimeMillis();
            loadedScripts.add(ctx);
            System.out.println("加载脚本: " + file.getName());
        } catch (ScriptException e) {
            System.err.println("脚本错误 " + file.getName() + ": " + e.getMessage());
        } catch (IOException e) {
            System.err.println("读取脚本失败 " + file.getName() + ": " + e.getMessage());
        }
    }

    public static void reloadAll() {
        if (!scriptAvailable) {
            System.err.println("Lua引擎不可用，无法重载脚本");
            return;
        }

        System.out.println("重新加载所有脚本...");
        loadedScripts.clear();

        luaEngine = manager.getEngineByName("lua");
        if (luaEngine == null) {
            luaEngine = manager.getEngineByName("luaj");
        }
        if (luaEngine == null) {
            System.err.println("未找到 Lua 引擎");
            scriptAvailable = false;
            return;
        }

        loadAllScripts();
    }

    public static void callFunction(String functionName, Object... args) {
        if (!scriptAvailable || luaEngine == null) return;

        try {
            StringBuilder call = new StringBuilder();
            call.append("return ").append(functionName).append("(");
            for (int i = 0; i < args.length; i++) {
                if (i > 0) call.append(", ");
                Object arg = args[i];
                if (arg == null) call.append("nil");
                else if (arg instanceof Number) call.append(arg);
                else if (arg instanceof Boolean) call.append(arg);
                else if (arg instanceof String) call.append("\"").append(arg).append("\"");
                else call.append("\"").append(arg.toString()).append("\"");
            }
            call.append(")");

            luaEngine.eval(call.toString());
        } catch (ScriptException e) {
        }
    }

    public static double getEnemySpeedMultiplier() {
        if (!scriptAvailable || luaEngine == null) return 1.0;

        try {
            Object result = luaEngine.eval("return getEnemySpeed()");
            if (result instanceof Number) {
                return ((Number) result).doubleValue();
            }
        } catch (ScriptException e) {
        }
        return 1.0;
    }

    public static boolean isAvailable() {
        return scriptAvailable;
    }

    public static int getScriptCount() {
        return loadedScripts.size();
    }

    private static class ScriptContext {
        String fileName;
        long loadTime;
    }
}