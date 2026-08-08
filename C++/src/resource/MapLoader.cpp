// src/resource/MapLoader.cpp
#include "MapLoader.h"
#include "../Wall.h"
#include "../Utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace fs = std::filesystem;
using json = nlohmann::json;

bool MapLoader::loadMap(const std::string& jsonFile, std::vector<Wall>& walls) {
    // 检查文件是否存在
    if (!fs::exists(jsonFile)) {
        std::cerr << "地图文件不存在: " << jsonFile << std::endl;
        return false;
    }

    // 读取文件内容
    std::ifstream file(jsonFile);
    if (!file.is_open()) {
        std::cerr << "无法打开地图文件: " << jsonFile << std::endl;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    // 解析 JSON
    bool result = parseMapJson(content, walls);
    if (result) {
        std::cout << "加载地图: " << fs::path(jsonFile).filename().string() << std::endl;
    }
    return result;
}

bool MapLoader::parseMapJson(const std::string& json, std::vector<Wall>& walls) {
    try {
        nlohmann::json data = nlohmann::json::parse(json);

        // 解析 bricks
        if (data.contains("bricks") && data["bricks"].is_array()) {
            parseWallArray(data["bricks"], walls, false);
        }

        // 解析 steels
        if (data.contains("steels") && data["steels"].is_array()) {
            parseWallArray(data["steels"], walls, true);
        }

        return true;

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON 解析失败: " << e.what() << std::endl;
        return parseMapJsonManual(json, walls);
    } catch (const std::exception& e) {
        std::cerr << "解析地图JSON失败: " << e.what() << std::endl;
        return false;
    }
}

void MapLoader::parseWallArray(const json& array, 
                               std::vector<Wall>& walls, 
                               bool isSteel) {
    for (const auto& item : array) {
        if (!item.is_array() || item.size() < 4) continue;

        try {
            int col = item[0].get<int>();
            int row = item[1].get<int>();
            int w = item[2].get<int>();
            int h = item[3].get<int>();

            int x = GRID_OFFSET_X + col * CELL_SIZE;
            int y = GRID_OFFSET_Y + row * CELL_SIZE;

            Wall wall(x, y, w * CELL_SIZE, h * CELL_SIZE, isSteel);

            // 检查是否与现有墙壁重叠
            bool overlap = false;
            for (const auto& existing : walls) {
                if (existing.isAlive() && 
                    wall.getRect().findIntersection(existing.getRect()).has_value()) {
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                walls.push_back(wall);
            }

        } catch (const std::exception& e) {
            std::cerr << "解析墙壁条目失败: " << e.what() << std::endl;
        }
    }
}

std::string MapLoader::getMapName(const std::string& jsonFile) {
    try {
        std::ifstream file(jsonFile);
        if (!file.is_open()) return "";

        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        json j = json::parse(content);
        if (j.contains("name")) {
            return j["name"].get<std::string>();
        }

    } catch (const std::exception& e) {
        // 忽略错误
    }

    // 返回文件名（不含扩展名）
    fs::path path(jsonFile);
    return path.stem().string();
}

std::vector<std::string> MapLoader::findMaps(const std::string& directory) {
    std::vector<std::string> maps;

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return maps;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".json") continue;

        maps.push_back(entry.path().string());
    }

    return maps;
}

int MapLoader::loadAllMaps(const std::string& directory, std::vector<Wall>& walls) {
    auto mapFiles = findMaps(directory);
    int loaded = 0;

    // 先保存现有的墙壁（边界等）
    std::vector<Wall> existingWalls = walls;

    for (const auto& file : mapFiles) {
        // 临时存储加载的墙壁
        std::vector<Wall> tempWalls;
        if (loadMap(file, tempWalls)) {
            // 合并到目标列表
            for (auto& wall : tempWalls) {
                // 检查是否与现有墙壁重叠
                bool overlap = false;
                for (const auto& existing : existingWalls) {
                    if (existing.isAlive() && 
                        wall.getRect().findIntersection(existing.getRect()).has_value()) {
                        overlap = true;
                        break;
                    }
                }
                // 也检查与已添加的新墙壁重叠
                if (!overlap) {
                    for (const auto& added : walls) {
                        if (added.isAlive() && 
                            wall.getRect().findIntersection(added.getRect()).has_value()) {
                            overlap = true;
                            break;
                        }
                    }
                }
                if (!overlap) {
                    walls.push_back(wall);
                }
            }
            loaded++;
        }
    }

    return loaded;
}

