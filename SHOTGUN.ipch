#include "Utils.h"

sf::String utf8(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

// ─── Easing ──────────────────────────────────────────────────────────────────
namespace Easing {
    float linear(float t) { return t; }
    float easeInQuad(float t) { return t * t; }
    float easeOutQuad(float t) { return t * (2.f - t); }
    float easeInOutQuad(float t) {
        return t < 0.5f ? 2.f * t * t : -1.f + (4.f - 2.f * t) * t;
    }
    float easeOutCubic(float t) {
        float f = t - 1.f;
        return f * f * f + 1.f;
    }
    float easeInCubic(float t) { return t * t * t; }
    float easeOutElastic(float t) {
        if (t == 0.f || t == 1.f) return t;
        float p = 0.3f;
        return std::pow(2.f, -10.f * t) *
               std::sin((t - p / 4.f) * (2.f * 3.14159265f) / p) + 1.f;
    }
    float easeOutBack(float t) {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.f;
        return 1.f + c3 * std::pow(t - 1.f, 3.f) + c1 * std::pow(t - 1.f, 2.f);
    }
    float easeInOutSine(float t) {
        return -(std::cos(3.14159265f * t) - 1.f) / 2.f;
    }
}

// ─── Lerp ────────────────────────────────────────────────────────────────────
float lerp(float a, float b, float t) { return a + (b - a) * t; }

sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
    return sf::Color(
        static_cast<sf::Uint8>(lerp(a.r, b.r, t)),
        static_cast<sf::Uint8>(lerp(a.g, b.g, t)),
        static_cast<sf::Uint8>(lerp(a.b, b.b, t)),
        static_cast<sf::Uint8>(lerp(a.a, b.a, t))
    );
}

// ─── Font search ─────────────────────────────────────────────────────────────
bool loadFont(sf::Font& font) {
    static const std::vector<std::string> candidates = {
        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/truetype/open-sans/OpenSans-Regular.ttf",
        // macOS
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        // Windows
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
    };
    for (const auto& path : candidates)
        if (font.loadFromFile(path)) return true;
    return false;
}

// ─── Draw rounded rect ───────────────────────────────────────────────────────
sf::RectangleShape makeRoundedRect(sf::Vector2f size, sf::Color fill,
                                   sf::Color outline, float thickness) {
    sf::RectangleShape r(size);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(thickness);
    return r;
}

// ─── Clamp ───────────────────────────────────────────────────────────────────
float clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }
int   clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// ─── Random int in [lo, hi] ──────────────────────────────────────────────────
int randInt(std::mt19937& rng, int lo, int hi) {
    return std::uniform_int_distribution<int>(lo, hi)(rng);
}
