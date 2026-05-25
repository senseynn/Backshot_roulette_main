#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include <string>
#include <optional>

#include "GameState.h"
#include "Shotgun.h"
#include "Item.h"
#include "Particle.h"
#include "UI.h"
#include "SoundManager.h"
#include "DealerAI.h"
#include "Utils.h"

// ─── Game Constants ───────────────────────────────────────────────────────────
static constexpr int   WIN_W          = 1280;
static constexpr int   WIN_H          = 720;
static constexpr int   MAX_HP         = 4;
static constexpr int   ROUNDS_TO_WIN  = 2;       // best of 3
static constexpr float ANIM_DURATION  = 1.2f;    // shoot animation seconds
static constexpr float AI_THINK_TIME  = 1.0f;    // seconds before AI acts

enum class Difficulty { EASY, NORMAL, HARD };

class Game {
public:
    Game();
    void run();

private:
    // ── Core ────────────────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::Font         font_;
    bool             fontLoaded_ = false;

    // ── State machine ────────────────────────────────────────────────────────
    GameState   state_       = GameState::MAIN_MENU;
    Turn        turn_        = Turn::PLAYER;
    RoundResult roundResult_ = RoundResult::NONE;

    // ── Game data ────────────────────────────────────────────────────────────
    std::mt19937           rng_;
    Shotgun                gun_;
    int                    playerHP_   = MAX_HP;
    int                    dealerHP_   = MAX_HP;
    int                    playerWins_ = 0;
    int                    dealerWins_ = 0;
    int                    round_      = 1;
    Difficulty             difficulty_ = Difficulty::NORMAL;

    std::vector<ItemType> playerItems_;
    std::vector<ItemType> dealerItems_;

    bool playerHandcuffed_    = false;
    bool dealerHandcuffed_    = false;
    bool playerCuffedThisTurn_= false;
    bool dealerCuffedThisTurn_= false;

    bool adrenalineMode_     = false;
    bool adrenalineIsDealer_ = false;

    std::optional<ShellType> peekResult_;
    std::optional<int>       phoneRevealIdx_;

    // ── Animation ────────────────────────────────────────────────────────────
    float         animTimer_    = 0.f;
    float         aiThinkTimer_ = 0.f;
    bool          shootSelf_    = false;
    ShellType     firedShell_   = ShellType::BLANK;
    float         flashAlpha_   = 0.f;
    sf::Vector2f  cameraOffset_{0.f, 0.f};
    float         cameraShake_  = 0.f;

    // ── Render objects ───────────────────────────────────────────────────────
    ParticleSystem    particles_;
    SoundManager      sound_;
    GameLog           log_;

    sf::RenderTexture rtTable_;
    sf::RenderTexture rtShotgun_;

    // ── UI ───────────────────────────────────────────────────────────────────
    Button                    btnShootPlayer_;
    Button                    btnShootSelf_;
    Button                    btnMenu_;
    Button                    btnEasy_;
    Button                    btnNormal_;
    Button                    btnHard_;
    std::vector<ItemSlot>     playerSlots_;
    std::vector<ItemSlot>     dealerSlots_;
    ShellIndicator            shellRow_;

    sf::Text txtTitle_;
    sf::Text txtPlayerHP_;
    sf::Text txtDealerHP_;
    sf::Text txtRound_;
    sf::Text txtTurn_;
    sf::Text txtStatus_;
    sf::Text txtPeek_;

    sf::RectangleShape vignette_;
    sf::RectangleShape flashOverlay_;

    // ── Methods ──────────────────────────────────────────────────────────────
    void initWindow();
    void initUI();
    void buildVignette();
    void buildProceduralGraphics();

    void startGame();
    void startRound();
    void dealItemsAndLoad();
    void endRound(RoundResult res);
    void endGame();

    void handleEvents();
    void update(float dt);
    void render();

    void playerAction_ShootOpponent();
    void playerAction_ShootSelf();
    void playerUseItem(int slotIdx);
    void applyAdrenalineSteal(int slotIdx);

    void dealerTurnUpdate(float dt);
    void dealerUseItem(ItemType it);

    void resolveFire(bool selfShot);
    void switchTurn();
    void checkMagEmpty();

    int         playerMaxHP() const;
    int         dealerMaxHP() const;
    float       dealerMistakeChance() const;
    std::string difficultyName() const;
    void        chooseDifficulty(Difficulty d);
    void        addLog(const std::string& msg, sf::Color col = sf::Color(210, 200, 180));
    void        triggerCameraShake(float amt);
    void        triggerFlash(float alpha = 1.f);

    void drawBackground(sf::RenderTarget& rt);
    void drawDealer(sf::RenderTarget& rt);
    void drawShotgun(sf::RenderTarget& rt);
    void drawHealthBars(sf::RenderTarget& rt);
    void drawShells(sf::RenderTarget& rt);
    void drawItems(sf::RenderTarget& rt);
    void drawHUD(sf::RenderTarget& rt);
    void drawMainMenu(sf::RenderTarget& rt);
    void drawRoundEnd(sf::RenderTarget& rt);
    void drawGameOver(sf::RenderTarget& rt);
    void drawPeekResult(sf::RenderTarget& rt);

    sf::Text makeText(const std::string& s, unsigned size,
                      sf::Color col = sf::Color::White) const;
};
