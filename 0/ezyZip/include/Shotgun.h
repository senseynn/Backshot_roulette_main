#pragma once
#include <vector>
#include <random>
#include <algorithm>
#include "Shell.h"

class Shotgun {
public:
    std::vector<Shell> chambers;
    int liveCount = 0;
    int blankCount = 0;
    bool knifeActive = false;   // double damage next shot
    bool handcuffsUsed = false;

    void load(int live, int blank, std::mt19937& rng);

    bool empty() const;
    int total() const;
    int liveRemaining() const;
    int blankRemaining() const;

    // Peek at top shell (magnifying glass)
    ShellType peek() const;

    // Mark top shell as known
    void revealTop();

    // Reveal a random unknown shell (phone)
    int revealRandom(std::mt19937& rng);

    // Fire: returns shell type, removes from front
    ShellType fire();

    // Eject (beer): remove front shell without firing
    ShellType eject();

    // Damage dealt by next shot
    int damage() const;
};
