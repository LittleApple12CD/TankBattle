package com.tankbattle.mod;

import com.tankbattle.Game;
import java.io.*;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.*;
import java.util.jar.JarFile;

public class ModLoader {

    private static final String MODS_DIR = "mods";
    private static final String MOD_LIST_FILE = "mod_list.txt";
    private static final String GAME_VERSION_FILE = "game_version.txt";
    private static String gameVersion = "1.0.0";

    private static List<ModInfo> mods = new ArrayList<>();
    private static List<IMod> loadedMods = new ArrayList<>();
    private static List<URLClassLoader> classLoaders = new ArrayList<>();

    public static void init() {
        loadGameVersion();

        File modDir = new File(MODS_DIR);
        if (!modDir.exists()) {
            modDir.mkdirs();
            System.out.println("创建 mods 目录");
            createModListFile();
            return;
        }

        createModListFile();
        scanMods();
    }

    private static void createModListFile() {
        File listFile = new File(MODS_DIR, MOD_LIST_FILE);
        if (!listFile.exists()) {
            try (FileWriter fw = new FileWriter(listFile)) {
                fw.write("# Mod list - one mod JAR file per line\n");
                fw.write("# Add # before filename to disable\n");
                fw.write("# Example:\n");
                fw.write("# PlaceholderMod.jar\n");
                fw.write("# #DisabledMod.jar\n");
                fw.write("\n");
                System.out.println("创建模组列表文件: " + MODS_DIR + "/" + MOD_LIST_FILE);
            } catch (IOException e) {
                System.err.println("创建模组列表文件失败: " + e.getMessage());
            }
        }
    }

    private static void loadGameVersion() {
        File versionFile = new File(GAME_VERSION_FILE);
        if (versionFile.exists()) {
            try (BufferedReader br = new BufferedReader(new FileReader(versionFile))) {
                gameVersion = br.readLine().trim();
                System.out.println("游戏版本: " + gameVersion);
            } catch (IOException e) {
                System.err.println("读取游戏版本失败，使用默认值: " + gameVersion);
            }
        } else {
            try (FileWriter fw = new FileWriter(versionFile)) {
                fw.write(gameVersion);
                System.out.println("创建游戏版本文件: " + GAME_VERSION_FILE);
            } catch (IOException e) {
                System.err.println("创建版本文件失败");
            }
        }
    }

    private static void scanMods() {
        File modDir = new File(MODS_DIR);
        File[] jars = modDir.listFiles((dir, name) -> name.endsWith(".jar"));

        if (jars == null || jars.length == 0) {
            System.out.println("未找到模组文件");
            return;
        }

        // 获取所有模组文件名（包括被禁用的）
        Set<String> allModFiles = new HashSet<>();
        for (File jar : jars) {
            allModFiles.add(jar.getName());
        }

        // 获取启用的模组列表
        Set<String> enabledMods = loadEnabledMods();

        System.out.println("发现 " + jars.length + " 个模组文件");

        // 先处理所有 JAR 文件
        Map<String, File> jarMap = new HashMap<>();
        for (File jar : jars) {
            jarMap.put(jar.getName(), jar);
        }

        // 处理所有模组文件
        for (String jarName : allModFiles) {
            File jarFile = jarMap.get(jarName);
            boolean isEnabled = enabledMods.contains(jarName);

            if (!isEnabled) {
                // 模组被禁用，显示为禁用状态
                ModInfo info = new ModInfo(jarName.replace(".jar", ""), "?", "?", "");
                info.setLoaded(false);
                info.setError("已禁用");
                mods.add(info);
                System.out.println("跳过禁用模组: " + jarName);
            } else {
                loadMod(jarFile);
            }
        }
    }

    private static Set<String> loadEnabledMods() {
        Set<String> enabled = new HashSet<>();
        File listFile = new File(MODS_DIR, MOD_LIST_FILE);

        if (!listFile.exists()) {
            return enabled;
        }

        try (BufferedReader br = new BufferedReader(new FileReader(listFile))) {
            String line;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                if (line.startsWith("#")) continue;

                if (line.endsWith(".jar")) {
                    enabled.add(line);
                }
            }
        } catch (IOException e) {
            System.err.println("读取模组列表失败: " + e.getMessage());
        }

