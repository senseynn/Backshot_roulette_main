#include "Game.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : currentStage(1), player(2), ai(2), playerTurn(true),
playerDoubleDamage(false), aiDoubleDamage(false),
playerSkipNextTurn(false), aiSkipNextTurn(false),
magnifierUsedThisTurn(false), phase(GamePhase::StageIntro) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  resetForNewStage();
}

void Game::start() {}

bool Game::isGameOver() const {
  return phase == GamePhase::GameOver;
}

std::string Game::getGameStateText() const {
  std::stringstream ss;
  ss << "Stage: " << currentStage << "\n"
    << "Player HP: " << player.getHealth() << "/" << player.getMaxHealth() << "\n"
    << "AI HP: " << ai.getHealth() << "/" << ai.getMaxHealth() << "\n"
    << "Shells left: " << shotgun.getShellsCount() << "\n"
    << "Turn: " << (playerTurn ? "Player" : "AI") << "\n";
  return ss.str();
}

void Game::nextStage() {
  if (currentStage < 3) {
    currentStage++;
    resetForNewStage();
  }
  else {
    phase = GamePhase::GameOver;
    addLogMessage("You won the game! Congratulations!");
  }
}

void Game::resetForNewStage() {
  int maxHealth = (currentStage == 1) ? 2 : (currentStage == 2) ? 4 : 5;
  player = Player(maxHealth);
  ai = Player(maxHealth);
  player.resetHealth();
  ai.resetHealth();

  playerDoubleDamage = false;
  aiDoubleDamage = false;
  playerSkipNextTurn = false;
  aiSkipNextTurn = false;
  magnifierUsedThisTurn = false;
  playerTurn = true;
  phase = GamePhase::PlayerTurn;

  int totalShells = 4 + (currentStage * 2);
  int liveShells = totalShells / 2 + (rand() % (totalShells / 2));
  shotgun.load(totalShells, liveShells);

  if (currentStage >= 2) {
    giveItemsToPlayers();
  }

  addLogMessage("New stage " + std::to_string(currentStage) + "! Your health: " +
    std::to_string(maxHealth) + ", creature's health: " + std::to_string(maxHealth));
}

void Game::giveItemsToPlayers() {
  std::vector<ItemType> allItems = {
      ItemType::Soda, ItemType::Saw, ItemType::Handcuffs,
      ItemType::Magnifier, ItemType::Apple
  };
  for (int i = 0; i < 3; ++i) {
    int idx = rand() % allItems.size();
    player.addItem(allItems[idx]);
  }
  for (int i = 0; i < 3; ++i) {
    int idx = rand() % allItems.size();
    ai.addItem(allItems[idx]);
  }
  addLogMessage("You received 3 items. The creature also received items.");
}

void Game::playerShootSelf() {
  if (phase != GamePhase::PlayerTurn || !playerTurn) return;
  if (playerSkipNextTurn) {
    addLogMessage("You skip your turn because of handcuffs!");
    playerSkipNextTurn = false;
    endTurn();
    return;
  }
  processShot(player, player, playerDoubleDamage);
  playerDoubleDamage = false;
  if (shotgun.isEmpty()) {
    addLogMessage("Shotgun is empty! Reloading...");
    resetForNewStage();
  }
  checkStageEnd();
}

void Game::playerShootOpponent() {
  if (phase != GamePhase::PlayerTurn || !playerTurn) return;
  if (playerSkipNextTurn) {
    addLogMessage("You skip your turn because of handcuffs!");
    playerSkipNextTurn = false;
    endTurn();
    return;
  }
  processShot(player, ai, playerDoubleDamage);
  playerDoubleDamage = false;
  if (shotgun.isEmpty()) {
    addLogMessage("Shotgun is empty! Reloading...");
    resetForNewStage();
  }
  checkStageEnd();
}

