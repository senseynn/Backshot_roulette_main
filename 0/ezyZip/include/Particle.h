#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float lifetime;  // seconds remaining
    float maxLife;
    float radius;
    sf::Color color;
    sf::Color endColor;
    float gravity;
    float drag;      // velocity multiplier per frame

    Particle(sf::Vector2f p, sf::Vector2f v, float life, float r,
             sf::Color c, sf::Color ec = sf::Color::Transparent,
             float grav = 80.f, float d = 0.98f);

    bool alive() const;
    void update(float dt);
    void draw(sf::RenderTarget& rt) const;
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void update(float dt);
    void draw(sf::RenderTarget& rt) const;

    void spawnGunshot(sf::Vector2f pos, std::mt19937& rng);
    void spawnShellEject(sf::Vector2f pos, std::mt19937& rng);
    void spawnHit(sf::Vector2f pos, std::mt19937& rng);
    void spawnSparkle(sf::Vector2f pos, sf::Color col, std::mt19937& rng);
    void clear();
};
