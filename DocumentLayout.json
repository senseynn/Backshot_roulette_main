#include "Item.h"

const char* itemName(ItemType t) {
    switch (t) {
        case ItemType::BEER:             return "Пиво";
        case ItemType::MAGNIFYING_GLASS: return "Лупа";
        case ItemType::CIGARETTE:        return "Сигарета";
        case ItemType::KNIFE:            return "Нож";
        case ItemType::HANDCUFFS:        return "Наручники";
        case ItemType::ADRENALINE:       return "Адреналин";
        case ItemType::PHONE:            return "Телефон";
        default:                         return "Пусто";
    }
}

const char* itemDesc(ItemType t) {
    switch (t) {
        case ItemType::BEER:             return "Выбить патрон";
        case ItemType::MAGNIFYING_GLASS: return "Проверить патрон";
        case ItemType::CIGARETTE:        return "+1 жизнь";
        case ItemType::KNIFE:            return "Урон x2";
        case ItemType::HANDCUFFS:        return "Пропуск хода";
        case ItemType::ADRENALINE:       return "Украсть предмет";
        case ItemType::PHONE:            return "Случайный патрон";
        default:                         return "";
    }
}

ItemType randomItem(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, ITEM_POOL_SIZE - 1);
    return static_cast<ItemType>(dist(rng));
}

std::vector<ItemType> dealItems(std::mt19937& rng, int count) {
    std::vector<ItemType> items;
    items.reserve(count);
    for (int i = 0; i < count; ++i)
        items.push_back(randomItem(rng));
    return items;
}