void Game::playerUseItem(int itemIndex) {
  if (phase != GamePhase::PlayerTurn || !playerTurn) return;
  if (playerSkipNextTurn) {
    addLogMessage("You skip your turn because of handcuffs!");
    playerSkipNextTurn = false;
    endTurn();
    return;
  }
  const auto& items = player.getItems();
  if (itemIndex < 0 || itemIndex >= static_cast<int>(items.size())) return;
  ItemType type = items[itemIndex].type;
  bool turnContinues = true;
  bool doubleDamage = false;
  bool skipOpponent = false;
  applyItemEffect(player, ai, type, turnContinues, doubleDamage, skipOpponent);
  player.removeItem(type);

  if (doubleDamage) {
    playerDoubleDamage = true;
    addLogMessage("You used a hand saw! Next shot deals double damage.");
  }
  if (skipOpponent) {
    aiSkipNextTurn = true;
    addLogMessage("You put handcuffs on the creature! It will skip its next turn.");
  }
  if (type == ItemType::Magnifier) {
    if (!shotgun.isEmpty()) {
      revealedShell = shotgun.nextShell();
      magnifierUsedThisTurn = true;
      addLogMessage("You used magnifier: current shell is " +
        std::string(revealedShell == ShellType::Live ? "LIVE (red)" : "BLANK (blue)"));
    }
    else {
      addLogMessage("Shotgun is empty, magnifier is useless.");
    }
  }
  if (type == ItemType::Soda) {
    if (!shotgun.isEmpty()) {
      shotgun.discardCurrent();
      addLogMessage("You discarded the current shell with soda.");
    }
    else {
      addLogMessage("Shotgun is empty, soda does nothing.");
    }
  }
  if (type == ItemType::Apple) {
    addLogMessage("You ate an apple and restored 1 health.");
  }

  if (!turnContinues) {
    endTurn();
  }
  checkStageEnd();
}

void Game::applyItemEffect(Player& user, Player& target, ItemType item,
  bool& turnContinues, bool& doubleDamage, bool& skipOpponent) {
  switch (item) {
  case ItemType::Soda:
    turnContinues = true;
    break;
  case ItemType::Saw:
    doubleDamage = true;
    turnContinues = true;
    break;
  case ItemType::Handcuffs:
    skipOpponent = true;
    turnContinues = true;
    break;
  case ItemType::Magnifier:
    turnContinues = true;
    break;
  case ItemType::Apple:
    user.heal(1);
    turnContinues = true;
    break;
  default:
    break;
  }
}

void Game::processShot(Player& shooter, Player& target, bool doubleDamage) {
  if (shotgun.isEmpty()) {
    addLogMessage("No shells!");
    return;
  }
  ShellType shell = shotgun.fire();
  int damage = (shell == ShellType::Live) ? 1 : 0;
  if (doubleDamage && damage > 0) damage = 2;

  std::string shooterName = (&shooter == &player) ? "Player" : "Creature";
  std::string targetName = (&target == &player) ? "Player" : "Creature";

  if (shell == ShellType::Live) {
    target.takeDamage(damage);
    addLogMessage(shooterName + " shot " + targetName +
      " and dealt " + std::to_string(damage) + " damage!");
  }
  else {
    addLogMessage(shooterName + " shot " + targetName + ", but the shell was blank.");
  }

  if (shell == ShellType::Live) {
    endTurn();
  }
  else {
    if (&shooter == &target) {
      addLogMessage("Blank shot at yourself. Turn continues.");
    }
    else {
      endTurn();
    }
  }
}

void Game::endTurn() {
  playerTurn = !playerTurn;
  magnifierUsedThisTurn = false;

  if (playerTurn && playerSkipNextTurn) {
    playerSkipNextTurn = false;
    addLogMessage("You skip your turn because of handcuffs!");
    playerTurn = false;
    endTurn();
    return;
  }
  if (!playerTurn && aiSkipNextTurn) {
    aiSkipNextTurn = false;
    addLogMessage("Creature skips its turn because of handcuffs!");
    playerTurn = true;
    endTurn();
    return;
  }

  if (!playerTurn && phase == GamePhase::PlayerTurn) {
    phase = GamePhase::AITurn;
    aiTurn();
  }
  else if (playerTurn && phase == GamePhase::AITurn) {
    phase = GamePhase::PlayerTurn;
  }
}

