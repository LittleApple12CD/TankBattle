package com.tankbattle.resource;

import com.tankbattle.Wall;

import java.io.*;
import java.util.List;

import static com.tankbattle.Utils.*;

/**
 * 地图加载器
 * 从 JSON 文件加载地图数据
 */
public class MapLoader {

    /**
     * 从 JSON 文件加载地图
     * 文件格式：
     * {
     *   "name": "我的地图",
     *   "bricks": [[2,6,1,1], [3,6,1,1], ...],
     *   "steels": [[6,6,1,1], [7,6,1,1], ...]
     * }
     */
    public static void loadMap(File jsonFile, List<Wall> walls) {
        if (jsonFile == null || !jsonFile.exists()) {
            System.err.println("地图文件不存在: " + jsonFile);
            return;
        }

        try (BufferedReader reader = new BufferedReader(
                new InputStreamReader(new FileInputStream(jsonFile), "UTF-8"))) {
            
            StringBuilder content = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                content.append(line.trim());
            }

            String json = content.toString();
            parseMapJson(json, walls);
            System.out.println("加载地图: " + jsonFile.getName());

        } catch (IOException e) {
            System.err.println("加载地图失败: " + jsonFile.getName() + " - " + e.getMessage());
        }
    }

    /**
     * 从 JSON 字符串解析地图
     */
    private static void parseMapJson(String json, List<Wall> walls) {
        // 简单解析（不依赖第三方库，纯手动解析）
        // 实际项目建议使用 Jackson 或 Gson
        
        try {
            // 解析 bricks
            int brickStart = json.indexOf("\"bricks\"");
            if (brickStart != -1) {
                int arrayStart = json.indexOf("[", brickStart);
                int arrayEnd = findMatchingBracket(json, arrayStart);
                String bricksJson = json.substring(arrayStart + 1, arrayEnd);
                parseWallArray(bricksJson, walls, false);
            }

            // 解析 steels
            int steelStart = json.indexOf("\"steels\"");
            if (steelStart != -1) {
                int arrayStart = json.indexOf("[", steelStart);
                int arrayEnd = findMatchingBracket(json, arrayStart);
                String steelsJson = json.substring(arrayStart + 1, arrayEnd);
                parseWallArray(steelsJson, walls, true);
            }

        } catch (Exception e) {
            System.err.println("解析地图JSON失败: " + e.getMessage());
        }
    }

    /**
     * 解析墙壁数组
     */
    private static void parseWallArray(String json, List<Wall> walls, boolean isSteel) {
        // 查找所有 [col,row,w,h] 格式
        int pos = 0;
        while (true) {
            int start = json.indexOf("[", pos);
            if (start == -1) break;
            int end = findMatchingBracket(json, start);
            if (end == -1) break;

            String wallStr = json.substring(start + 1, end);
            String[] parts = wallStr.split(",");
            if (parts.length >= 4) {
                try {
                    int col = Integer.parseInt(parts[0].trim());
                    int row = Integer.parseInt(parts[1].trim());
                    int w = Integer.parseInt(parts[2].trim());
                    int h = Integer.parseInt(parts[3].trim());
                    
                    int x = GRID_OFFSET_X + col * CELL_SIZE;
                    int y = GRID_OFFSET_Y + row * CELL_SIZE;
                    Wall wall = new Wall(x, y, w * CELL_SIZE, h * CELL_SIZE, isSteel);
                    
                    // 检查是否与现有墙壁重叠
                    boolean overlap = false;
                    for (Wall existing : walls) {
                        if (existing.alive && wall.getRect().intersects(existing.getRect())) {
                            overlap = true;
                            break;
                        }
                    }
                    if (!overlap) {
                        walls.add(wall);
                    }
                } catch (NumberFormatException e) {
                    // 跳过无效条目
                }
            }
            pos = end + 1;
        }
    }

    /**
     * 查找匹配的括号
     */
    private static int findMatchingBracket(String str, int start) {
        char open = str.charAt(start);
        char close = (open == '[') ? ']' : '}';
        int depth = 0;
        for (int i = start; i < str.length(); i++) {
            char c = str.charAt(i);
            if (c == open) depth++;
            else if (c == close) {
                depth--;
                if (depth == 0) return i;
            }
        }
        return -1;
    }
}