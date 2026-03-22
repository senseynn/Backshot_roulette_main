#pragma once

#include "Player.h"
#include "Shotgun.h"

class AI {
public:
  struct Action {
    enum Type { UseItem, ShootSelf, ShootOpponent };
    Type type;
    int itemIndex; // РµСЃР»Рё type == UseItem
  };

  Action decide(const Player& aiPlayer, const Player& humanPlayer,
    const Shotgun& shotgun, bool nextShotDoubleDamage,
    bool opponentSkipNextTurn);
};