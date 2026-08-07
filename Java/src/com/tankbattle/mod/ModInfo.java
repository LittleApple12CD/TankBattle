package com.tankbattle.mod;

public class ModInfo {
    private String name;
    private String version;
    private String gameVersion;
    private String mainClass;
    private boolean loaded;
    private String error;

    public ModInfo(String name, String version, String gameVersion, String mainClass) {
        this.name = name;
        this.version = version;
        this.gameVersion = gameVersion;
        this.mainClass = mainClass;
        this.loaded = false;
        this.error = null;
    }

    public String getName() { return name; }
    public String getVersion() { return version; }
    public String getGameVersion() { return gameVersion; }
    public String getMainClass() { return mainClass; }
    public boolean isLoaded() { return loaded; }
    public void setLoaded(boolean loaded) { this.loaded = loaded; }
    public String getError() { return error; }
    public void setError(String error) { this.error = error; }

    public boolean isDisabled() {
        return error != null && error.equals("已禁用");
    }

    @Override
    public String toString() {
        return name + " v" + version + " (需要游戏 " + gameVersion + ")";
    }
}