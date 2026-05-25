#include "SoundManager.h"
#include <cmath>
#include <cstdlib>

SoundManager::SoundManager() {
    generateSounds();
}

void SoundManager::play(SFX sfx) {
    int idx = static_cast<int>(sfx);
    if (idx < 0 || idx >= (int)buffers_.size()) return;
    for (auto& s : sounds_) {
        if (s.getStatus() == sf::Sound::Stopped) {
            s.setBuffer(*buffers_[idx]);
            s.setVolume(70.f);
            s.play();
            return;
        }
    }
    sounds_.emplace_back();
    sounds_.back().setBuffer(*buffers_[idx]);
    sounds_.back().setVolume(70.f);
    sounds_.back().play();
}

void SoundManager::setMasterVolume(float v) { masterVolume_ = v; }

// ── Synthesis helpers ─────────────────────────────────────────────────────────

std::vector<sf::Int16> SoundManager::makeSilence(float seconds) {
    int n = static_cast<int>(SAMPLE_RATE * seconds);
    return std::vector<sf::Int16>(n, 0);
}

std::vector<sf::Int16> SoundManager::makeNoiseBurst(float decayTime, float amp, bool lowPass) {
    int n = static_cast<int>(SAMPLE_RATE * decayTime);
    std::vector<sf::Int16> buf(n);
    float prev = 0.f;
    for (int i = 0; i < n; ++i) {
        float t = (float)i / n;
        float env = std::exp(-t * 8.f);
        float noise = ((rand() % 32768) / 16384.f) - 1.f;
        if (lowPass) {
            prev = prev * 0.6f + noise * 0.4f;
            noise = prev;
        }
        buf[i] = static_cast<sf::Int16>(noise * env * amp * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeTone(float freq, float duration,
                                               float attack, float decay, float amp) {
    int n = static_cast<int>(SAMPLE_RATE * duration);
    std::vector<sf::Int16> buf(n);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / SAMPLE_RATE;
        float env;
        if (t < attack) env = t / attack;
        else if (t < attack + decay) env = 1.f - (t - attack) / decay;
        else env = 0.f;
        float wave = std::sin(2.f * 3.14159265f * freq * t);
        buf[i] = static_cast<sf::Int16>(wave * env * amp * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeClick(float dur) {
    int n = static_cast<int>(SAMPLE_RATE * dur);
    std::vector<sf::Int16> buf(n);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / n;
        float env = std::exp(-t * 20.f);
        float v = ((rand() % 32768) / 16384.f - 1.f) * env;
        buf[i] = static_cast<sf::Int16>(v * 0.7f * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeSweep(float f0, float f1, float dur) {
    int n = static_cast<int>(SAMPLE_RATE * dur);
    std::vector<sf::Int16> buf(n);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / SAMPLE_RATE;
        float frac = (float)i / n;
        float freq = f0 + (f1 - f0) * frac;
        float env = std::exp(-frac * 6.f);
        float wave = std::sin(2.f * 3.14159265f * freq * t);
        buf[i] = static_cast<sf::Int16>(wave * env * 0.5f * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeDing(float freq, float dur) {
    int n = static_cast<int>(SAMPLE_RATE * dur);
    std::vector<sf::Int16> buf(n);
    for (int i = 0; i < n; ++i) {
        float t = (float)i / SAMPLE_RATE;
        float frac = (float)i / n;
        float env = std::exp(-frac * 5.f) * (1.f - std::exp(-frac * 20.f));
        float wave = std::sin(2.f * 3.14159265f * freq * t)
                   + 0.3f * std::sin(2.f * 3.14159265f * freq * 2.f * t);
        buf[i] = static_cast<sf::Int16>(wave * env * 0.5f * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeWin() {
    auto a = makeTone(440.f, 0.15f, 0.01f, 0.14f, 0.4f);
    auto b = makeTone(660.f, 0.3f,  0.01f, 0.29f, 0.5f);
    std::vector<sf::Int16> out;
    out.reserve(a.size() + b.size());
    out.insert(out.end(), a.begin(), a.end());
    out.insert(out.end(), b.begin(), b.end());
    return out;
}

std::vector<sf::Int16> SoundManager::makeLose() {
    auto a = makeTone(400.f, 0.2f,  0.01f, 0.19f, 0.4f);
    auto b = makeTone(280.f, 0.35f, 0.01f, 0.34f, 0.5f);
    std::vector<sf::Int16> out;
    out.reserve(a.size() + b.size());
    out.insert(out.end(), a.begin(), a.end());
    out.insert(out.end(), b.begin(), b.end());
    return out;
}

std::vector<sf::Int16> SoundManager::makeBeer() {
    int n = static_cast<int>(SAMPLE_RATE * 0.5f);
    std::vector<sf::Int16> buf(n);
    float prev = 0.f;
    for (int i = 0; i < n; ++i) {
        float t = (float)i / n;
        float noise = ((rand() % 32768) / 16384.f) - 1.f;
        prev = prev * 0.7f + noise * 0.3f;
        float env = (1.f - t) * std::exp(-t * 3.f);
        buf[i] = static_cast<sf::Int16>(prev * env * 0.6f * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makePhone() {
    int total = static_cast<int>(SAMPLE_RATE * 0.6f);
    std::vector<sf::Int16> buf(total, 0);
    for (int i = 0; i < total; ++i) {
        float t = (float)i / SAMPLE_RATE;
        float pulse = (std::fmod(t, 0.12f) < 0.06f) ? 1.f : 0.f;
        float wave = (std::sin(2.f * 3.14159265f * 880.f * t)
                    + std::sin(2.f * 3.14159265f * 960.f * t)) * 0.5f;
        buf[i] = static_cast<sf::Int16>(wave * pulse * 0.4f * 32767.f);
    }
    return buf;
}

std::vector<sf::Int16> SoundManager::makeHandcuffs() {
    auto c1 = makeClick(0.05f);
    std::vector<sf::Int16> gap(static_cast<int>(SAMPLE_RATE * 0.1f), 0);
    auto c2 = makeClick(0.05f);
    std::vector<sf::Int16> out;
    out.insert(out.end(), c1.begin(), c1.end());
    out.insert(out.end(), gap.begin(), gap.end());
    out.insert(out.end(), c2.begin(), c2.end());
    return out;
}

void SoundManager::addBuffer(const std::vector<sf::Int16>& samples) {
    auto buf = std::make_shared<sf::SoundBuffer>();
    buf->loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE);
    buffers_.push_back(buf);
}

void SoundManager::generateSounds() {
    srand(42); // deterministic
    // Order MUST match SFX enum
    addBuffer(makeNoiseBurst(0.4f, 0.95f, false)); // GUNSHOT
    addBuffer(makeNoiseBurst(0.18f, 0.5f, true));  // BLANK_SHOT
    addBuffer(makeSweep(400.f, 100.f, 0.25f));     // SHELL_EJECT
    addBuffer(makeDing(880.f, 0.25f));              // ITEM_USE
    addBuffer(makeDing(660.f, 0.35f));              // HEAL
    addBuffer(makeHandcuffs());                     // HANDCUFFS
    addBuffer(makeDing(1200.f, 0.2f));              // REVEAL
    addBuffer(makeClick(0.06f));                    // CLICK
    addBuffer(makeWin());                           // ROUND_WIN
    addBuffer(makeLose());                          // ROUND_LOSE
    addBuffer(makeBeer());                          // BEER
    addBuffer(makePhone());                         // PHONE
}
