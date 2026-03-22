#include "AI.h"
#include <vector>
#include <cstdlib>

AI::Action AI::decide(const Player& aiPlayer, const Player& humanPlayer,
  const Shotgun& shotgun, bool nextShotDoubleDamage,
  bool opponentSkipNextTurn) {
  const auto& items = aiPlayer.getItems();

  bool hasApple = aiPlayer.hasItem(ItemType::Apple);
  bool hasMagnifier = aiPlayer.hasItem(ItemType::Magnifier);
  bool hasSaw = aiPlayer.hasItem(ItemType::Saw);
  bool hasSoda = aiPlayer.hasItem(ItemType::Soda);
  bool hasHandcuffs = aiPlayer.hasItem(ItemType::Handcuffs);

  if (hasApple && aiPlayer.getHealth() < aiPlayer.getMaxHealth() && aiPlayer.getHealth() <= 2) {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Apple) {
        return { Action::UseItem, static_cast<int>(i) };
      }
    }
  }

  if (hasMagnifier) {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Magnifier) {
        return { Action::UseItem, static_cast<int>(i) };
      }
    }
  }

  if (hasSaw && !nextShotDoubleDamage) {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Saw) {
        return { Action::UseItem, static_cast<int>(i) };
      }
    }
  }

  if (hasSoda && !shotgun.isEmpty() && shotgun.nextShell() == ShellType::Live) {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Soda) {
        return { Action::UseItem, static_cast<int>(i) };
      }
    }
  }

  if (hasHandcuffs && !opponentSkipNextTurn) {
    for (size_t i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Handcuffs) {
        return { Action::UseItem, static_cast<int>(i) };
      }
    }
  }

  if (!shotgun.isEmpty()) {
    if (shotgun.nextShell() == ShellType::Live) {
      return { Action::ShootOpponent, 0 };
    }
    else {
      return { Action::ShootSelf, 0 };
    }
  }
  else {
    return { Action::ShootSelf, 0 };
  }
}