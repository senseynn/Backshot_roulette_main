#include "Player.h"
#include <algorithm>

Player::Player() : health(0), maxHealth(0) {}

Player::Player(int maxHealth) : health(maxHealth), maxHealth(maxHealth) {}

void Player::takeDamage(int damage) {
  health -= damage;
  if (health < 0) health = 0;
}

void Player::heal(int amount) {
  health += amount;
  if (health > maxHealth) health = maxHealth;
}

bool Player::isAlive() const {
  return health > 0;
}

int Player::getHealth() const {
  return health;
}

int Player::getMaxHealth() const {
  return maxHealth;
}

void Player::resetHealth() {
  health = maxHealth;
}

void Player::addItem(ItemType type) {
  items.push_back({ type });
}

bool Player::hasItem(ItemType type) const {
  return std::find_if(items.begin(), items.end(),
    [type](const Item& item) { return item.type == type; }) != items.end();
}

void Player::removeItem(ItemType type) {
  auto it = std::find_if(items.begin(), items.end(),
    [type](const Item& item) { return item.type == type; });
  if (it != items.end()) {
    items.erase(it);
  }
}

const std::vector<Item>& Player::getItems() const {
  return items;
}