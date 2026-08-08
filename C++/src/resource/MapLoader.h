// src/resource/MapLoader.h
#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class Wall;
class Game;

class MapLoader {
public:
    static bool loadMap(const std::string& jsonFile, std::vector<Wall>& walls);
    static bool parseMapJson(const std::string& json, std::vector<Wall>& walls);
    static std::string getMapName(const std::string& jsonFile);
    static std::vector<std::string> findMaps(const std::string& directory);
    static int loadAllMaps(const std::string& directory, std::vector<Wall>& walls);

private:
    static void parseWallArray(const nlohmann::json& array, 
                               std::vector<Wall>& walls, 
                               bool isSteel);

    static int findMatchingBracket(const std::string& str, int start);
    static bool parseMapJsonManual(const std::string& json, std::vector<Wall>& walls);
    static void parseWallArrayManual(const std::string& json, 
                                     std::vector<Wall>& walls, 
                                     bool isSteel);
};