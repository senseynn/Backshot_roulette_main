#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <random>

sf::String utf8(const std::string& s);

// ─── Easing functions ────────────────────────────────────────────────────────
namespace Easing {
    float linear(float t);
    float easeInQuad(float t);
    float easeOutQuad(float t);
    float easeInOutQuad(float t);
    float easeOutCubic(float t);
    float easeInCubic(float t);
    float easeOutElastic(float t);
    float easeOutBack(float t);
    float easeInOutSine(float t);
}

// ─── Lerp ────────────────────────────────────────────────────────────────────
float lerp(float a, float b, float t);
sf::Color lerpColor(sf::Color a, sf::Color b, float t);

// ─── Font search ─────────────────────────────────────────────────────────────
bool loadFont(sf::Font& font);

// ─── Draw rounded rect ───────────────────────────────────────────────────────
sf::RectangleShape makeRoundedRect(sf::Vector2f size, sf::Color fill,
                                   sf::Color outline = sf::Color::Transparent,
                                   float thickness = 0.f);

// ─── Clamp ───────────────────────────────────────────────────────────────────
float clamp01(float v);
int   clamp(int v, int lo, int hi);

// ─── Random int in [lo, hi] ──────────────────────────────────────────────────
int randInt(std::mt19937& rng, int lo, int hi);
