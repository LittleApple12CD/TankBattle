package com.tankbattle;

import java.io.*;

/**
 * 存档管理
 */
public class SaveManager {

    private static final String SAVE_DIR = "PlayerData";
    private static final String SAVE_FILE = "LevelSaves.dat";

    public static void ensureSaveDir() {
        File dir = new File(SAVE_DIR);
        if (!dir.exists()) {
            dir.mkdirs();
        }
    }

    public static void saveProgress(int level) {
        ensureSaveDir();
        String path = SAVE_DIR + File.separator + SAVE_FILE;
        try (DataOutputStream dos = new DataOutputStream(new FileOutputStream(path))) {
            dos.writeInt(level);
        } catch (IOException e) {
            System.err.println("保存失败: " + e.getMessage());
        }
    }

    public static int loadProgress() {
        ensureSaveDir();
        String path = SAVE_DIR + File.separator + SAVE_FILE;
        File file = new File(path);
        if (!file.exists()) {
            return 1; // 没有存档，从第1关开始
        }
        try (DataInputStream dis = new DataInputStream(new FileInputStream(file))) {
            return dis.readInt();
        } catch (IOException e) {
            return 1;
        }
    }

    public static boolean hasSave() {
        String path = SAVE_DIR + File.separator + SAVE_FILE;
        return new File(path).exists();
    }
}