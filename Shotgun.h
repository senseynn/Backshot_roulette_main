#pragma once

#include <vector>
#include <random>

enum class ShellType {
  Live,
  Blank
};

class Shotgun {
public:
  Shotgun();
  void load(int numShells, int numLive);
  ShellType nextShell() const;
  ShellType fire();
  bool isEmpty() const;
  int getShellsCount() const;
  void discardCurrent();

private:
  std::vector<ShellType> shells;
  int currentIndex;
  std::mt19937 rng;
};