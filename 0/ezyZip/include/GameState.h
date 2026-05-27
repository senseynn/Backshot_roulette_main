#pragma once

enum class GameState {
    MAIN_MENU,
    ROUND_START,     // Dealing items & shells
    PLAYER_TURN,     // Player choosing action
    DEALER_TURN,     // AI taking action
    SHOOT_SELF,      // Animation: shooting self
    SHOOT_OPPONENT,  // Animation: shooting opponent
    ITEM_USE,        // Item use animation
    ROUND_END,       // Round over, showing result
    GAME_OVER        // Best of 3 ended
};

enum class Turn {
    PLAYER,
    DEALER
};

enum class RoundResult {
    NONE,
    PLAYER_WIN,
    DEALER_WIN
};
