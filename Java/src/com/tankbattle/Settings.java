package com.tankbattle;

import java.io.*;
import java.util.Properties;
import java.awt.event.KeyEvent;

/**
 * 设置管理器 v1.8
 * 负责加载/保存配置文件，管理所有游戏设置
 */
public class Settings {

    private static final String CONFIG_FILE = "settings.properties";
    private static Properties props = new Properties();

    // ===== 设置项（默认值） =====
    public static int WINDOW_WIDTH = 1600;
    public static int WINDOW_HEIGHT = 900;
    public static float SOUND_VOLUME = 0.8f;
    public static boolean SOUND_MUTED = false;
    public static boolean SOUND_ENABLED = true;

    // ===== 按键绑定 =====
    // P1
    public static int KEY_P1_UP = 38;      // UP
    public static int KEY_P1_DOWN = 40;    // DOWN
    public static int KEY_P1_LEFT = 37;    // LEFT
    public static int KEY_P1_RIGHT = 39;   // RIGHT
    public static int KEY_P1_SHOOT = 32;   // SPACE

    // P2
    public static int KEY_P2_UP = 87;      // W
    public static int KEY_P2_DOWN = 83;    // S
    public static int KEY_P2_LEFT = 65;    // A
    public static int KEY_P2_RIGHT = 68;   // D
    public static int KEY_P2_SHOOT = 74;   // J

    // ===== 游戏默认值（备份，用于重置） =====
    private static final int DEFAULT_WINDOW_WIDTH = 1600;
    private static final int DEFAULT_WINDOW_HEIGHT = 900;
    private static final float DEFAULT_SOUND_VOLUME = 0.8f;
    private static final boolean DEFAULT_SOUND_MUTED = false;
    private static final boolean DEFAULT_SOUND_ENABLED = true;

    private static final int DEFAULT_KEY_P1_UP = 38;
    private static final int DEFAULT_KEY_P1_DOWN = 40;
    private static final int DEFAULT_KEY_P1_LEFT = 37;
    private static final int DEFAULT_KEY_P1_RIGHT = 39;
    private static final int DEFAULT_KEY_P1_SHOOT = 32;
    private static final int DEFAULT_KEY_P2_UP = 87;
    private static final int DEFAULT_KEY_P2_DOWN = 83;
    private static final int DEFAULT_KEY_P2_LEFT = 65;
    private static final int DEFAULT_KEY_P2_RIGHT = 68;
    private static final int DEFAULT_KEY_P2_SHOOT = 74;

    // ===== 窗口尺寸选项 =====
    public static final int[][] WINDOW_SIZES = {
        {1200, 800},
        {1600, 900},
        {1920, 1080}
    };
    public static int currentSizeIndex = 1; // 默认 1600x900

    static {
        load();
    }

    /**
     * 加载配置文件
     */
    public static void load() {
        File file = new File(CONFIG_FILE);
        if (!file.exists()) {
            save(); // 不存在则创建默认配置
            return;
        }

        try (FileInputStream fis = new FileInputStream(file)) {
            props.load(fis);

            // 读取窗口尺寸
            int w = Integer.parseInt(props.getProperty("window.width", String.valueOf(DEFAULT_WINDOW_WIDTH)));
            int h = Integer.parseInt(props.getProperty("window.height", String.valueOf(DEFAULT_WINDOW_HEIGHT)));
            WINDOW_WIDTH = w;
            WINDOW_HEIGHT = h;
            currentSizeIndex = findSizeIndex(w, h);

            // 读取音效
            SOUND_VOLUME = Float.parseFloat(props.getProperty("sound.volume", String.valueOf(DEFAULT_SOUND_VOLUME)));
            SOUND_MUTED = Boolean.parseBoolean(props.getProperty("sound.muted", String.valueOf(DEFAULT_SOUND_MUTED)));
            SOUND_ENABLED = Boolean.parseBoolean(props.getProperty("sound.enabled", String.valueOf(DEFAULT_SOUND_ENABLED)));

            // 读取按键
            KEY_P1_UP = Integer.parseInt(props.getProperty("key.p1.up", String.valueOf(DEFAULT_KEY_P1_UP)));
            KEY_P1_DOWN = Integer.parseInt(props.getProperty("key.p1.down", String.valueOf(DEFAULT_KEY_P1_DOWN)));
            KEY_P1_LEFT = Integer.parseInt(props.getProperty("key.p1.left", String.valueOf(DEFAULT_KEY_P1_LEFT)));
            KEY_P1_RIGHT = Integer.parseInt(props.getProperty("key.p1.right", String.valueOf(DEFAULT_KEY_P1_RIGHT)));
            KEY_P1_SHOOT = Integer.parseInt(props.getProperty("key.p1.shoot", String.valueOf(DEFAULT_KEY_P1_SHOOT)));

            KEY_P2_UP = Integer.parseInt(props.getProperty("key.p2.up", String.valueOf(DEFAULT_KEY_P2_UP)));
            KEY_P2_DOWN = Integer.parseInt(props.getProperty("key.p2.down", String.valueOf(DEFAULT_KEY_P2_DOWN)));
            KEY_P2_LEFT = Integer.parseInt(props.getProperty("key.p2.left", String.valueOf(DEFAULT_KEY_P2_LEFT)));
            KEY_P2_RIGHT = Integer.parseInt(props.getProperty("key.p2.right", String.valueOf(DEFAULT_KEY_P2_RIGHT)));
            KEY_P2_SHOOT = Integer.parseInt(props.getProperty("key.p2.shoot", String.valueOf(DEFAULT_KEY_P2_SHOOT)));

            System.out.println("✅ 设置加载成功: " + CONFIG_FILE);

        } catch (Exception e) {
            System.err.println("⚠️ 加载设置失败，使用默认值: " + e.getMessage());
            resetToDefaults();
        }
    }

