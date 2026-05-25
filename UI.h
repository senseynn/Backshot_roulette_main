#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>

class SoundManager {
public:
    enum class SFX {
        GUNSHOT,
        BLANK_SHOT,
        SHELL_EJECT,
        ITEM_USE,
        HEAL,
        HANDCUFFS,
        REVEAL,
        CLICK,
        ROUND_WIN,
        ROUND_LOSE,
        BEER,
        PHONE
    };

    static constexpr unsigned SAMPLE_RATE = 44100;

    SoundManager();

    void play(SFX sfx);
    void setMasterVolume(float v);

private:
    std::vector<std::shared_ptr<sf::SoundBuffer>> buffers_;
    std::vector<sf::Sound> sounds_;
    float masterVolume_ = 1.f;

    // ── Synthesis helpers ──────────────────────────────────────────────────
    static std::vector<sf::Int16> makeSilence(float seconds);
    static std::vector<sf::Int16> makeNoiseBurst(float decayTime, float amp = 0.9f, bool lowPass = false);
    static std::vector<sf::Int16> makeTone(float freq, float duration,
                                           float attack = 0.01f, float decay = 0.1f,
                                           float amp = 0.5f);
    static std::vector<sf::Int16> makeClick(float dur = 0.04f);
    static std::vector<sf::Int16> makeSweep(float f0, float f1, float dur);
    static std::vector<sf::Int16> makeDing(float freq, float dur = 0.3f);
    static std::vector<sf::Int16> makeWin();
    static std::vector<sf::Int16> makeLose();
    static std::vector<sf::Int16> makeBeer();
    static std::vector<sf::Int16> makePhone();
    static std::vector<sf::Int16> makeHandcuffs();

    void addBuffer(const std::vector<sf::Int16>& samples);
    void generateSounds();
};
