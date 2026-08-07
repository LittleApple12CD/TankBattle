package com.tankbattle.resource;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.HashMap;
import java.util.Map;

public class TextureManager {

    private static Map<String, BufferedImage> textures = new HashMap<>();
    private static Map<String, String> entityMapping = new HashMap<>();

    public static void registerTexture(String entityId, BufferedImage image) {
        textures.put(entityId, image);
    }

    public static void registerMapping(String entityId, String fileName) {
        entityMapping.put(entityId, fileName);
    }

    public static BufferedImage getTexture(String entityId) {
        return textures.get(entityId);
    }

    public static boolean hasTexture(String entityId) {
        return textures.containsKey(entityId);
    }

    public static BufferedImage loadImage(File file) {
        try {
            return ImageIO.read(file);
        } catch (IOException e) {
            System.err.println("加载贴图失败: " + file.getName() + " - " + e.getMessage());
            return null;
        }
    }

    public static BufferedImage loadImage(InputStream is) {
        try {
            return ImageIO.read(is);
        } catch (IOException e) {
            System.err.println("加载贴图失败: " + e.getMessage());
            return null;
        }
    }

    public static void clear() {
        textures.clear();
        entityMapping.clear();
    }

    public static Map<String, String> getEntityMapping() {
        return entityMapping;
    }

    public static boolean hasEntityTexture(String entityId) {
        return textures.containsKey(entityId);
    }

    public static BufferedImage getEntityTexture(String entityId) {
        return textures.getOrDefault(entityId, null);
    }
}