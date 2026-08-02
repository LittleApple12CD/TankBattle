#pragma once
#include <string>

class SaveManager {
public:
    static void ensureSaveDir();
    static void saveProgress(int level);
    static int loadProgress();
    static bool hasSave();

private:
    static const std::string SAVE_DIR;
    static const std::string SAVE_FILE;
    static std::string getSavePath();
};