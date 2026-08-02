#include "SaveManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

const std::string SaveManager::SAVE_DIR = "PlayerData";
const std::string SaveManager::SAVE_FILE = "LevelSaves.dat";

void SaveManager::ensureSaveDir() {
    struct stat st;
    if (stat(SAVE_DIR.c_str(), &st) != 0) {
        // 目录不存在，创建
        if (MKDIR(SAVE_DIR.c_str()) != 0) {
            std::cerr << "创建存档目录失败: " << SAVE_DIR << std::endl;
        }
    }
}

std::string SaveManager::getSavePath() {
    return SAVE_DIR + "/" + SAVE_FILE;
}

void SaveManager::saveProgress(int level) {
    ensureSaveDir();
    std::string path = getSavePath();
    
    std::ofstream file(path, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&level), sizeof(level));
        file.close();
    } else {
        std::cerr << "保存失败: " << path << std::endl;
    }
}

int SaveManager::loadProgress() {
    ensureSaveDir();
    std::string path = getSavePath();
    
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {
        int level = 1;
        file.read(reinterpret_cast<char*>(&level), sizeof(level));
        file.close();
        return level;
    }
    return 1;  // 没有存档，从第1关开始
}

bool SaveManager::hasSave() {
    std::string path = getSavePath();
    std::ifstream file(path);
    return file.good();
}