void Game::aiTurn() {
  if (phase != GamePhase::AITurn) return;

  if (aiSkipNextTurn) {
    addLogMessage("Creature skips its turn because of handcuffs!");
    aiSkipNextTurn = false;
    endTurn();
    return;
  }

  auto action = aiBrain.decide(ai, player, shotgun, aiDoubleDamage, playerSkipNextTurn);

  if (action.type == AI::Action::UseItem) {
    const auto& items = ai.getItems();
    if (action.itemIndex >= 0 && action.itemIndex < static_cast<int>(items.size())) {
      ItemType type = items[action.itemIndex].type;
      bool turnContinues = true;
      bool doubleDamage = false;
      bool skipOpponent = false;
      applyItemEffect(ai, player, type, turnContinues, doubleDamage, skipOpponent);
      ai.removeItem(type);

      if (doubleDamage) {
        aiDoubleDamage = true;
        addLogMessage("Creature uses a hand saw! Its next shot deals double damage.");
      }
      if (skipOpponent) {
        playerSkipNextTurn = true;
        addLogMessage("Creature puts handcuffs on you! You will skip your next turn.");
      }
      if (type == ItemType::Magnifier) {
        if (!shotgun.isEmpty()) {
          addLogMessage("Creature uses magnifier.");
        }
        else {
          addLogMessage("Creature tries to use magnifier, but shotgun is empty.");
        }
      }
      if (type == ItemType::Soda) {
        if (!shotgun.isEmpty()) {
          shotgun.discardCurrent();
          addLogMessage("Creature discards the current shell with soda.");
        }
        else {
          addLogMessage("Creature tries to use soda, but shotgun is empty.");
        }
      }
      if (type == ItemType::Apple) {
        addLogMessage("Creature eats an apple and restores health.");
      }

      if (!turnContinues) {
        endTurn();
      }
    }
  }
  else if (action.type == AI::Action::ShootSelf) {
    processShot(ai, ai, aiDoubleDamage);
    aiDoubleDamage = false;
  }
  else if (action.type == AI::Action::ShootOpponent) {
    processShot(ai, player, aiDoubleDamage);
    aiDoubleDamage = false;
  }

  if (shotgun.isEmpty()) {
    addLogMessage("Shotgun is empty! Reloading...");
    resetForNewStage();
  }
  checkStageEnd();
}

void Game::checkStageEnd() {
  if (!player.isAlive()) {
    addLogMessage("Player lost this stage.");
    if (currentStage < 3) {
      addLogMessage("You are resurrected and start the stage anew.");
      resetForNewStage();
    }
    else {
      phase = GamePhase::GameOver;
      addLogMessage("Game over. You lost.");
    }
  }
  else if (!ai.isAlive()) {
    addLogMessage("Creature lost this stage.");
    if (currentStage < 3) {
      addLogMessage("You won this stage! Moving to the next stage.");
      currentStage++;
      if (currentStage > 3) {
        phase = GamePhase::GameOver;
        addLogMessage("Congratulations! You won the game!");
      }
      else {
        resetForNewStage();
      }
    }
    else {
      phase = GamePhase::GameOver;
      addLogMessage("You won the game! Congratulations!");
    }
  }
}

void Game::addLogMessage(const std::string& msg) {
  lastMessage = msg;
  logMessages.push_back(msg);
  if (logMessages.size() > 10) logMessages.erase(logMessages.begin());
}

int Game::getPlayerHealth() const { return player.getHealth(); }
int Game::getAIHealth() const { return ai.getHealth(); }
int Game::getStage() const { return currentStage; }
const std::vector<Item>& Game::getPlayerItems() const { return player.getItems(); }
const std::vector<Item>& Game::getAIItems() const { return ai.getItems(); }
bool Game::isPlayerTurn() const { return (phase == GamePhase::PlayerTurn) && playerTurn; }
bool Game::isDoubleDamageActive() const { return playerDoubleDamage; }
bool Game::isOpponentSkipNextTurn() const { return aiSkipNextTurn; }
std::string Game::getCurrentShellInfo() const {
  if (magnifierUsedThisTurn && !shotgun.isEmpty()) {
    return (revealedShell == ShellType::Live) ? "Live" : "Blank";
  }
  return "???";
}
std::string Game::getLastMessage() const { return lastMessage; }