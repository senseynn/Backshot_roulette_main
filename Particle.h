#pragma once
#include <random>
#include <vector>
#include <string>

enum class ItemType {
    NONE = -1,
    BEER,             // Eject current shell without firing
    MAGNIFYING_GLASS, // Peek at current shell
    CIGARETTE,        // Heal 1 HP (up to max)
    KNIFE,            // Next shot deals 2 damage
    HANDCUFFS,        // Skip opponent's next turn
    ADRENALINE,       // Steal and use one of opponent's items
    PHONE             // Learn a random unknown shell's type
};

const char* itemName(ItemType t);
const char* itemDesc(ItemType t);

static constexpr int MAX_ITEMS = 8;
static constexpr int ITEM_POOL_SIZE = 7;

ItemType randomItem(std::mt19937& rng);
std::vector<ItemType> dealItems(std::mt19937& rng, int count);
