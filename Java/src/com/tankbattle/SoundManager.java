package com.tankbattle;

import javax.sound.sampled.*;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import static com.tankbattle.Utils.*;

/**
 * 声音管理器 - 单例模式
 * v1.7 音效系统
 */
public class SoundManager {

    private static SoundManager instance;
    private Map<String, Clip> sounds;
    private float volume = SOUND_VOLUME;
    private boolean muted = SOUND_MUTED;
    private boolean initialized = false;

    private SoundManager() {
        sounds = new HashMap<>();
        initAudio();
    }

    public static SoundManager getInstance() {
        if (instance == null) {
            instance = new SoundManager();
        }
        return instance;
    }

    private void initAudio() {
        try {
            for (String name : SOUND_NAMES) {
                loadSound(name);
            }
            initialized = true;
            System.out.println("🎵 声音系统初始化成功");
        } catch (Exception e) {
            System.err.println("⚠️ 声音系统初始化失败: " + e.getMessage());
            initialized = false;
        }
    }

    private void loadSound(String name) {
        try {
            String path = SOUND_PATH + name + ".wav";
            URL url = SoundManager.class.getResource(path);
            
            if (url == null) {
                java.io.File file = new java.io.File("assets/sounds/" + name + ".wav");
                if (file.exists()) {
                    url = file.toURI().toURL();
                }
            }
            
            if (url == null) {
                System.err.println("⚠️ 音效文件不存在: " + name + ".wav");
                return;
            }

            AudioInputStream audioIn = AudioSystem.getAudioInputStream(url);
            Clip clip = AudioSystem.getClip();
            clip.open(audioIn);
            
            setClipVolume(clip, muted ? 0 : volume);
            
            sounds.put(name, clip);
            
        } catch (UnsupportedAudioFileException | IOException | LineUnavailableException e) {
            System.err.println("⚠️ 加载音效失败 " + name + ": " + e.getMessage());
        }
    }

    public void play(String name) {
        if (!initialized || muted || !SOUND_ENABLED) {
            return;
        }

        Clip clip = sounds.get(name);
        if (clip != null) {
            if (clip.isRunning()) {
                clip.stop();
            }
            clip.setFramePosition(0);
            clip.start();
        }
    }

    public void setVolume(float volume) {
        this.volume = Math.max(0.0f, Math.min(1.0f, volume));
        SOUND_VOLUME = this.volume;
        float vol = muted ? 0 : this.volume;
        for (Clip clip : sounds.values()) {
            setClipVolume(clip, vol);
        }
    }

    public boolean toggleMute() {
        this.muted = !this.muted;
        SOUND_MUTED = this.muted;
        float vol = muted ? 0 : volume;
        for (Clip clip : sounds.values()) {
            setClipVolume(clip, vol);
        }
        return muted;
    }

    public boolean isMuted() {
        return muted;
    }

    public void setEnabled(boolean enabled) {
        SOUND_ENABLED = enabled;
        if (!enabled) {
            for (Clip clip : sounds.values()) {
                if (clip != null && clip.isRunning()) {
                    clip.stop();
                }
            }
        }
    }

    public boolean isEnabled() {
        return SOUND_ENABLED;
    }

    public float getVolume() {
        return volume;
    }

    private void setClipVolume(Clip clip, float volume) {
        if (clip == null) return;
        
        float gain = volume > 0 ? (float) (Math.log10(volume) * 20) : -80.0f;
        
        try {
            FloatControl gainControl = (FloatControl) clip.getControl(FloatControl.Type.MASTER_GAIN);
            if (gainControl != null) {
                float min = gainControl.getMinimum();
                float max = gainControl.getMaximum();
                float clampedGain = gain;  // 使用新变量
                if (clampedGain < min) clampedGain = min;
                if (clampedGain > max) clampedGain = max;
                gainControl.setValue(clampedGain);
            }
        } catch (IllegalArgumentException e) {
            // 忽略
        }
    }

    public void dispose() {
        for (Clip clip : sounds.values()) {
            if (clip != null) {
                clip.close();
            }
        }
        sounds.clear();
        initialized = false;
    }

    public void reloadSounds() {
        dispose();
        sounds.clear();
        initAudio();
    }
}