    /**
     * 保存配置文件
     */
    public static void save() {
        props.setProperty("window.width", String.valueOf(WINDOW_WIDTH));
        props.setProperty("window.height", String.valueOf(WINDOW_HEIGHT));
        props.setProperty("sound.volume", String.valueOf(SOUND_VOLUME));
        props.setProperty("sound.muted", String.valueOf(SOUND_MUTED));
        props.setProperty("sound.enabled", String.valueOf(SOUND_ENABLED));

        props.setProperty("key.p1.up", String.valueOf(KEY_P1_UP));
        props.setProperty("key.p1.down", String.valueOf(KEY_P1_DOWN));
        props.setProperty("key.p1.left", String.valueOf(KEY_P1_LEFT));
        props.setProperty("key.p1.right", String.valueOf(KEY_P1_RIGHT));
        props.setProperty("key.p1.shoot", String.valueOf(KEY_P1_SHOOT));

        props.setProperty("key.p2.up", String.valueOf(KEY_P2_UP));
        props.setProperty("key.p2.down", String.valueOf(KEY_P2_DOWN));
        props.setProperty("key.p2.left", String.valueOf(KEY_P2_LEFT));
        props.setProperty("key.p2.right", String.valueOf(KEY_P2_RIGHT));
        props.setProperty("key.p2.shoot", String.valueOf(KEY_P2_SHOOT));

        try (FileOutputStream fos = new FileOutputStream(CONFIG_FILE)) {
            props.store(fos, "Tank Battle v1.8 Settings");
            System.out.println("✅ 设置保存成功: " + CONFIG_FILE);
        } catch (IOException e) {
            System.err.println("❌ 保存设置失败: " + e.getMessage());
        }
    }

    /**
     * 重置为默认值
     */
    public static void resetToDefaults() {
        WINDOW_WIDTH = DEFAULT_WINDOW_WIDTH;
        WINDOW_HEIGHT = DEFAULT_WINDOW_HEIGHT;
        SOUND_VOLUME = DEFAULT_SOUND_VOLUME;
        SOUND_MUTED = DEFAULT_SOUND_MUTED;
        SOUND_ENABLED = DEFAULT_SOUND_ENABLED;

        KEY_P1_UP = DEFAULT_KEY_P1_UP;
        KEY_P1_DOWN = DEFAULT_KEY_P1_DOWN;
        KEY_P1_LEFT = DEFAULT_KEY_P1_LEFT;
        KEY_P1_RIGHT = DEFAULT_KEY_P1_RIGHT;
        KEY_P1_SHOOT = DEFAULT_KEY_P1_SHOOT;

        KEY_P2_UP = DEFAULT_KEY_P2_UP;
        KEY_P2_DOWN = DEFAULT_KEY_P2_DOWN;
        KEY_P2_LEFT = DEFAULT_KEY_P2_LEFT;
        KEY_P2_RIGHT = DEFAULT_KEY_P2_RIGHT;
        KEY_P2_SHOOT = DEFAULT_KEY_P2_SHOOT;

        currentSizeIndex = findSizeIndex(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        save();
    }

    private static int findSizeIndex(int w, int h) {
        for (int i = 0; i < WINDOW_SIZES.length; i++) {
            if (WINDOW_SIZES[i][0] == w && WINDOW_SIZES[i][1] == h) {
                return i;
            }
        }
        return 1; // 默认 1600x900
    }

    /**
     * 获取按键名称（用于显示）
     */
    public static String getKeyName(int keyCode) {
        return KeyEvent.getKeyText(keyCode);
    }

    /**
     * 获取窗口尺寸文本
     */
    public static String getWindowSizeText() {
        return WINDOW_WIDTH + "x" + WINDOW_HEIGHT;
    }

    /**
     * 切换窗口尺寸（循环）
     */
    public static void nextWindowSize() {
        currentSizeIndex = (currentSizeIndex + 1) % WINDOW_SIZES.length;
        WINDOW_WIDTH = WINDOW_SIZES[currentSizeIndex][0];
        WINDOW_HEIGHT = WINDOW_SIZES[currentSizeIndex][1];
    }

    public static void prevWindowSize() {
        currentSizeIndex = (currentSizeIndex - 1 + WINDOW_SIZES.length) % WINDOW_SIZES.length;
        WINDOW_WIDTH = WINDOW_SIZES[currentSizeIndex][0];
        WINDOW_HEIGHT = WINDOW_SIZES[currentSizeIndex][1];
    }

    // ===== 同步到 Utils =====
    public static void applyToUtils() {
        Utils.WINDOW_WIDTH = WINDOW_WIDTH;
        Utils.WINDOW_HEIGHT = WINDOW_HEIGHT;
        Utils.SOUND_VOLUME = SOUND_VOLUME;
        Utils.SOUND_MUTED = SOUND_MUTED;
        Utils.SOUND_ENABLED = SOUND_ENABLED;
    }
}