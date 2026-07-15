#pragma once
#include <vector>
#include "Wall.h"

class MapGenerator {
public:
    static void generateMap(int mapId, std::vector<Wall>& walls);
private:
    static void buildBorder(std::vector<Wall>& walls);
    static bool addWall(std::vector<Wall>& walls, int col, int row, int w, int h, bool isSteel);
    static void addBrick(std::vector<Wall>& walls, int col, int row, int w, int h);
    static void addSteel(std::vector<Wall>& walls, int col, int row, int w, int h);
    static void mapEmpty(std::vector<Wall>& walls);
    static void mapCross(std::vector<Wall>& walls);
    static void mapMaze(std::vector<Wall>& walls);
    static void mapBunker(std::vector<Wall>& walls);
    static void mapSymmetric(std::vector<Wall>& walls);
};