        return enabled;
    }

    private static void loadMod(File jarFile) {
        try {
            URLClassLoader loader = new URLClassLoader(
                new URL[]{jarFile.toURI().toURL()},
                Thread.currentThread().getContextClassLoader()
            );

            java.util.jar.Manifest manifest = getManifest(jarFile);
            if (manifest == null) {
                System.err.println("警告 " + jarFile.getName() + " 缺少 MANIFEST.MF");
                ModInfo info = new ModInfo(jarFile.getName().replace(".jar", ""), "?", "?", "");
                info.setLoaded(false);
                info.setError("缺少 MANIFEST.MF");
                mods.add(info);
                return;
            }

            Map<String, String> attrs = new HashMap<>();
            manifest.getMainAttributes().forEach((key, value) -> {
                attrs.put(key.toString(), value.toString());
            });

            String modName = attrs.getOrDefault("Mod-Name", jarFile.getName().replace(".jar", ""));
            String modVersion = attrs.getOrDefault("Mod-Version", "1.0.0");
            String requiredGameVersion = attrs.getOrDefault("Game-Version", "1.0.0");
            String mainClass = attrs.get("Main-Class");

            if (mainClass == null) {
                System.err.println("警告 " + jarFile.getName() + " 缺少 Main-Class");
                ModInfo info = new ModInfo(modName, modVersion, requiredGameVersion, mainClass);
                info.setLoaded(false);
                info.setError("缺少 Main-Class");
                mods.add(info);
                return;
            }

            ModInfo info = new ModInfo(modName, modVersion, requiredGameVersion, mainClass);
            mods.add(info);

            if (!isCompatible(requiredGameVersion)) {
                String error = "需要游戏 v" + requiredGameVersion + "，当前 v" + gameVersion;
                info.setError(error);
                System.err.println("错误 " + modName + ": " + error);
                return;
            }

            Class<?> clazz = loader.loadClass(mainClass);
            if (!IMod.class.isAssignableFrom(clazz)) {
                String error = "未实现 IMod 接口";
                info.setError(error);
                System.err.println("错误 " + modName + ": " + error);
                return;
            }

            IMod mod = (IMod) clazz.getDeclaredConstructor().newInstance();
            info.setLoaded(true);
            loadedMods.add(mod);
            classLoaders.add(loader);
            System.out.println("加载模组: " + modName + " v" + modVersion);

        } catch (Exception e) {
            ModInfo info = new ModInfo(jarFile.getName().replace(".jar", ""), "?", "?", "");
            info.setLoaded(false);
            info.setError("加载失败: " + e.getMessage());
            mods.add(info);
            System.err.println("错误 " + jarFile.getName() + " 加载失败: " + e.getMessage());
        }
    }

    private static java.util.jar.Manifest getManifest(File jarFile) {
        try (JarFile jar = new JarFile(jarFile)) {
            return jar.getManifest();
        } catch (IOException e) {
            return null;
        }
    }

    private static boolean isCompatible(String required) {
        if (required.equals("*") || required.equals(gameVersion)) return true;

        try {
            String[] reqParts = required.split("\\.");
            String[] gameParts = gameVersion.split("\\.");
            int minLen = Math.min(reqParts.length, gameParts.length);

            for (int i = 0; i < minLen; i++) {
                int req = Integer.parseInt(reqParts[i]);
                int game = Integer.parseInt(gameParts[i]);
                if (game != req) {
                    return game > req;
                }
            }
            return reqParts.length <= gameParts.length;
        } catch (NumberFormatException e) {
            return required.equals(gameVersion) || required.equals("*");
        }
    }

    public static void onGameLoad(Game game) {
        for (IMod mod : loadedMods) {
            try {
                mod.onLoad(game);
                System.out.println("模组初始化完成: " + mod.getModName());
            } catch (Exception e) {
                System.err.println("模组 " + mod.getModName() + " 初始化失败: " + e.getMessage());
            }
        }
    }

    public static void onGameUpdate(double dt) {
        for (IMod mod : loadedMods) {
            try {
                mod.onUpdate(dt);
            } catch (Exception e) {
            }
        }
    }

    public static void unloadAll() {
        for (IMod mod : loadedMods) {
            try {
                mod.onUnload();
            } catch (Exception e) {
            }
        }

        for (URLClassLoader loader : classLoaders) {
            try {
                loader.close();
            } catch (IOException e) {
            }
        }
        classLoaders.clear();
        loadedMods.clear();
        mods.clear();
    }

    public static List<ModInfo> getMods() {
        return mods;
    }

    public static List<IMod> getLoadedMods() {
        return loadedMods;
    }

    public static String getGameVersion() {
        return gameVersion;
    }

    public static void toggleMod(int index) {
        if (index < 0 || index >= mods.size()) return;

        ModInfo info = mods.get(index);
    
        // 尝试匹配 jar 文件名
        String jarName = info.getName();
        if (!jarName.endsWith(".jar")) {
            jarName = jarName + ".jar";
        }

        File listFile = new File(MODS_DIR, MOD_LIST_FILE);
        List<String> lines = new ArrayList<>();
        boolean found = false;

        try (BufferedReader br = new BufferedReader(new FileReader(listFile))) {
            String line;
            while ((line = br.readLine()) != null) {
                String trimmed = line.trim();
                String clean = trimmed.replaceFirst("^#\\s*", "").trim();

                // 匹配时忽略大小写
                if (clean.equalsIgnoreCase(jarName) || clean.equalsIgnoreCase(info.getName())) {
                    found = true;
                    if (line.trim().startsWith("#")) {
                        lines.add(clean);
                    } else {
                        lines.add("# " + clean);
                    }
                } else {
                    lines.add(line);
                }
            }
        } catch (IOException e) {
            System.err.println("读取模组列表失败: " + e.getMessage());
            return;
        }

        if (!found) {
            if (info.isLoaded()) {
                lines.add("# " + jarName);
            } else {
                lines.add(jarName);
            }
        }

        try (FileWriter fw = new FileWriter(listFile)) {
            for (String line : lines) {
                fw.write(line + "\n");
            }
        } catch (IOException e) {
            System.err.println("写入模组列表失败: " + e.getMessage());
            return;
        }

        System.out.println("模组切换: " + info.getName() + " -> " + 
            (info.isLoaded() ? "已禁用 (重启生效)" : "已启用 (重启生效)"));
    }
}