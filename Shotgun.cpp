#include "Shotgun.h"
#include <algorithm>
#include <random>

Shotgun::Shotgun() : currentIndex(0), rng(std::random_device{}()) {}

void Shotgun::load(int numShells, int numLive) {
  shells.clear();
  currentIndex = 0;
  for (int i = 0; i < numLive; ++i)
    shells.push_back(ShellType::Live);
  for (int i = 0; i < numShells - numLive; ++i)
    shells.push_back(ShellType::Blank);
  std::shuffle(shells.begin(), shells.end(), rng);
}

ShellType Shotgun::nextShell() const {
  if (isEmpty()) return ShellType::Blank;
  return shells[currentIndex];
}

ShellType Shotgun::fire() {
  if (isEmpty()) return ShellType::Blank;
  return shells[currentIndex++];
}

bool Shotgun::isEmpty() const {
  return currentIndex >= (int)shells.size();
}

int Shotgun::getShellsCount() const {
  return static_cast<int>(shells.size() - currentIndex);
}

void Shotgun::discardCurrent() {
  if (!isEmpty()) {
    ++currentIndex;
  }
}