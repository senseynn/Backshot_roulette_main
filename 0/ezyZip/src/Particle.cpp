#include "Particle.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>

// ─── Particle ────────────────────────────────────────────────────────────────

Particle::Particle(sf::Vector2f p, sf::Vector2f v, float life, float r,
                   sf::Color c, sf::Color ec, float grav, float d)
    : pos(p), vel(v), lifetime(life), maxLife(life),
      radius(r), color(c), endColor(ec), gravity(grav), drag(d) {}

bool Particle::alive() const { return lifetime > 0.f; }

void Particle::update(float dt) {
    vel.y += gravity * dt;
    vel.x *= drag;
    vel.y *= drag;
    pos += vel * dt;
    lifetime -= dt;
}

void Particle::draw(sf::RenderTarget& rt) const {
    float t = clamp01(1.f - lifetime / maxLife);
    sf::Color c = lerpColor(color, endColor, t);
    float r = radius * (1.f - t * 0.5f);
    sf::CircleShape cs(r);
    cs.setFillColor(c);
    cs.setOrigin(r, r);
    cs.setPosition(pos);
    rt.draw(cs);
}

// ─── ParticleSystem ──────────────────────────────────────────────────────────

void ParticleSystem::update(float dt) {
    for (auto& p : particles) p.update(dt);
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p) { return !p.alive(); }),
        particles.end());
}

void ParticleSystem::draw(sf::RenderTarget& rt) const {
    for (const auto& p : particles) p.draw(rt);
}

void ParticleSystem::spawnGunshot(sf::Vector2f pos, std::mt19937& rng) {
    std::uniform_real_distribution<float> angle(0.f, 360.f);
    std::uniform_real_distribution<float> speed(80.f, 260.f);
    std::uniform_real_distribution<float> life(0.25f, 0.6f);
    std::uniform_real_distribution<float> r(2.f, 6.f);
    int n = 24;
    for (int i = 0; i < n; ++i) {
        float a = angle(rng) * 3.14159265f / 180.f;
        float s = speed(rng);
        sf::Vector2f vel(std::cos(a) * s, std::sin(a) * s);
        particles.emplace_back(pos, vel, life(rng), r(rng),
                               sf::Color(255, 220, 60, 255),
                               sf::Color(200, 40, 0, 0), 120.f, 0.94f);
    }
    // Smoke
    for (int i = 0; i < 8; ++i) {
        float a = angle(rng) * 3.14159265f / 180.f;
        float s = speed(rng) * 0.3f;
        sf::Vector2f v(std::cos(a) * s, std::sin(a) * s - 30.f);
        particles.emplace_back(pos, v, life(rng) + 0.3f, r(rng) * 2.f,
                               sf::Color(180, 180, 180, 180),
                               sf::Color(100, 100, 100, 0), -20.f, 0.97f);
    }
}

void ParticleSystem::spawnShellEject(sf::Vector2f pos, std::mt19937& rng) {
    std::uniform_real_distribution<float> life(0.4f, 0.8f);
    for (int i = 0; i < 6; ++i) {
        float angle = (-60.f + i * 20.f) * 3.14159265f / 180.f;
        float s = 90.f + i * 15.f;
        sf::Vector2f v(std::cos(angle) * s, std::sin(angle) * s);
        particles.emplace_back(pos, v, life(rng), 3.f,
                               sf::Color(200, 160, 40, 220),
                               sf::Color(120, 80, 10, 0), 200.f, 0.92f);
    }
}

void ParticleSystem::spawnHit(sf::Vector2f pos, std::mt19937& rng) {
    std::uniform_real_distribution<float> angle(0.f, 360.f);
    std::uniform_real_distribution<float> speed(50.f, 200.f);
    std::uniform_real_distribution<float> life(0.3f, 0.7f);
    for (int i = 0; i < 18; ++i) {
        float a = angle(rng) * 3.14159265f / 180.f;
        float s = speed(rng);
        sf::Vector2f v(std::cos(a) * s, std::sin(a) * s);
        particles.emplace_back(pos, v, life(rng), 3.f + (rng() % 4),
                               sf::Color(180, 20, 10, 255),
                               sf::Color(80, 0, 0, 0), 150.f, 0.93f);
    }
}

void ParticleSystem::spawnSparkle(sf::Vector2f pos, sf::Color col, std::mt19937& rng) {
    std::uniform_real_distribution<float> angle(0.f, 360.f);
    std::uniform_real_distribution<float> speed(30.f, 120.f);
    std::uniform_real_distribution<float> life(0.4f, 0.9f);
    for (int i = 0; i < 14; ++i) {
        float a = angle(rng) * 3.14159265f / 180.f;
        float s = speed(rng);
        sf::Vector2f v(std::cos(a) * s, std::sin(a) * s);
        particles.emplace_back(pos, v, life(rng), 2.f + (rng() % 3),
                               col, sf::Color(col.r, col.g, col.b, 0), 0.f, 0.96f);
    }
}

void ParticleSystem::clear() { particles.clear(); }
