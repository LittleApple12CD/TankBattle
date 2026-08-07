package com.tankbattle.resource;

import com.tankbattle.Game;
import com.tankbattle.Wall;
import org.json.JSONObject;

import java.io.*;
import java.nio.file.Files;
import java.util.*;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class ResourcePackLoader {

    private static final String RESOURCEPACKS_DIR = "resourcepacks";
    private static List<String> loadedPacks = new ArrayList<>();

    public static void init(Game game) {
        File packDir = new File(RESOURCEPACKS_DIR);
        if (!packDir.exists()) {
            packDir.mkdirs();
            System.out.println("创建 resourcepacks 目录");
            return;
        }

        File[] entries = packDir.listFiles();
        if (entries == null) return;

        for (File entry : entries) {
            if (entry.isDirectory()) {
                loadResourcePack(entry, game);
            } else if (entry.getName().endsWith(".zip")) {
                loadResourcePackZip(entry, game);
            }
        }

        System.out.println("加载了 " + loadedPacks.size() + " 个资源包");
    }

    private static void loadResourcePack(File packDir, Game game) {
        File packJson = new File(packDir, "pack.json");
        if (!packJson.exists()) {
            return;
        }

        try {
            PackInfo info = parsePackJson(packJson);
            if (info == null) return;

            System.out.println("加载资源包: " + info.name + " v" + info.version);

            File imagesDir = new File(packDir, "images");
            if (imagesDir.exists()) {
                loadTextures(imagesDir);
            }

            File mapsDir = new File(packDir, "maps");
            if (mapsDir.exists()) {
                loadMaps(mapsDir, game);
            }

            loadedPacks.add(info.name);

        } catch (Exception e) {
            System.err.println("加载资源包失败: " + packDir.getName() + " - " + e.getMessage());
        }
    }

    private static void loadResourcePackZip(File zipFile, Game game) {
        try (ZipFile zip = new ZipFile(zipFile)) {
            ZipEntry packEntry = zip.getEntry("pack.json");
            if (packEntry == null) {
                System.err.println("ZIP包缺少 pack.json: " + zipFile.getName());
                return;
            }

            PackInfo info = parsePackJson(zip.getInputStream(packEntry));
            if (info == null) return;

            System.out.println("加载资源包(ZIP): " + info.name + " v" + info.version);

            loadTexturesFromZip(zip, "images/");
            loadMapsFromZip(zip, game);

            loadedPacks.add(info.name);

        } catch (IOException e) {
            System.err.println("加载ZIP资源包失败: " + zipFile.getName() + " - " + e.getMessage());
        }
    }

    private static PackInfo parsePackJson(File file) throws IOException {
        String content = new String(Files.readAllBytes(file.toPath()), "UTF-8");
        return parsePackJsonContent(content);
    }

    private static PackInfo parsePackJson(InputStream is) throws IOException {
        StringBuilder content = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"))) {
            String line;
            while ((line = reader.readLine()) != null) {
                content.append(line);
            }
        }
        return parsePackJsonContent(content.toString());
    }

    private static PackInfo parsePackJsonContent(String json) {
        try {
            JSONObject obj = new JSONObject(json);
            PackInfo info = new PackInfo();
            info.name = obj.optString("name", null);
            info.version = obj.optString("version", "1.0.0");
            info.type = obj.optString("type", "ResourcePack");

            if (info.name == null || info.name.isEmpty()) {
                System.err.println("pack.json 缺少 name 字段");
                return null;
            }

            return info;
        } catch (Exception e) {
            System.err.println("解析 pack.json 失败: " + e.getMessage());
            return null;
        }
    }

    private static void loadTextures(File imagesDir) {
        File mappingFile = new File(imagesDir, "images.json");
        Map<String, String> mapping = new HashMap<>();

        if (mappingFile.exists()) {
            try {
                String content = new String(Files.readAllBytes(mappingFile.toPath()), "UTF-8");
                parseMappingJson(content, mapping);
            } catch (IOException e) {
                System.err.println("读取 images.json 失败: " + e.getMessage());
            }
        }

        File[] pngFiles = imagesDir.listFiles((dir, name) -> name.endsWith(".png"));
        if (pngFiles != null) {
            for (File png : pngFiles) {
                java.awt.image.BufferedImage img = TextureManager.loadImage(png);
                if (img != null) {
                    String name = png.getName().replace(".png", "");
                    TextureManager.registerTexture(name, img);

                    for (Map.Entry<String, String> entry : mapping.entrySet()) {
                        if (entry.getValue().equals(png.getName())) {
                            TextureManager.registerTexture(entry.getKey(), img);
                        }
                    }
                }
            }
        }
    }

    private static void loadTexturesFromZip(ZipFile zip, String prefix) {
        ZipEntry mappingEntry = zip.getEntry(prefix + "images.json");
        Map<String, String> mapping = new HashMap<>();

        if (mappingEntry != null) {
            try {
                StringBuilder content = new StringBuilder();
                try (BufferedReader reader = new BufferedReader(
                        new InputStreamReader(zip.getInputStream(mappingEntry), "UTF-8"))) {
                    String line;
                    while ((line = reader.readLine()) != null) {
                        content.append(line);
                    }
                }
                parseMappingJson(content.toString(), mapping);
            } catch (IOException e) {
                System.err.println("读取 images.json 失败: " + e.getMessage());
            }
        }

        zip.stream()
            .filter(entry -> entry.getName().startsWith(prefix) && entry.getName().endsWith(".png"))
            .forEach(entry -> {
                try {
                    java.awt.image.BufferedImage img = TextureManager.loadImage(zip.getInputStream(entry));
                    if (img != null) {
                        String name = entry.getName().replace(prefix, "").replace(".png", "");
                        TextureManager.registerTexture(name, img);

                        for (Map.Entry<String, String> mapEntry : mapping.entrySet()) {
                            if (mapEntry.getValue().equals(entry.getName().replace(prefix, ""))) {
                                TextureManager.registerTexture(mapEntry.getKey(), img);
                            }
                        }
                    }
                } catch (IOException e) {
                    System.err.println("加载贴图失败: " + entry.getName());
                }
            });
    }

    private static void parseMappingJson(String json, Map<String, String> mapping) {
        try {
            JSONObject obj = new JSONObject(json);
            JSONObject mappings = obj.optJSONObject("mappings");
            if (mappings == null) return;

            for (String key : mappings.keySet()) {
                mapping.put(key, mappings.getString(key));
            }
        } catch (Exception e) {
            System.err.println("解析 images.json 失败: " + e.getMessage());
        }
    }

    private static void loadMaps(File mapsDir, Game game) {
        File[] jsonFiles = mapsDir.listFiles((dir, name) -> name.endsWith(".json"));
        if (jsonFiles == null) return;

        for (File jsonFile : jsonFiles) {
            MapLoader.loadMap(jsonFile, game.getWalls());
        }
    }

    private static void loadMapsFromZip(ZipFile zip, Game game) {
        zip.stream()
            .filter(entry -> entry.getName().startsWith("maps/") && entry.getName().endsWith(".json"))
            .forEach(entry -> {
                try {
                    String content;
                    try (BufferedReader reader = new BufferedReader(
                            new InputStreamReader(zip.getInputStream(entry), "UTF-8"))) {
                        StringBuilder sb = new StringBuilder();
                        String line;
                        while ((line = reader.readLine()) != null) {
                            sb.append(line);
                        }
                        content = sb.toString();
                    }

                    java.util.List<Wall> walls = game.getWalls();
                    parseMapJson(content, walls);

                } catch (IOException e) {
                    System.err.println("加载地图失败: " + entry.getName());
                }
            });
    }

    private static void parseMapJson(String json, java.util.List<Wall> walls) {
        try {
            JSONObject obj = new JSONObject(json);
            parseWallArray(obj.optJSONArray("bricks"), walls, false);
            parseWallArray(obj.optJSONArray("steels"), walls, true);
        } catch (Exception e) {
            System.err.println("解析地图JSON失败: " + e.getMessage());
        }
    }

    private static void parseWallArray(org.json.JSONArray array, java.util.List<Wall> walls, boolean isSteel) {
        if (array == null) return;

        for (int i = 0; i < array.length(); i++) {
            org.json.JSONArray wall = array.getJSONArray(i);
            if (wall.length() >= 4) {
                try {
                    int col = wall.getInt(0);
                    int row = wall.getInt(1);
                    int w = wall.getInt(2);
                    int h = wall.getInt(3);

                    int x = com.tankbattle.Utils.GRID_OFFSET_X + col * com.tankbattle.Utils.CELL_SIZE;
                    int y = com.tankbattle.Utils.GRID_OFFSET_Y + row * com.tankbattle.Utils.CELL_SIZE;
                    Wall wallObj = new Wall(x, y, w * com.tankbattle.Utils.CELL_SIZE, 
                                            h * com.tankbattle.Utils.CELL_SIZE, isSteel);

                    boolean overlap = false;
                    for (Wall existing : walls) {
                        if (existing.alive && wallObj.getRect().intersects(existing.getRect())) {
                            overlap = true;
                            break;
                        }
                    }
                    if (!overlap) {
                        walls.add(wallObj);
                    }
                } catch (Exception e) {
                }
            }
        }
    }

    private static class PackInfo {
        String name;
        String version;
        String type;
    }
}