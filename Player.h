#pragma once

#include <vector>
#include "Item.h"

class Player {
public:
  Player();                     // РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ
  Player(int maxHealth);        // РєРѕРЅСЃС‚СЂСѓРєС‚РѕСЂ СЃ РїР°СЂР°РјРµС‚СЂРѕРј

  void takeDamage(int damage);
  void heal(int amount);
  bool isAlive() const;
  int getHealth() const;
  int getMaxHealth() const;
  void resetHealth();

  void addItem(ItemType type);
  bool hasItem(ItemType type) const;
  void removeItem(ItemType type);
  const std::vector<Item>& getItems() const;

private:
  int health;
  int maxHealth;
  std::vector<Item> items;
};