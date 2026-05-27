#include "Shotgun.h"

void Shotgun::load(int live, int blank, std::mt19937& rng) {
    chambers.clear();
    liveCount = live;
    blankCount = blank;
    for (int i = 0; i < live; ++i)  chambers.emplace_back(ShellType::LIVE);
    for (int i = 0; i < blank; ++i) chambers.emplace_back(ShellType::BLANK);
    std::shuffle(chambers.begin(), chambers.end(), rng);
    knifeActive = false;
}

bool Shotgun::empty() const { return chambers.empty(); }
int  Shotgun::total() const { return (int)chambers.size(); }

int Shotgun::liveRemaining() const {
    int c = 0;
    for (auto& s : chambers) if (s.isLive()) ++c;
    return c;
}

int Shotgun::blankRemaining() const {
    int c = 0;
    for (auto& s : chambers) if (s.isBlank()) ++c;
    return c;
}

ShellType Shotgun::peek() const {
    if (chambers.empty()) return ShellType::BLANK;
    return chambers.front().type;
}

void Shotgun::revealTop() {
    if (!chambers.empty()) chambers.front().known = true;
}

int Shotgun::revealRandom(std::mt19937& rng) {
    std::vector<int> unknown;
    for (int i = 0; i < (int)chambers.size(); ++i)
        if (!chambers[i].known) unknown.push_back(i);
    if (unknown.empty()) return -1;
    std::uniform_int_distribution<int> d(0, (int)unknown.size() - 1);
    int idx = unknown[d(rng)];
    chambers[idx].known = true;
    return idx;
}

ShellType Shotgun::fire() {
    if (chambers.empty()) return ShellType::BLANK;
    ShellType t = chambers.front().type;
    chambers.erase(chambers.begin());
    return t;
}

ShellType Shotgun::eject() {
    if (chambers.empty()) return ShellType::BLANK;
    ShellType t = chambers.front().type;
    chambers.erase(chambers.begin());
    return t;
}

int Shotgun::damage() const { return knifeActive ? 2 : 1; }
