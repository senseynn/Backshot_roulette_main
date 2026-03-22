#pragma once

#include "Player.h"
#include "AI.h"
#include "Shotgun.h"
#include <vector>
#include <string>

enum class GamePhase {
  StageIntro,
  PlayerTurn,
  AITurn,
  StageEnd,
  GameOver
};

class Game {
public:
  Game();
  void start();
  bool isGameOver() const;
  std::string getGameStateText() const;

  void playerShootSelf();
  void playerShootOpponent();
  void playerUseItem(int itemIndex);

  int getPlayerHealth() const;
  int getAIHealth() const;
  int getStage() const;
  const std::vector<Item>& getPlayerItems() const;
  const std::vector<Item>& getAIItems() const;
  bool isPlayerTurn() const;
  bool isDoubleDamageActive() const;
  bool isOpponentSkipNextTurn() const;
  std::string getCurrentShellInfo() const;
  std::string getLastMessage() const;

private:
  void nextStage();
  void resetForNewStage();
  void giveItemsToPlayers();
  void aiTurn();
  void applyItemEffect(Player& user, Player& target, ItemType item,
    bool& turnContinues, bool& doubleDamage, bool& skipOpponent);
  void processShot(Player& shooter, Player& target, bool doubleDamage);
  void endTurn();
  void checkStageEnd();
  void addLogMessage(const std::string& msg);

  int currentStage;
  int playerWins;
  int aiWins;

  Player player;
  Player ai;
  Shotgun shotgun;
  AI aiBrain;

  GamePhase phase;
  bool playerTurn;
  bool playerDoubleDamage;
  bool aiDoubleDamage;
  bool playerSkipNextTurn;
  bool aiSkipNextTurn;
  bool magnifierUsedThisTurn;
  ShellType revealedShell;

  std::vector<std::string> logMessages;
  std::string lastMessage;
};  