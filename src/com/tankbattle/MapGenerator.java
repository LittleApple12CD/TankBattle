package com.tankbattle;

import java.util.List;

import static com.tankbattle.Utils.*;

/**
 * 地图生成器
 */
public class MapGenerator {

    /**
     * 生成地图
     */
    public static void generateMap(int mapId, List<Wall> walls) {
        walls.clear();
        buildBorder(walls);

        switch (mapId) {
            case 0: mapEmpty(walls); break;
            case 1: mapCross(walls); break;
            case 2: mapMaze(walls); break;
            case 3: mapBunker(walls); break;
            case 4: mapSymmetric(walls); break;
            default: mapEmpty(walls); break;
        }
    }

    private static void buildBorder(List<Wall> walls) {
        int margin = CELL_SIZE / 2;
        int wallThick = margin;

        for (int i = 0; i < GRID_SIZE; i++) {
            int x = GRID_OFFSET_X + i * CELL_SIZE;
            int y = GRID_OFFSET_Y + i * CELL_SIZE;

            // 上边
            walls.add(new Wall(x, GRID_OFFSET_Y, CELL_SIZE, wallThick, false));
            // 下边
            walls.add(new Wall(x, GRID_OFFSET_Y + GRID_SIZE * CELL_SIZE - wallThick, CELL_SIZE, wallThick, false));
            // 左边
            walls.add(new Wall(GRID_OFFSET_X, y, wallThick, CELL_SIZE, false));
            // 右边
            walls.add(new Wall(GRID_OFFSET_X + GRID_SIZE * CELL_SIZE - wallThick, y, wallThick, CELL_SIZE, false));
        }
    }

    private static boolean addWall(List<Wall> walls, int col, int row, int w, int h, boolean isSteel) {
        int x = GRID_OFFSET_X + col * CELL_SIZE;
        int y = GRID_OFFSET_Y + row * CELL_SIZE;
        Wall wall = new Wall(x, y, w * CELL_SIZE, h * CELL_SIZE, isSteel);

        for (Wall existing : walls) {
            if (existing.alive && wall.getRect().intersects(existing.getRect())) {
                return false;
            }
        }
        walls.add(wall);
        return true;
    }

    private static void addBrick(List<Wall> walls, int col, int row, int w, int h) {
        addWall(walls, col, row, w, h, false);
    }

    private static void addSteel(List<Wall> walls, int col, int row, int w, int h) {
        addWall(walls, col, row, w, h, true);
    }

    // ===== 地图布局 =====

    private static void mapEmpty(List<Wall> walls) {
        // 空地图，只有边界
    }

    private static void mapCross(List<Wall> walls) {
        for (int col = 2; col < 11; col++) {
            addBrick(walls, col, 6, 1, 1);
        }
        for (int row = 2; row < 11; row++) {
            addBrick(walls, 6, row, 1, 1);
        }
        addSteel(walls, 6, 6, 1, 1);
        addBrick(walls, 3, 3, 1, 1);
        addBrick(walls, 9, 3, 1, 1);
        addBrick(walls, 3, 9, 1, 1);
        addBrick(walls, 9, 9, 1, 1);
    }

    private static void mapMaze(List<Wall> walls) {
        int[][] maze = {
                {2, 2, 1, 3}, {8, 2, 1, 3},
                {3, 4, 3, 1}, {7, 4, 3, 1},
                {2, 5, 1, 3}, {8, 5, 1, 3},
                {4, 7, 1, 2}, {6, 7, 1, 2},
                {3, 8, 3, 1}, {7, 8, 3, 1},
                {2, 9, 1, 2}, {8, 9, 1, 2},
        };
        for (int[] m : maze) {
            addBrick(walls, m[0], m[1], m[2], m[3]);
        }
        addSteel(walls, 5, 5, 1, 1);
        addSteel(walls, 7, 5, 1, 1);
        addSteel(walls, 5, 7, 1, 1);
        addSteel(walls, 7, 7, 1, 1);
    }

    private static void mapBunker(List<Wall> walls) {
        int[][] bunkers = {
                {2, 2, 2, 1}, {10, 2, 2, 1},
                {2, 4, 1, 2}, {11, 4, 1, 2},
                {2, 8, 2, 1}, {10, 8, 2, 1},
                {2, 10, 1, 2}, {11, 10, 1, 2},
                {4, 3, 1, 1}, {9, 3, 1, 1},
                {4, 9, 1, 1}, {9, 9, 1, 1},
        };
        for (int[] b : bunkers) {
            addBrick(walls, b[0], b[1], b[2], b[3]);
        }
        addSteel(walls, 6, 11, 1, 1);
        addSteel(walls, 7, 11, 1, 1);
        addSteel(walls, 5, 11, 1, 1);
        addSteel(walls, 8, 11, 1, 1);
    }

    private static void mapSymmetric(List<Wall> walls) {
        for (int i = 2; i < 5; i++) {
            addBrick(walls, i, 3, 1, 1);
            addBrick(walls, 12 - i, 3, 1, 1);
            addBrick(walls, i, 9, 1, 1);
            addBrick(walls, 12 - i, 9, 1, 1);
        }
        for (int i = 4; i < 7; i++) {
            addBrick(walls, 3, i, 1, 1);
            addBrick(walls, 10, i, 1, 1);
            addBrick(walls, 3, 12 - i, 1, 1);
            addBrick(walls, 10, 12 - i, 1, 1);
        }
        addSteel(walls, 6, 5, 1, 1);
        addSteel(walls, 7, 5, 1, 1);
        addSteel(walls, 6, 7, 1, 1);
        addSteel(walls, 7, 7, 1, 1);
        addBrick(walls, 5, 6, 1, 1);
        addBrick(walls, 8, 6, 1, 1);
    }
}