// ============================================================
// 手动解析（兼容 Java 版本，不依赖 nlohmann/json）
// ============================================================

bool MapLoader::parseMapJsonManual(const std::string& json, std::vector<Wall>& walls) {
    try {
        // 解析 bricks
        size_t brickStart = json.find("\"bricks\"");
        if (brickStart != std::string::npos) {
            size_t arrayStart = json.find('[', brickStart);
            if (arrayStart != std::string::npos) {
                int arrayEnd = findMatchingBracket(json, (int)arrayStart);
                if (arrayEnd != -1) {
                    std::string bricksJson = json.substr(arrayStart + 1, 
                                                         arrayEnd - arrayStart - 1);
                    parseWallArrayManual(bricksJson, walls, false);
                }
            }
        }

        // 解析 steels
        size_t steelStart = json.find("\"steels\"");
        if (steelStart != std::string::npos) {
            size_t arrayStart = json.find('[', steelStart);
            if (arrayStart != std::string::npos) {
                int arrayEnd = findMatchingBracket(json, (int)arrayStart);
                if (arrayEnd != -1) {
                    std::string steelsJson = json.substr(arrayStart + 1, 
                                                         arrayEnd - arrayStart - 1);
                    parseWallArrayManual(steelsJson, walls, true);
                }
            }
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "手动解析地图JSON失败: " << e.what() << std::endl;
        return false;
    }
}

void MapLoader::parseWallArrayManual(const std::string& json, 
                                     std::vector<Wall>& walls, 
                                     bool isSteel) {
    size_t pos = 0;
    while (true) {
        size_t start = json.find('[', pos);
        if (start == std::string::npos) break;

        int end = findMatchingBracket(json, (int)start);
        if (end == -1) break;

        std::string wallStr = json.substr(start + 1, end - start - 1);
        
        // 解析数字
        std::vector<int> values;
        size_t numStart = 0;
        while (numStart < wallStr.length()) {
            // 跳过空白和逗号
            while (numStart < wallStr.length() && 
                   (wallStr[numStart] == ' ' || wallStr[numStart] == ',' || 
                    wallStr[numStart] == '\t' || wallStr[numStart] == '\n' || 
                    wallStr[numStart] == '\r')) {
                numStart++;
            }
            if (numStart >= wallStr.length()) break;

            size_t numEnd = numStart;
            while (numEnd < wallStr.length() && 
                   (isdigit(wallStr[numEnd]) || wallStr[numEnd] == '-')) {
                numEnd++;
            }

            if (numEnd > numStart) {
                try {
                    values.push_back(std::stoi(wallStr.substr(numStart, numEnd - numStart)));
                } catch (...) {
                    // 忽略解析错误
                }
            }
            numStart = numEnd;
        }

        if (values.size() >= 4) {
            try {
                int col = values[0];
                int row = values[1];
                int w = values[2];
                int h = values[3];

                int x = GRID_OFFSET_X + col * CELL_SIZE;
                int y = GRID_OFFSET_Y + row * CELL_SIZE;

                Wall wall(x, y, w * CELL_SIZE, h * CELL_SIZE, isSteel);

                // 检查是否与现有墙壁重叠
                bool overlap = false;
                for (const auto& existing : walls) {
                    if (existing.isAlive() && 
                        wall.getRect().findIntersection(existing.getRect()).has_value()) {
                        overlap = true;
                        break;
                    }
                }
                if (!overlap) {
                    walls.push_back(wall);
                }

            } catch (const std::exception& e) {
                // 跳过无效条目
            }
        }

        pos = end + 1;
    }
}

int MapLoader::findMatchingBracket(const std::string& str, int start) {
    if (start < 0 || start >= (int)str.length()) return -1;

    char open = str[start];
    char close = (open == '[') ? ']' : '}';

    int depth = 0;
    bool inString = false;

    for (int i = start; i < (int)str.length(); ++i) {
        char c = str[i];

        if (c == '"' && (i == 0 || str[i-1] != '\\')) {
            inString = !inString;
            continue;
        }

        if (inString) continue;

        if (c == open) {
            depth++;
        } else if (c == close) {
            depth--;
            if (depth == 0) {
                return i;
            }
        }
    }

    return -1;
}