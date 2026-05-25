#include "DealerAI.h"
#include <algorithm>

DealerAction DealerAI::decide(
    std::vector<ItemType>& dealerItems,
    std::vector<ItemType>& playerItems,
    const Shotgun& gun,
    int dealerHP, int playerHP,
    std::mt19937& rng)
{
    int total = gun.total();
    int live  = gun.liveRemaining();
    int blank = gun.blankRemaining();
    if (total == 0)
        return {DealerAction::Type::SHOOT_PLAYER};

    float pLive  = (float)live  / total;
    float pBlank = (float)blank / total;
    int   dmg    = gun.damage();

    // ── Item priority logic ──────────────────────────────────────────────

    // 1. Use cigarette if HP < max and it helps
    if (dealerHP < 4) {
        auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::CIGARETTE);
        if (it != dealerItems.end()) {
            dealerItems.erase(it);
            return {DealerAction::Type::USE_ITEM, ItemType::CIGARETTE};
        }
    }

    // 2. Use handcuffs if player is dangerous and not already cuffed
    if (playerHP <= 2 || (playerHP <= 3 && pLive > 0.5f)) {
        auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::HANDCUFFS);
        if (it != dealerItems.end()) {
            dealerItems.erase(it);
            return {DealerAction::Type::USE_ITEM, ItemType::HANDCUFFS};
        }
    }

    // 3. Use magnifying glass if top shell unknown and we're uncertain
    if (pLive > 0.25f && pLive < 0.75f && total > 1) {
        if (!gun.chambers.empty() && !gun.chambers.front().known) {
            auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::MAGNIFYING_GLASS);
            if (it != dealerItems.end()) {
                dealerItems.erase(it);
                return {DealerAction::Type::USE_ITEM, ItemType::MAGNIFYING_GLASS};
            }
        }
    }

    // 4. Use knife if we're about to shoot player and live is likely
    if (pLive >= 0.5f) {
        auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::KNIFE);
        if (it != dealerItems.end()) {
            dealerItems.erase(it);
            return {DealerAction::Type::USE_ITEM, ItemType::KNIFE};
        }
    }

    // 5. Beer: eject if top is likely blank (save time, cycle to live) OR we know top is blank
    bool topKnownBlank = (!gun.chambers.empty() &&
                          gun.chambers.front().known &&
                          gun.chambers.front().isBlank());
    if (topKnownBlank || pLive < 0.2f) {
        auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::BEER);
        if (it != dealerItems.end()) {
            dealerItems.erase(it);
            return {DealerAction::Type::USE_ITEM, ItemType::BEER};
        }
    }

    // 6. Adrenaline: steal player's knife/handcuffs if they have one
    auto hasDangerous = [&](ItemType t) {
        return std::find(playerItems.begin(), playerItems.end(), t) != playerItems.end();
    };
    if (hasDangerous(ItemType::KNIFE) || hasDangerous(ItemType::HANDCUFFS)) {
        auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::ADRENALINE);
        if (it != dealerItems.end()) {
            dealerItems.erase(it);
            return {DealerAction::Type::USE_ITEM, ItemType::ADRENALINE};
        }
    }

    // 7. Phone if many unknowns and we have it
    if (total >= 4 && live > 0) {
        int knownCount = 0;
        for (auto& s : gun.chambers) if (s.known) ++knownCount;
        if (knownCount < total / 2) {
            auto it = std::find(dealerItems.begin(), dealerItems.end(), ItemType::PHONE);
            if (it != dealerItems.end()) {
                dealerItems.erase(it);
                return {DealerAction::Type::USE_ITEM, ItemType::PHONE};
            }
        }
    }

    // ── Shoot decision (EV-based) ────────────────────────────────────────

    bool topKnownLive = (!gun.chambers.empty() &&
                         gun.chambers.front().known &&
                         gun.chambers.front().isLive());

    if (topKnownLive)  return {DealerAction::Type::SHOOT_PLAYER};
    if (topKnownBlank) return {DealerAction::Type::SHOOT_SELF};

    // EV: prefer self-shoot if pBlank > 0.5 (free turn)
    if (pBlank > 0.5f)
        return {DealerAction::Type::SHOOT_SELF};

    return {DealerAction::Type::SHOOT_PLAYER};
}

ItemType DealerAI::chooseStealTarget(const std::vector<ItemType>& playerItems) {
    static const std::vector<ItemType> priority = {
        ItemType::KNIFE, ItemType::HANDCUFFS, ItemType::CIGARETTE,
        ItemType::MAGNIFYING_GLASS, ItemType::BEER, ItemType::PHONE, ItemType::ADRENALINE
    };
    for (auto& want : priority) {
        auto it = std::find(playerItems.begin(), playerItems.end(), want);
        if (it != playerItems.end()) return want;
    }
    return ItemType::NONE;
}
