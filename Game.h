#pragma once
#include <vector>
#include <random>
#include "Item.h"
#include "Shell.h"
#include "Shotgun.h"

// ─── DealerAI: Expected Value-based decision making ──────────────────────────
// Returns a decision each turn: either USE_ITEM(type) or SHOOT_SELF / SHOOT_PLAYER
struct DealerAction {
    enum class Type { SHOOT_SELF, SHOOT_PLAYER, USE_ITEM };
    Type     actionType;
    ItemType item = ItemType::NONE;
};

class DealerAI {
public:
    // Decide what the dealer should do this turn
    static DealerAction decide(
        std::vector<ItemType>& dealerItems,
        std::vector<ItemType>& playerItems,
        const Shotgun& gun,
        int dealerHP, int playerHP,
        std::mt19937& rng);

    // When adrenaline was used: pick best item to steal from player
    static ItemType chooseStealTarget(const std::vector<ItemType>& playerItems);
};
