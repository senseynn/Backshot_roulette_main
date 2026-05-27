#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

// ────────────────────────────────────────────────────────────────── Helpers ──

static sf::Color darkBg(15, 12, 18);
static sf::Color feltGreen(28, 80, 40);
static sf::Color gold(200, 170, 80);
static sf::Color liveRed(210, 55, 35);
static sf::Color blankBlue(55, 90, 200);
static sf::Color textCream(230, 220, 195);

static void drawRoundRect(sf::RenderTarget& rt, sf::Vector2f pos, sf::Vector2f sz,
    float r, sf::Color fill, sf::Color outline = sf::Color::Transparent,
    float outlineT = 0.f) {
    sf::RectangleShape rect(sz);
    rect.setPosition(pos);
    rect.setFillColor(fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(outlineT);
    rt.draw(rect);
}

// ─────────────────────────────────────────────────── constructor / run ───────

Game::Game()
    : rng_(static_cast<unsigned>(std::time(nullptr)))
{
    initWindow();
    fontLoaded_ = loadFont(font_);
    initUI();
    buildProceduralGraphics();
    buildVignette();
}

void Game::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);
        handleEvents();
        update(dt);
        render();
    }
}

// ──────────────────────────────────────────────────────── initialisation ─────

void Game::initWindow() {
    sf::VideoMode vm(WIN_W, WIN_H);
    window_.create(vm, "Buckshot Roulette", sf::Style::Titlebar | sf::Style::Close);
    window_.setFramerateLimit(60);
}

void Game::initUI() {
    btnShootPlayer_.setup({ WIN_W / 2.f - 270.f, WIN_H - 74.f }, { 240.f, 48.f },
        "ВЫСТРЕЛИТЬ В ДИЛЕРА", font_, 15,
        sf::Color(100, 30, 20), sf::Color(160, 50, 30), sf::Color(70, 15, 10));
    btnShootPlayer_.id = "shoot_player";

    btnShootSelf_.setup({ WIN_W / 2.f + 30.f, WIN_H - 74.f }, { 240.f, 48.f },
        "ВЫСТРЕЛИТЬ В СЕБЯ", font_, 15,
        sf::Color(30, 50, 100), sf::Color(50, 80, 160), sf::Color(15, 30, 70));
    btnShootSelf_.id = "shoot_self";

    btnMenu_.setup({ WIN_W - 154.f, 18.f }, { 132.f, 34.f }, "ГЛАВНОЕ МЕНЮ", font_, 12);
    btnMenu_.id = "menu";

    btnEasy_.setup({ WIN_W / 2.f - 330.f, WIN_H - 86.f }, { 200.f, 46.f },
        "ЛЁГКАЯ", font_, 18,
        sf::Color(28, 70, 42), sf::Color(42, 110, 62), sf::Color(18, 48, 30));
    btnEasy_.id = "easy";

    btnNormal_.setup({ WIN_W / 2.f - 100.f, WIN_H - 86.f }, { 200.f, 46.f },
        "НОРМАЛЬНАЯ", font_, 17,
        sf::Color(72, 58, 28), sf::Color(118, 92, 36), sf::Color(48, 38, 20));
    btnNormal_.id = "normal";

    btnHard_.setup({ WIN_W / 2.f + 130.f, WIN_H - 86.f }, { 200.f, 46.f },
        "СЛОЖНАЯ", font_, 18,
        sf::Color(85, 28, 28), sf::Color(140, 38, 38), sf::Color(54, 18, 18));
    btnHard_.id = "hard";

    playerSlots_.resize(MAX_ITEMS);
    for (int i = 0; i < MAX_ITEMS; ++i) {
        float x = 40.f + i * 148.f;
        float y = WIN_H - 154.f;
        playerSlots_[i].setup({ x, y }, { 136.f, 52.f }, font_);
        playerSlots_[i].setItem(ItemType::NONE);
    }

    dealerSlots_.resize(MAX_ITEMS);
    for (int i = 0; i < MAX_ITEMS; ++i) {
        float x = 760.f + (i % 4) * 86.f;
        float y = 24.f + (i / 4) * 34.f;
        dealerSlots_[i].setup({ x, y }, { 78.f, 28.f }, font_);
        dealerSlots_[i].showDescription = false;
        dealerSlots_[i].setItem(ItemType::NONE);
    }

    log_.setFont(font_, { 38.f, 238.f }, 330.f);

    txtTitle_ = makeText("БАКШОТ РУЛЕТКА", 48, gold);
    txtPlayerHP_ = makeText("Жизни: 4", 22, textCream);
    txtDealerHP_ = makeText("Жизни: 4", 22, textCream);
    txtRound_ = makeText("Раунд 1", 20, gold);
    txtTurn_ = makeText("ТВОЙ ХОД", 22, sf::Color(100, 220, 100));
    txtStatus_ = makeText("", 20, textCream);
    txtPeek_ = makeText("", 24, sf::Color::White);
}

void Game::buildVignette() {
    vignette_.setSize({ (float)WIN_W, (float)WIN_H });
    vignette_.setFillColor(sf::Color::Transparent);
    flashOverlay_.setSize({ (float)WIN_W, (float)WIN_H });
    flashOverlay_.setFillColor(sf::Color(255, 255, 255, 0));
}

void Game::buildProceduralGraphics() {}

// ──────────────────────────────────────────────────────────── game logic ─────

void Game::startGame() {
    playerHP_ = playerMaxHP();
    dealerHP_ = dealerMaxHP();
    playerWins_ = 0;
    dealerWins_ = 0;
    round_ = 1;
    playerItems_.clear();
    dealerItems_.clear();
    log_.add("=== НОВАЯ ИГРА ===", gold);
    log_.add("Сложность: " + difficultyName(), sf::Color(220, 200, 120));
    startRound();
}

void Game::startRound() {
    playerHP_ = playerMaxHP();
    dealerHP_ = dealerMaxHP();
    playerHandcuffed_ = false;
    dealerHandcuffed_ = false;
    playerCuffedThisTurn_ = false;
    dealerCuffedThisTurn_ = false;
    adrenalineMode_ = false;
    peekResult_.reset();
    phoneRevealIdx_.reset();
    particles_.clear();
    gun_.knifeActive = false;

    dealItemsAndLoad();
    turn_ = Turn::PLAYER;
    state_ = GameState::PLAYER_TURN;

    std::ostringstream oss;
    oss << "=== РАУНД " << round_ << " ===";
    log_.add(oss.str(), gold);
    log_.add("Патроны заряжены. Твой ход.", textCream);
}

void Game::dealItemsAndLoad() {
    int total = randInt(rng_, 2, 8);
    int live = randInt(rng_, 1, total - 1);
    int blank = total - live;
    gun_.load(live, blank, rng_);

    int pItems = (round_ >= 2) ? randInt(rng_, 1, 4) : 0;
    int dItems = (round_ >= 2) ? randInt(rng_, 1, 4) : 0;

    playerItems_ = dealItems(rng_, pItems);
    dealerItems_ = dealItems(rng_, dItems);

    if (round_ >= 2) {
        if (difficulty_ == Difficulty::EASY) {
            if ((int)playerItems_.size() < MAX_ITEMS) playerItems_.push_back(ItemType::CIGARETTE);
            if (!dealerItems_.empty()) dealerItems_.pop_back();
        }
        else if (difficulty_ == Difficulty::HARD) {
            if ((int)dealerItems_.size() < MAX_ITEMS) dealerItems_.push_back(randomItem(rng_));
            if (playerItems_.size() > 1) playerItems_.pop_back();
        }
    }

    for (int i = 0; i < MAX_ITEMS; ++i) {
        playerSlots_[i].setItem(i < (int)playerItems_.size() ? playerItems_[i] : ItemType::NONE);
        dealerSlots_[i].setItem(i < (int)dealerItems_.size() ? dealerItems_[i] : ItemType::NONE);
    }

    std::ostringstream oss;
    oss << "Заряжено: боевых " << gun_.liveCount << ", холостых " << gun_.blankCount << ".";
    log_.add(oss.str(), textCream);
    oss.str("");
    if (round_ >= 2) {
        oss << "Ты получил предметов: " << playerItems_.size()
            << ". Дилер получил: " << dealerItems_.size() << ".";
        log_.add(oss.str(), textCream);
    }
    else {
        log_.add("В 1-м раунде предметов нет. Со 2-го раунда они выдаются случайно.", textCream);
    }

    shellRow_.rebuild(gun_.chambers, font_, { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
}

void Game::endRound(RoundResult res) {
    roundResult_ = res;
    state_ = GameState::ROUND_END;
    if (res == RoundResult::PLAYER_WIN) {
        playerWins_++;
        log_.add("ТЫ ВЫИГРАЛ раунд!", sf::Color(100, 255, 100));
        sound_.play(SoundManager::SFX::ROUND_WIN);
    }
    else {
        dealerWins_++;
        log_.add("ДИЛЕР ВЫИГРАЛ раунд!", sf::Color(255, 100, 100));
        sound_.play(SoundManager::SFX::ROUND_LOSE);
    }
}

void Game::endGame() {
    state_ = GameState::GAME_OVER;
}

int Game::playerMaxHP() const {
    switch (difficulty_) {
    case Difficulty::EASY:   return 5;
    case Difficulty::HARD:   return 4;
    default:                 return 4;
    }
}

int Game::dealerMaxHP() const {
    switch (difficulty_) {
    case Difficulty::EASY:   return 3;
    case Difficulty::HARD:   return 5;
    default:                 return 4;
    }
}

float Game::dealerMistakeChance() const {
    switch (difficulty_) {
    case Difficulty::EASY:   return 0.25f;
    case Difficulty::NORMAL: return 0.08f;
    default:                 return 0.0f;
    }
}

std::string Game::difficultyName() const {
    switch (difficulty_) {
    case Difficulty::EASY:   return "ЛЁГКАЯ";
    case Difficulty::HARD:   return "СЛОЖНАЯ";
    default:                 return "НОРМАЛЬНАЯ";
    }
}

void Game::chooseDifficulty(Difficulty d) {
    difficulty_ = d;
    sound_.play(SoundManager::SFX::CLICK);
}

// ──────────────────────────────────────────────────────── handle events ──────

void Game::handleEvents() {
    sf::Vector2f mouse = window_.mapPixelToCoords(sf::Mouse::getPosition(window_));
    bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (state_ == GameState::PLAYER_TURN) {
        btnShootPlayer_.update(mouse, mouseDown);
        btnShootSelf_.update(mouse, mouseDown);
        for (auto& slot : playerSlots_) slot.update(mouse);
        for (auto& slot : dealerSlots_) slot.update(mouse);
    }
    btnMenu_.update(mouse, mouseDown);
    if (state_ == GameState::MAIN_MENU) {
        btnEasy_.update(mouse, mouseDown);
        btnNormal_.update(mouse, mouseDown);
        btnHard_.update(mouse, mouseDown);
    }

    sf::Event ev;
    while (window_.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed) {
            window_.close();
        }

        if (ev.type == sf::Event::MouseButtonReleased &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f pt((float)ev.mouseButton.x, (float)ev.mouseButton.y);

            if (btnMenu_.contains(pt)) {
                state_ = GameState::MAIN_MENU;
                sound_.play(SoundManager::SFX::CLICK);
                return;
            }

            if (state_ == GameState::MAIN_MENU) {
                if (btnEasy_.contains(pt)) { chooseDifficulty(Difficulty::EASY);   startGame(); }
                else if (btnNormal_.contains(pt)) { chooseDifficulty(Difficulty::NORMAL); startGame(); }
                else if (btnHard_.contains(pt)) { chooseDifficulty(Difficulty::HARD);   startGame(); }
                return;
            }

            if (state_ == GameState::ROUND_END) {
                if (playerWins_ >= ROUNDS_TO_WIN || dealerWins_ >= ROUNDS_TO_WIN)
                    endGame();
                else { round_++; startRound(); }
                return;
            }

            if (state_ == GameState::GAME_OVER) {
                state_ = GameState::MAIN_MENU;
                return;
            }

            if (state_ == GameState::PLAYER_TURN) {
                if (adrenalineMode_ && !adrenalineIsDealer_) {
                    for (int i = 0; i < (int)playerSlots_.size(); ++i) {
                        if (playerSlots_[i].contains(pt) &&
                            playerSlots_[i].item != ItemType::NONE) {
                            applyAdrenalineSteal(i);
                            return;
                        }
                    }
                    return;
                }

                if (btnShootPlayer_.contains(pt)) { playerAction_ShootOpponent(); return; }
                if (btnShootSelf_.contains(pt)) { playerAction_ShootSelf();     return; }

                for (int i = 0; i < (int)playerSlots_.size(); ++i)
                    if (playerSlots_[i].contains(pt)) { playerUseItem(i); return; }
            }
        }
    }
}

// ─────────────────────────────────────────────────── player actions ──────────

void Game::playerAction_ShootOpponent() {
    if (state_ != GameState::PLAYER_TURN) return;
    state_ = GameState::SHOOT_OPPONENT;
    animTimer_ = 0.f;
    shootSelf_ = false;
    firedShell_ = gun_.peek();
}

void Game::playerAction_ShootSelf() {
    if (state_ != GameState::PLAYER_TURN) return;
    state_ = GameState::SHOOT_SELF;
    animTimer_ = 0.f;
    shootSelf_ = true;
    firedShell_ = gun_.peek();
}

void Game::playerUseItem(int slotIdx) {
    if (slotIdx < 0 || slotIdx >= (int)playerItems_.size()) return;
    ItemType it = playerItems_[slotIdx];
    if (it == ItemType::NONE) return;

    playerItems_.erase(playerItems_.begin() + slotIdx);
    for (int i = 0; i < MAX_ITEMS; ++i)
        playerSlots_[i].setItem(i < (int)playerItems_.size() ? playerItems_[i] : ItemType::NONE);

    sound_.play(SoundManager::SFX::ITEM_USE);
    log_.add(std::string("Ты используешь: ") + itemName(it), sf::Color(180, 220, 255));
    particles_.spawnSparkle({ WIN_W / 2.f, WIN_H / 2.f }, sf::Color(180, 140, 255), rng_);

    switch (it) {
    case ItemType::BEER: {
        ShellType ejected = gun_.eject();
        std::string s = "Пиво: выбит патрон ";
        s += (ejected == ShellType::LIVE) ? "БОЕВОЙ" : "ХОЛОСТОЙ";
        log_.add(s, sf::Color(220, 200, 100));
        sound_.play(SoundManager::SFX::BEER);
        particles_.spawnShellEject({ WIN_W / 2.f, WIN_H / 2.f + 30.f }, rng_);
        shellRow_.rebuild(gun_.chambers, font_, { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        checkMagEmpty();
        break;
    }
    case ItemType::MAGNIFYING_GLASS: {
        gun_.revealTop();
        ShellType top = gun_.peek();
        peekResult_ = top;
        std::string s = "Лупа: в стволе ";
        s += (top == ShellType::LIVE) ? "БОЕВОЙ!" : "холостой.";
        log_.add(s, sf::Color(200, 255, 200));
        sound_.play(SoundManager::SFX::REVEAL);
        shellRow_.rebuild(gun_.chambers, font_, { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        break;
    }
    case ItemType::CIGARETTE: {
        if (playerHP_ < playerMaxHP()) {
            playerHP_++;
            log_.add("Сигарета: +1 жизнь", sf::Color(100, 255, 100));
        }
        else {
            log_.add("Сигарета: здоровье уже полное", sf::Color(180, 180, 180));
        }
        sound_.play(SoundManager::SFX::HEAL);
        break;
    }
    case ItemType::KNIFE:
        gun_.knifeActive = true;
        log_.add("Нож: следующий боевой выстрел нанесёт 2 урона!", sf::Color(255, 160, 80));
        break;
    case ItemType::HANDCUFFS:
        dealerHandcuffed_ = true;
        log_.add("Наручники: дилер пропустит следующий ход!", sf::Color(200, 200, 100));
        sound_.play(SoundManager::SFX::HANDCUFFS);
        break;
    case ItemType::ADRENALINE: {
        ItemType stolen = DealerAI::chooseStealTarget(dealerItems_);
        if (stolen == ItemType::NONE && !dealerItems_.empty())
            stolen = dealerItems_.front();
        if (stolen != ItemType::NONE) {
            auto dit = std::find(dealerItems_.begin(), dealerItems_.end(), stolen);
            if (dit != dealerItems_.end()) {
                dealerItems_.erase(dit);
                log_.add(std::string("Адреналин: украден предмет ") + itemName(stolen) +
                    " у дилера!", sf::Color(255, 120, 255));
                playerItems_.insert(playerItems_.begin(), stolen);
                for (int i = 0; i < MAX_ITEMS; ++i)
                    playerSlots_[i].setItem(i < (int)playerItems_.size() ? playerItems_[i] : ItemType::NONE);
                for (int i = 0; i < MAX_ITEMS; ++i)
                    dealerSlots_[i].setItem(i < (int)dealerItems_.size() ? dealerItems_[i] : ItemType::NONE);
                log_.add("Украденный предмет добавлен в руку!", sf::Color(200, 200, 255));
            }
        }
        else {
            log_.add("Адреналин: у дилера нет предметов!", sf::Color(180, 180, 180));
        }
        break;
    }
    case ItemType::PHONE: {
        int idx = gun_.revealRandom(rng_);
        if (idx >= 0 && idx < (int)gun_.chambers.size()) {
            ShellType rt2 = gun_.chambers[idx].type;
            std::ostringstream oss;
            oss << "Телефон: патрон #" << (idx + 1) << " — "
                << (rt2 == ShellType::LIVE ? "БОЕВОЙ" : "ХОЛОСТОЙ");
            log_.add(oss.str(), sf::Color(100, 200, 255));
            phoneRevealIdx_ = idx;
        }
        else {
            log_.add("Телефон: все патроны уже известны.", sf::Color(180, 180, 180));
        }
        sound_.play(SoundManager::SFX::PHONE);
        shellRow_.rebuild(gun_.chambers, font_, { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        break;
    }
    default: break;
    }
}

// ─────────────────────────────────────────────── dealerUseItem ───────────────

void Game::dealerUseItem(ItemType it) {
    log_.add(std::string("Дилер использует: ") + itemName(it), sf::Color(255, 200, 180));
    sound_.play(SoundManager::SFX::ITEM_USE);
    particles_.spawnSparkle({ WIN_W / 2.f, 200.f }, sf::Color(255, 180, 120), rng_);

    switch (it) {
    case ItemType::BEER: {
        ShellType ejected = gun_.eject();
        std::string s = "Пиво: дилер выбил патрон ";
        s += (ejected == ShellType::LIVE) ? "БОЕВОЙ" : "ХОЛОСТОЙ";
        log_.add(s, sf::Color(220, 200, 100));
        sound_.play(SoundManager::SFX::BEER);
        shellRow_.rebuild(gun_.chambers, font_,
            { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        checkMagEmpty();
        break;
    }
    case ItemType::MAGNIFYING_GLASS: {
        gun_.revealTop();
        ShellType top = gun_.peek();
        std::string s = "Дилер смотрит через лупу: ";
        s += (top == ShellType::LIVE) ? "БОЕВОЙ!" : "холостой.";
        log_.add(s, sf::Color(255, 220, 180));
        sound_.play(SoundManager::SFX::REVEAL);
        shellRow_.rebuild(gun_.chambers, font_,
            { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        break;
    }
    case ItemType::CIGARETTE: {
        if (dealerHP_ < dealerMaxHP()) {
            dealerHP_++;
            log_.add("Дилер лечится: +1 жизнь", sf::Color(255, 120, 120));
        }
        else {
            log_.add("Дилер: здоровье уже полное", sf::Color(180, 180, 180));
        }
        sound_.play(SoundManager::SFX::HEAL);
        break;
    }
    case ItemType::KNIFE:
        gun_.knifeActive = true;
        log_.add("Дилер активирует нож! Урон x2!", sf::Color(255, 100, 60));
        break;
    case ItemType::HANDCUFFS:
        playerHandcuffed_ = true;
        log_.add("Дилер надел на тебя наручники! Ты пропустишь ход!", sf::Color(255, 180, 60));
        sound_.play(SoundManager::SFX::HANDCUFFS);
        break;
    case ItemType::ADRENALINE: {
        ItemType stolen = DealerAI::chooseStealTarget(playerItems_);
        if (stolen == ItemType::NONE && !playerItems_.empty())
            stolen = playerItems_.front();
        if (stolen != ItemType::NONE) {
            auto pit = std::find(playerItems_.begin(), playerItems_.end(), stolen);
            if (pit != playerItems_.end()) {
                playerItems_.erase(pit);
                dealerItems_.insert(dealerItems_.begin(), stolen);
                log_.add(std::string("Дилер украл твой предмет: ") + itemName(stolen) + "!",
                    sf::Color(255, 100, 200));
                for (int i = 0; i < MAX_ITEMS; ++i) {
                    playerSlots_[i].setItem(i < (int)playerItems_.size() ? playerItems_[i] : ItemType::NONE);
                    dealerSlots_[i].setItem(i < (int)dealerItems_.size() ? dealerItems_[i] : ItemType::NONE);
                }
            }
        }
        else {
            log_.add("Адреналин дилера: нечего украсть.", sf::Color(180, 180, 180));
        }
        break;
    }
    case ItemType::PHONE: {
        int idx = gun_.revealRandom(rng_);
        if (idx >= 0) {
            std::ostringstream oss;
            oss << "Телефон дилера: проверен патрон #" << (idx + 1);
            log_.add(oss.str(), sf::Color(255, 200, 120));
        }
        sound_.play(SoundManager::SFX::PHONE);
        shellRow_.rebuild(gun_.chambers, font_,
            { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });
        break;
    }
    default: break;
    }
}

void Game::applyAdrenalineSteal(int slotIdx) {
    // Not used in this flow (handled inline in playerUseItem)
}

void Game::checkMagEmpty() {
    if (gun_.empty()) {
        log_.add("Магазин пуст. Перезарядка...", gold);
        dealItemsAndLoad();
    }
}

// ──────────────────────────────────────────────────────────────── update ─────

void Game::update(float dt) {
    if (cameraShake_ > 0.f) {
        cameraShake_ -= dt * 6.f;
        if (cameraShake_ < 0.f) cameraShake_ = 0.f;
        float sx = ((rng_() % 200) / 100.f - 1.f) * cameraShake_ * 8.f;
        float sy = ((rng_() % 200) / 100.f - 1.f) * cameraShake_ * 8.f;
        cameraOffset_ = { sx, sy };
    }
    else {
        cameraOffset_ = { 0.f, 0.f };
    }

    if (flashAlpha_ > 0.f) {
        flashAlpha_ -= dt * 3.f;
        if (flashAlpha_ < 0.f) flashAlpha_ = 0.f;
    }

    particles_.update(dt);

    switch (state_) {
    case GameState::MAIN_MENU: break;
    case GameState::GAME_OVER: break;
    case GameState::ROUND_END:
        animTimer_ += dt;
        break;
    case GameState::SHOOT_SELF:
    case GameState::SHOOT_OPPONENT:
        animTimer_ += dt;
        if (animTimer_ >= ANIM_DURATION)
            resolveFire(state_ == GameState::SHOOT_SELF);
        break;
    case GameState::DEALER_TURN:
        dealerTurnUpdate(dt);
        break;
    case GameState::PLAYER_TURN:
        peekResult_.reset();
        break;
    default: break;
    }
}

void Game::dealerTurnUpdate(float dt) {
    aiThinkTimer_ += dt;
    if (aiThinkTimer_ < AI_THINK_TIME) return;
    aiThinkTimer_ = 0.f;

    if (dealerHandcuffed_ && !dealerCuffedThisTurn_) {
        dealerHandcuffed_ = false;
        dealerCuffedThisTurn_ = true;
        log_.add("Дилер в наручниках — пропускает ход!", sf::Color(220, 220, 100));
        switchTurn();
        return;
    }
    dealerCuffedThisTurn_ = false;

    if (gun_.empty()) dealItemsAndLoad();

    DealerAction action = DealerAI::decide(dealerItems_, playerItems_, gun_,
        dealerHP_, playerHP_, rng_);

    std::uniform_real_distribution<float> missDist(0.f, 1.f);
    if (missDist(rng_) < dealerMistakeChance()) {
        if (action.actionType == DealerAction::Type::USE_ITEM) {
            action.actionType = DealerAction::Type::SHOOT_PLAYER;
            action.item = ItemType::NONE;
        }
        else if (action.actionType == DealerAction::Type::SHOOT_PLAYER) {
            action.actionType = DealerAction::Type::SHOOT_SELF;
        }
        else {
            action.actionType = DealerAction::Type::SHOOT_PLAYER;
        }
        log_.add("Дилер нервничает и ошибается...", sf::Color(180, 180, 120));
    }

    switch (action.actionType) {
    case DealerAction::Type::USE_ITEM:
        dealerUseItem(action.item);
        for (int i = 0; i < MAX_ITEMS; ++i)
            dealerSlots_[i].setItem(i < (int)dealerItems_.size() ? dealerItems_[i] : ItemType::NONE);
        break;
    case DealerAction::Type::SHOOT_SELF:
        log_.add("Дилер стреляет в себя...", sf::Color(255, 200, 150));
        state_ = GameState::SHOOT_SELF;
        animTimer_ = 0.f;
        shootSelf_ = true;
        firedShell_ = gun_.peek();
        break;
    case DealerAction::Type::SHOOT_PLAYER:
        log_.add("Дилер стреляет в ТЕБЯ!", sf::Color(255, 80, 80));
        state_ = GameState::SHOOT_OPPONENT;
        animTimer_ = 0.f;
        shootSelf_ = false;
        firedShell_ = gun_.peek();
        break;
    }
}

// ────────────────────────────────────────────────────────── resolve fire ─────

void Game::resolveFire(bool selfShot) {
    ShellType shell = gun_.fire();
    int damage = gun_.damage();
    gun_.knifeActive = false;
    bool isLive = (shell == ShellType::LIVE);

    sf::Vector2f muzzlePos = { WIN_W / 2.f, WIN_H / 2.f };
    if (isLive) {
        triggerCameraShake(1.f);
        triggerFlash(0.6f);
        if (turn_ == Turn::PLAYER) {
            if (selfShot) {
                playerHP_ -= damage;
                muzzlePos = { WIN_W / 2.f - 200.f, WIN_H * 0.7f };
                particles_.spawnHit(muzzlePos, rng_);
                log_.add("БОЕВОЙ! Ты выстрелил в себя. Урон: " + std::to_string(damage) + "!", liveRed);
            }
            else {
                dealerHP_ -= damage;
                muzzlePos = { WIN_W / 2.f + 100.f, WIN_H * 0.3f };
                particles_.spawnHit(muzzlePos, rng_);
                log_.add("БОЕВОЙ! Дилер получает урон: " + std::to_string(damage) + "!", sf::Color(255, 60, 60));
            }
        }
        else {
            if (selfShot) {
                dealerHP_ -= damage;
                muzzlePos = { WIN_W / 2.f + 100.f, WIN_H * 0.3f };
                particles_.spawnHit(muzzlePos, rng_);
                log_.add("БОЕВОЙ! Дилер выстрелил в себя. Урон: " + std::to_string(damage) + "!", sf::Color(200, 120, 80));
            }
            else {
                playerHP_ -= damage;
                muzzlePos = { WIN_W / 2.f - 200.f, WIN_H * 0.7f };
                particles_.spawnHit(muzzlePos, rng_);
                log_.add("БОЕВОЙ! Ты получаешь урон: " + std::to_string(damage) + "!", liveRed);
            }
        }
        particles_.spawnGunshot(muzzlePos, rng_);
        sound_.play(SoundManager::SFX::GUNSHOT);
    }
    else {
        particles_.spawnShellEject(muzzlePos, rng_);
        sound_.play(SoundManager::SFX::BLANK_SHOT);
        if (turn_ == Turn::PLAYER) {
            if (selfShot)
                log_.add("Холостой. Ты выстрелил в себя — ход остаётся!", blankBlue);
            else
                log_.add("Холостой. Выстрел по дилеру не сработал.", sf::Color(120, 120, 200));
        }
        else {
            if (selfShot)
                log_.add("Холостой. Дилер выстрелил в себя — ход остаётся у него.", sf::Color(200, 160, 120));
            else
                log_.add("Холостой. Дилер промахнулся.", blankBlue);
        }
    }

    if (playerHP_ <= 0) { log_.add("Ты МЁРТВ.", liveRed);                  endRound(RoundResult::DEALER_WIN); return; }
    if (dealerHP_ <= 0) { log_.add("Дилер МЁРТВ.", sf::Color(100, 255, 100)); endRound(RoundResult::PLAYER_WIN); return; }

    if (gun_.empty()) { log_.add("Магазин пуст. Перезарядка...", gold); dealItemsAndLoad(); }

    shellRow_.rebuild(gun_.chambers, font_,
        { WIN_W / 2.f - gun_.total() * 14.f, WIN_H / 2.f - 42.f });

    bool keepTurn = (!isLive && selfShot);
    if (keepTurn) {
        if (turn_ == Turn::PLAYER) { state_ = GameState::PLAYER_TURN; peekResult_.reset(); }
        else { state_ = GameState::DEALER_TURN;  aiThinkTimer_ = 0.f; }
    }
    else {
        switchTurn();
    }
}

void Game::switchTurn() {
    peekResult_.reset();
    gun_.knifeActive = false;

    if (turn_ == Turn::PLAYER) {
        turn_ = Turn::DEALER;
        state_ = GameState::DEALER_TURN;
        aiThinkTimer_ = 0.f;
        if (dealerHandcuffed_ && !dealerCuffedThisTurn_) {
            dealerHandcuffed_ = false;
            dealerCuffedThisTurn_ = true;
            log_.add("Дилер в наручниках — пропуск!", sf::Color(220, 220, 100));
            switchTurn();
        }
    }
    else {
        turn_ = Turn::PLAYER;
        state_ = GameState::PLAYER_TURN;
        if (playerHandcuffed_ && !playerCuffedThisTurn_) {
            playerHandcuffed_ = false;
            playerCuffedThisTurn_ = true;
            log_.add("Ты в наручниках — пропуск!", sf::Color(220, 220, 100));
            switchTurn();
        }
    }
}

// ──────────────────────────────────────────────────────────────── render ─────

void Game::render() {
    window_.clear(darkBg);

    sf::View view = window_.getDefaultView();
    view.move(cameraOffset_);
    window_.setView(view);

    if (state_ == GameState::MAIN_MENU)      drawMainMenu(window_);
    else if (state_ == GameState::GAME_OVER) drawGameOver(window_);
    else {
        drawBackground(window_);
        drawDealer(window_);
        drawShotgun(window_);
        drawShells(window_);
        drawHealthBars(window_);
        drawItems(window_);
        drawHUD(window_);
        if (state_ == GameState::ROUND_END) drawRoundEnd(window_);
        drawPeekResult(window_);
        particles_.draw(window_);
        window_.setView(window_.getDefaultView());
        log_.draw(window_);
    }

    window_.setView(window_.getDefaultView());

    if (flashAlpha_ > 0.f) {
        flashOverlay_.setFillColor(sf::Color(255, 255, 255,
            static_cast<sf::Uint8>(flashAlpha_ * 200.f)));
        window_.draw(flashOverlay_);
    }

    {
        auto drawGrad = [&](sf::Vector2f pos, sf::Vector2f sz, sf::Color c1, sf::Color c2, bool horiz) {
            sf::VertexArray va(sf::Quads, 4);
            va[0].position = pos;
            va[1].position = { pos.x + sz.x, pos.y };
            va[2].position = { pos.x + sz.x, pos.y + sz.y };
            va[3].position = { pos.x, pos.y + sz.y };
            if (horiz) { va[0].color = c1; va[1].color = c2; va[2].color = c2; va[3].color = c1; }
            else { va[0].color = c1; va[1].color = c1; va[2].color = c2; va[3].color = c2; }
            window_.draw(va);
            };
        sf::Color vC(0, 0, 0, 0), vE(0, 0, 0, 140);
        float vW = 120.f;
        drawGrad({ 0,0 }, { vW, (float)WIN_H }, vE, vC, true);
        drawGrad({ WIN_W - vW, 0 }, { vW, (float)WIN_H }, vC, vE, true);
        drawGrad({ 0, 0 }, { (float)WIN_W, vW }, vE, vC, false);
        drawGrad({ 0, WIN_H - vW }, { (float)WIN_W, vW }, vC, vE, false);
    }

    window_.display();
}

// ──────────────────────────────────────────────────────── draw routines ───────


// ════════════════════════════════════════════════════ drawBackground ══════════
void Game::drawBackground(sf::RenderTarget& rt) {
    // Top HUD panel
    sf::RectangleShape topPanel({ (float)WIN_W, 92.f });
    topPanel.setPosition(0.f, 0.f);
    topPanel.setFillColor(sf::Color(8, 7, 10, 245));
    rt.draw(topPanel);
    // Gold border bottom of top panel
    sf::RectangleShape topBorder({ (float)WIN_W, 2.f });
    topBorder.setPosition(0.f, 91.f);
    topBorder.setFillColor(sf::Color(90, 65, 25, 180));
    rt.draw(topBorder);

    // Bottom HUD panel
    sf::RectangleShape bottomPanel({ (float)WIN_W, 178.f });
    bottomPanel.setPosition(0.f, WIN_H - 178.f);
    bottomPanel.setFillColor(sf::Color(10, 8, 10, 245));
    rt.draw(bottomPanel);
    sf::RectangleShape botBorder({ (float)WIN_W, 2.f });
    botBorder.setPosition(0.f, WIN_H - 178.f);
    botBorder.setFillColor(sf::Color(90, 65, 25, 180));
    rt.draw(botBorder);

    // Table body
    sf::RectangleShape table({ (float)WIN_W, (float)WIN_H * 0.55f });
    table.setPosition(0, 120.f);
    table.setFillColor(sf::Color(24, 15, 8));
    rt.draw(table);

    // Felt surface — layered gradient rows
    for (int i = 0; i < 8; ++i) {
        sf::Uint8 g = (sf::Uint8)(72 + (i < 4 ? i : 8 - i) * 3);
        sf::RectangleShape layer({ (float)WIN_W - 96.f, 300.f / 8.f });
        layer.setPosition(48.f, 180.f + i * (300.f / 8.f));
        layer.setFillColor(sf::Color(22, g, 32));
        rt.draw(layer);
    }
    // Felt border
    sf::RectangleShape feltBorder({ (float)WIN_W - 96.f, 300.f });
    feltBorder.setPosition(48.f, 180.f);
    feltBorder.setFillColor(sf::Color::Transparent);
    feltBorder.setOutlineColor(sf::Color(70, 48, 20));
    feltBorder.setOutlineThickness(4.5f);
    rt.draw(feltBorder);
    // Inner thin edge highlight
    sf::RectangleShape feltInner({ (float)WIN_W - 106.f, 295.f });
    feltInner.setPosition(53.f, 183.f);
    feltInner.setFillColor(sf::Color::Transparent);
    feltInner.setOutlineColor(sf::Color(42, 100, 48, 75));
    feltInner.setOutlineThickness(1.5f);
    rt.draw(feltInner);
    // Grid lines horizontal
    for (int i = 0; i < 8; ++i) {
        sf::RectangleShape line({ (float)WIN_W - 96.f, 1.f });
        line.setPosition(48.f, 180.f + i * (300.f / 8.f));
        line.setFillColor(sf::Color(40, 100, 50, 55));
        rt.draw(line);
    }
    // Grid lines vertical
    for (int i = 1; i < 14; ++i) {
        sf::RectangleShape vline({ 1.f, 300.f });
        vline.setPosition(48.f + i * ((WIN_W - 96.f) / 14.f), 180.f);
        vline.setFillColor(sf::Color(38, 95, 46, 28));
        rt.draw(vline);
    }

    // Ceiling strip
    sf::RectangleShape ceil2({ (float)WIN_W, 88.f });
    ceil2.setPosition(0, 92.f);
    ceil2.setFillColor(sf::Color(11, 9, 14));
    rt.draw(ceil2);

    // Lamp — glow layers + cord + shade + bulb + cone
    const float lampX = WIN_W / 2.f;
    // Light cone on table
    sf::ConvexShape cone;
    cone.setPointCount(4);
    cone.setPoint(0, { lampX - 10.f, 110.f });
    cone.setPoint(1, { lampX + 10.f, 110.f });
    cone.setPoint(2, { lampX + 260.f, 480.f });
    cone.setPoint(3, { lampX - 260.f, 480.f });
    cone.setFillColor(sf::Color(95, 82, 28, 16));
    rt.draw(cone);
    // Outer glow halos
    for (int g = 4; g >= 1; --g) {
        sf::CircleShape halo(g * 38.f);
        halo.setOrigin(halo.getRadius(), halo.getRadius());
        halo.setPosition(lampX, 108.f);
        halo.setFillColor(sf::Color(95, 82, 28, (sf::Uint8)(7 * g)));
        rt.draw(halo);
    }
    // Cord
    sf::RectangleShape cord({ 2.5f, WIN_H * 0.1f });
    cord.setPosition(lampX - 1.25f, 0.f);
    cord.setFillColor(sf::Color(58, 54, 58));
    rt.draw(cord);
    // Lamp shade (trapezoid)
    sf::ConvexShape shade;
    shade.setPointCount(4);
    shade.setPoint(0, { lampX - 20.f, 100.f });
    shade.setPoint(1, { lampX + 20.f, 100.f });
    shade.setPoint(2, { lampX + 28.f, 118.f });
    shade.setPoint(3, { lampX - 28.f, 118.f });
    shade.setFillColor(sf::Color(52, 44, 26));
    shade.setOutlineColor(sf::Color(85, 72, 35));
    shade.setOutlineThickness(1.5f);
    rt.draw(shade);
    // Bulb
    sf::CircleShape lamp(10.f);
    lamp.setOrigin(10.f, 10.f);
    lamp.setPosition(lampX, 108.f);
    lamp.setFillColor(sf::Color(252, 228, 148));
    lamp.setOutlineColor(sf::Color(165, 142, 75));
    lamp.setOutlineThickness(2.f);
    rt.draw(lamp);

    // Table front edge highlight
    sf::RectangleShape tableEdge({ (float)WIN_W, 5.f });
    tableEdge.setPosition(0.f, 478.f);
    tableEdge.setFillColor(sf::Color(50, 32, 14, 160));
    rt.draw(tableEdge);
}

// ════════════════════════════════════════════════════════ drawDealer ══════════
void Game::drawDealer(sf::RenderTarget& rt) {
    const float cx = 270.f;
    const float baseY = 96.f;
    const float ox = cx;
    const int maxHP = dealerMaxHP();
    const int lostHP = std::max(0, maxHP - dealerHP_);

    // Дилер остаётся ЗА столом: рисуем только верхнюю часть тела и голову.

    sf::CircleShape shadow(78.f);
    shadow.setOrigin(78.f, 78.f);
    shadow.setPosition(ox, baseY + 94.f);
    shadow.setScale(1.15f, 0.28f);
    shadow.setFillColor(sf::Color(0, 0, 0, 72));
    rt.draw(shadow);

    sf::ConvexShape shoulders;
    shoulders.setPointCount(6);
    shoulders.setPoint(0, { ox - 92.f, baseY + 118.f });
    shoulders.setPoint(1, { ox - 58.f, baseY + 76.f });
    shoulders.setPoint(2, { ox - 24.f, baseY + 60.f });
    shoulders.setPoint(3, { ox + 24.f, baseY + 60.f });
    shoulders.setPoint(4, { ox + 58.f, baseY + 76.f });
    shoulders.setPoint(5, { ox + 92.f, baseY + 118.f });
    shoulders.setFillColor(sf::Color(12, 11, 16));
    shoulders.setOutlineColor(sf::Color(34, 28, 22));
    shoulders.setOutlineThickness(2.f);
    rt.draw(shoulders);

    for (int s : { -1, 1 }) {
        sf::ConvexShape lapel;
        lapel.setPointCount(4);
        lapel.setPoint(0, { ox + s * 12.f, baseY + 62.f });
        lapel.setPoint(1, { ox + s * 26.f, baseY + 62.f });
        lapel.setPoint(2, { ox + s * 31.f, baseY + 92.f });
        lapel.setPoint(3, { ox + s * 8.f,  baseY + 86.f });
        lapel.setFillColor(sf::Color(52, 18, 18));
        rt.draw(lapel);
    }

    sf::RectangleShape chest({ 96.f, 72.f });
    chest.setOrigin(48.f, 0.f);
    chest.setPosition(ox, baseY + 82.f);
    chest.setFillColor(sf::Color(18, 17, 22));
    chest.setOutlineColor(sf::Color(40, 34, 28));
    chest.setOutlineThickness(1.5f);
    rt.draw(chest);

    sf::RectangleShape shirt({ 18.f, 66.f });
    shirt.setOrigin(9.f, 0.f);
    shirt.setPosition(ox, baseY + 82.f);
    shirt.setFillColor(sf::Color(186, 178, 164));
    rt.draw(shirt);

    sf::ConvexShape tie;
    tie.setPointCount(5);
    tie.setPoint(0, { ox - 9.f, baseY + 84.f });
    tie.setPoint(1, { ox + 9.f, baseY + 84.f });
    tie.setPoint(2, { ox + 6.f, baseY + 136.f });
    tie.setPoint(3, { ox + 0.f, baseY + 148.f });
    tie.setPoint(4, { ox - 6.f, baseY + 136.f });
    tie.setFillColor(sf::Color(42, 14, 14));
    rt.draw(tie);

    sf::ConvexShape knot;
    knot.setPointCount(4);
    knot.setPoint(0, { ox - 10.f, baseY + 82.f });
    knot.setPoint(1, { ox + 10.f, baseY + 82.f });
    knot.setPoint(2, { ox + 7.f,  baseY + 92.f });
    knot.setPoint(3, { ox - 7.f,  baseY + 92.f });
    knot.setFillColor(sf::Color(28, 10, 10));
    rt.draw(knot);

    sf::RectangleShape neck({ 30.f, 24.f });
    neck.setOrigin(15.f, 0.f);
    neck.setPosition(ox, baseY + 52.f);
    neck.setFillColor(sf::Color(136, 116, 98));
    rt.draw(neck);

    sf::ConvexShape collar;
    collar.setPointCount(6);
    collar.setPoint(0, { ox - 15.f, baseY + 52.f });
    collar.setPoint(1, { ox - 24.f, baseY + 64.f });
    collar.setPoint(2, { ox - 10.f, baseY + 78.f });
    collar.setPoint(3, { ox + 10.f, baseY + 78.f });
    collar.setPoint(4, { ox + 24.f, baseY + 64.f });
    collar.setPoint(5, { ox + 15.f, baseY + 52.f });
    collar.setFillColor(sf::Color(188, 180, 165));
    rt.draw(collar);

    sf::CircleShape head(44.f);
    head.setOrigin(44.f, 44.f);
    head.setPosition(ox, baseY + 22.f);
    head.setFillColor(sf::Color(126, 108, 90));
    head.setOutlineColor(sf::Color(68, 52, 38));
    head.setOutlineThickness(2.f);
    rt.draw(head);

    sf::RectangleShape browShade({ 82.f, 11.f });
    browShade.setOrigin(41.f, 5.5f);
    browShade.setPosition(ox, baseY - 12.f);
    browShade.setFillColor(sf::Color(38, 26, 16, 170));
    rt.draw(browShade);

    sf::ConvexShape hair;
    hair.setPointCount(6);
    hair.setPoint(0, { ox - 44.f, baseY - 18.f });
    hair.setPoint(1, { ox - 26.f, baseY - 30.f });
    hair.setPoint(2, { ox + 0.f,  baseY - 34.f });
    hair.setPoint(3, { ox + 26.f, baseY - 30.f });
    hair.setPoint(4, { ox + 44.f, baseY - 18.f });
    hair.setPoint(5, { ox + 0.f,  baseY - 10.f });
    hair.setFillColor(sf::Color(16, 12, 8));
    rt.draw(hair);

    for (int s : { -1, 1 }) {
        sf::ConvexShape brow;
        brow.setPointCount(4);
        brow.setPoint(0, { ox + s * 10.f, baseY - 4.f });
        brow.setPoint(1, { ox + s * 28.f, baseY - 12.f });
        brow.setPoint(2, { ox + s * 28.f, baseY - 8.f });
        brow.setPoint(3, { ox + s * 10.f, baseY + 0.f });
        brow.setFillColor(sf::Color(18, 12, 6));
        rt.draw(brow);
    }

    auto drawEye = [&](float dx) {
        sf::CircleShape socket(13.f);
        socket.setOrigin(13.f, 13.f);
        socket.setPosition(ox + dx, baseY + 8.f);
        socket.setFillColor(sf::Color(10, 8, 8));
        rt.draw(socket);

        sf::CircleShape sclera(9.f);
        sclera.setOrigin(9.f, 9.f);
        sclera.setPosition(ox + dx, baseY + 8.f);
        sclera.setFillColor(sf::Color(198, 182, 160));
        rt.draw(sclera);

        sf::CircleShape iris(6.f);
        iris.setOrigin(6.f, 6.f);
        iris.setPosition(ox + dx, baseY + 8.f);
        iris.setFillColor(sf::Color(138, 28, 20));
        rt.draw(iris);

        sf::CircleShape pupil(3.5f);
        pupil.setOrigin(3.5f, 3.5f);
        pupil.setPosition(ox + dx, baseY + 8.f);
        pupil.setFillColor(sf::Color(6, 4, 4));
        rt.draw(pupil);

        sf::CircleShape glare(1.8f);
        glare.setOrigin(1.8f, 1.8f);
        glare.setPosition(ox + dx + 2.5f, baseY + 2.f);
        glare.setFillColor(sf::Color(255, 212, 190, 165));
        rt.draw(glare);
        };

    drawEye(-20.f);
    drawEye(20.f);

    sf::ConvexShape nose;
    nose.setPointCount(5);
    nose.setPoint(0, { ox - 4.f, baseY + 16.f });
    nose.setPoint(1, { ox + 4.f, baseY + 16.f });
    nose.setPoint(2, { ox + 7.f, baseY + 30.f });
    nose.setPoint(3, { ox + 0.f, baseY + 33.f });
    nose.setPoint(4, { ox - 7.f, baseY + 30.f });
    nose.setFillColor(sf::Color(110, 92, 76));
    nose.setOutlineColor(sf::Color(58, 42, 28));
    nose.setOutlineThickness(1.f);
    rt.draw(nose);

    sf::ConvexShape mouth;
    mouth.setPointCount(6);
    mouth.setPoint(0, { ox - 24.f, baseY + 40.f });
    mouth.setPoint(1, { ox - 10.f, baseY + 36.f });
    mouth.setPoint(2, { ox + 0.f,  baseY + 38.f });
    mouth.setPoint(3, { ox + 10.f, baseY + 36.f });
    mouth.setPoint(4, { ox + 24.f, baseY + 40.f });
    mouth.setPoint(5, { ox + 0.f,  baseY + 47.f });
    mouth.setFillColor(sf::Color(28, 8, 8));
    rt.draw(mouth);

    const float toothW[5] = { 8.f, 7.f, 6.f, 7.f, 8.f };
    const float toothH[5] = { 7.f, 8.f, 6.f, 8.f, 7.f };
    float startX = ox - 19.f;
    for (int i = 0; i < 5; ++i) {
        sf::RectangleShape tooth({ toothW[i], toothH[i] });
        tooth.setPosition(startX, baseY + 40.f);
        tooth.setFillColor(sf::Color(208, 194, 138));
        tooth.setOutlineColor(sf::Color(78, 58, 28));
        tooth.setOutlineThickness(0.8f);
        rt.draw(tooth);
        startX += toothW[i] + 1.5f;
    }

    // Базовый шрам оставляем
    sf::RectangleShape scarLine({ 3.f, 28.f });
    scarLine.setOrigin(1.5f, 0.f);
    scarLine.setRotation(-40.f);
    scarLine.setPosition(ox + 36.f, baseY + 4.f);
    scarLine.setFillColor(sf::Color(148, 68, 62, 180));
    rt.draw(scarLine);

    for (int i = 0; i < 4; ++i) {
        sf::RectangleShape st({ 5.f, 1.5f });
        st.setOrigin(2.5f, 0.75f);
        st.setRotation(50.f);
        st.setPosition(ox + 31.f + i * 4.5f, baseY + 8.f + i * 5.5f);
        st.setFillColor(sf::Color(72, 38, 35, 150));
        rt.draw(st);
    }

    // Увечья по потерянному HP
    if (lostHP >= 1) {
        sf::CircleShape bruise(10.f);
        bruise.setOrigin(10.f, 10.f);
        bruise.setPosition(ox - 21.f, baseY + 14.f);
        bruise.setScale(1.35f, 1.0f);
        bruise.setFillColor(sf::Color(72, 26, 62, 116));
        rt.draw(bruise);

        sf::CircleShape eyeBlood(5.f);
        eyeBlood.setOrigin(5.f, 5.f);
        eyeBlood.setPosition(ox - 18.f, baseY + 8.f);
        eyeBlood.setFillColor(sf::Color(120, 20, 18, 78));
        rt.draw(eyeBlood);
    }

    if (lostHP >= 2) {
        sf::RectangleShape cut({ 2.5f, 24.f });
        cut.setOrigin(1.25f, 0.f);
        cut.setRotation(24.f);
        cut.setPosition(ox - 31.f, baseY + 22.f);
        cut.setFillColor(sf::Color(152, 32, 28, 205));
        rt.draw(cut);

        for (int i = 0; i < 3; ++i) {
            sf::RectangleShape blood({ 4.5f, 6.f + i * 2.f });
            blood.setOrigin(2.25f, 0.f);
            blood.setPosition(ox - 26.f + i * 2.f, baseY + 34.f + i * 4.5f);
            blood.setFillColor(sf::Color(118, 16, 14, 158));
            rt.draw(blood);
        }
    }

    if (lostHP >= 3) {
        sf::CircleShape lipBruise(8.f);
        lipBruise.setOrigin(8.f, 8.f);
        lipBruise.setPosition(ox + 17.f, baseY + 42.f);
        lipBruise.setScale(1.45f, 0.82f);
        lipBruise.setFillColor(sf::Color(82, 22, 52, 100));
        rt.draw(lipBruise);

        sf::RectangleShape crack({ 20.f, 2.f });
        crack.setOrigin(10.f, 1.f);
        crack.setRotation(-8.f);
        crack.setPosition(ox + 11.f, baseY + 43.f);
        crack.setFillColor(sf::Color(138, 30, 26, 175));
        rt.draw(crack);
    }

    if (lostHP >= 4) {
        sf::RectangleShape foreheadCut({ 3.f, 22.f });
        foreheadCut.setOrigin(1.5f, 0.f);
        foreheadCut.setRotation(-18.f);
        foreheadCut.setPosition(ox + 6.f, baseY - 10.f);
        foreheadCut.setFillColor(sf::Color(156, 40, 34, 214));
        rt.draw(foreheadCut);

        sf::RectangleShape bloodTrail({ 4.f, 16.f });
        bloodTrail.setOrigin(2.f, 0.f);
        bloodTrail.setRotation(-8.f);
        bloodTrail.setPosition(ox + 4.f, baseY + 4.f);
        bloodTrail.setFillColor(sf::Color(110, 12, 12, 144));
        rt.draw(bloodTrail);
    }

    // Маска стола спереди: она частично перекрывает корпус и возвращает дилера «за стол».
    sf::RectangleShape tableMask({ 360.f, 300.f });
    tableMask.setPosition(48.f, 180.f);
    tableMask.setFillColor(sf::Color::Transparent);

    for (int i = 0; i < 8; ++i) {
        sf::Uint8 g = static_cast<sf::Uint8>(72 + ((i < 4) ? i * 4 : (8 - i) * 3));
        sf::RectangleShape layer({ 360.f, 300.f / 8.f });
        layer.setPosition(48.f, 180.f + i * (300.f / 8.f));
        layer.setFillColor(sf::Color(22, g, 32));
        rt.draw(layer);
    }

    for (int i = 0; i <= 8; ++i) {
        sf::RectangleShape line({ 360.f, 1.f });
        line.setPosition(48.f, 180.f + i * (300.f / 8.f));
        line.setFillColor(sf::Color(40, 100, 50, 55));
        rt.draw(line);
    }

    sf::RectangleShape topEdge({ 360.f, 4.f });
    topEdge.setPosition(48.f, 180.f);
    topEdge.setFillColor(sf::Color(68, 44, 20, 180));
    rt.draw(topEdge);

    sf::RectangleShape innerEdge({ 360.f, 1.5f });
    innerEdge.setPosition(48.f, 184.f);
    innerEdge.setFillColor(sf::Color(120, 90, 38, 90));
    rt.draw(innerEdge);
}

// ════════════════════════════════════════════════════════ drawShotgun ════════
void Game::drawShotgun(sf::RenderTarget& rt) {
    float cx = WIN_W / 2.f;
    float cy = WIN_H / 2.f + 10.f;

    float kick = 0.f;
    if (state_ == GameState::SHOOT_SELF || state_ == GameState::SHOOT_OPPONENT) {
        float t = clamp01(animTimer_ / ANIM_DURATION);
        kick = Easing::easeOutCubic(std::min(t * 3.f, 1.f)) *
            Easing::easeInQuad(1.f - t) * 25.f;
    }
    float bx = cx + kick;

    // Shadow on felt
    sf::RectangleShape gunShadow({ 200.f, 10.f });
    gunShadow.setOrigin(100.f, 5.f);
    gunShadow.setPosition(cx, cy + 18.f);
    gunShadow.setFillColor(sf::Color(0, 0, 0, 55));
    rt.draw(gunShadow);

    // Under-barrel tube
    sf::RectangleShape tube({ 170.f, 8.f });
    tube.setOrigin(85.f, 4.f);
    tube.setPosition(bx - 10.f, cy + 6.f);
    tube.setFillColor(sf::Color(52, 48, 44));
    tube.setOutlineColor(sf::Color(74, 68, 60));
    tube.setOutlineThickness(1.f);
    rt.draw(tube);

    // Pump
    sf::RectangleShape pump({ 100.f, 12.f });
    pump.setOrigin(50.f, 6.f);
    pump.setPosition(bx - 10.f, cy + 6.f);
    pump.setFillColor(sf::Color(88, 68, 48));
    pump.setOutlineColor(sf::Color(60, 44, 26));
    pump.setOutlineThickness(1.f);
    rt.draw(pump);

    // Main barrel
    sf::RectangleShape barrel({ 180.f, 15.f });
    barrel.setOrigin(90.f, 7.5f);
    barrel.setPosition(bx, cy - 10.f);
    barrel.setFillColor(kick > 0.f ? sf::Color(88, 78, 66) : sf::Color(68, 62, 56));
    barrel.setOutlineColor(sf::Color(100, 90, 78));
    barrel.setOutlineThickness(1.5f);
    rt.draw(barrel);
    // Barrel top rib
    sf::RectangleShape rib({ 176.f, 3.f });
    rib.setOrigin(88.f, 1.5f);
    rib.setPosition(bx, cy - 17.f);
    rib.setFillColor(sf::Color(50, 46, 42));
    rt.draw(rib);

    // Receiver
    sf::RectangleShape recv({ 44.f, 20.f });
    recv.setOrigin(0.f, 10.f);
    recv.setPosition(bx + 40.f, cy - 10.f);
    recv.setFillColor(sf::Color(58, 52, 46));
    recv.setOutlineColor(sf::Color(85, 76, 64));
    recv.setOutlineThickness(1.f);
    rt.draw(recv);

    // Stock (wood)
    sf::RectangleShape stock({ 60.f, 20.f });
    stock.setOrigin(0.f, 10.f);
    stock.setPosition(bx + 90.f, cy - 5.f);
    stock.setFillColor(sf::Color(98, 60, 24));
    stock.setOutlineColor(sf::Color(68, 42, 12));
    stock.setOutlineThickness(1.f);
    rt.draw(stock);
    // Wood grain lines
    for (int g = 0; g < 2; ++g) {
        sf::RectangleShape grain({ 58.f, 2.f });
        grain.setOrigin(0.f, 1.f);
        grain.setPosition(bx + 91.f, cy - 5.f + g * 7.f);
        grain.setFillColor(sf::Color(112, 68, 26, 55));
        rt.draw(grain);
    }

    // Pistol grip
    sf::RectangleShape grip({ 18.f, 30.f });
    grip.setOrigin(9.f, 0.f);
    grip.setPosition(bx + 50.f, cy - 7.f);
    grip.setFillColor(sf::Color(88, 52, 18));
    grip.setOutlineColor(sf::Color(60, 36, 10));
    grip.setOutlineThickness(1.f);
    rt.draw(grip);

    // Muzzle cap
    sf::RectangleShape muzzle({ 9.f, 22.f });
    muzzle.setOrigin(4.5f, 11.f);
    muzzle.setPosition(bx - 90.f, cy - 10.f);
    muzzle.setFillColor(sf::Color(44, 42, 40));
    rt.draw(muzzle);

    // Trigger guard
    sf::ConvexShape guard;
    guard.setPointCount(4);
    guard.setPoint(0, { bx + 40.f, cy - 2.f });
    guard.setPoint(1, { bx + 50.f, cy - 2.f });
    guard.setPoint(2, { bx + 50.f, cy + 18.f });
    guard.setPoint(3, { bx + 40.f, cy + 18.f });
    guard.setFillColor(sf::Color(48, 44, 40));
    rt.draw(guard);

    // Muzzle flash when firing live
    if (kick > 5.f && firedShell_ == ShellType::LIVE) {
        float fa = 1.f - kick / 25.f;
        for (int f = 3; f >= 1; --f) {
            sf::CircleShape flash(8.f + f * 5.f + kick * 0.25f);
            flash.setOrigin(flash.getRadius(), flash.getRadius());
            flash.setPosition(bx - 96.f, cy - 10.f);
            flash.setFillColor(sf::Color(255, 210 - f * 20, 60,
                (sf::Uint8)(fa * 175.f / f)));
            rt.draw(flash);
        }
    }
}

// ════════════════════════════════════════════════════ drawHealthBars ══════════
void Game::drawHealthBars(sf::RenderTarget& rt) {
    auto drawHP = [&](float x, float y, int hp, int maxhp,
        const std::string& name, sf::Color barCol) {
            // Name label with shadow
            sf::Text shadow2 = makeText(name, 16, sf::Color(0, 0, 0, 100));
            shadow2.setPosition(x + 1.f, y - 21.f);
            rt.draw(shadow2);
            sf::Text nameT = makeText(name, 16, textCream);
            nameT.setPosition(x, y - 22.f);
            rt.draw(nameT);

            for (int i = 0; i < maxhp; ++i) {
                bool alive = (i < hp);
                // Drop shadow
                sf::RectangleShape sh({ 22.f, 22.f });
                sh.setPosition(x + i * 26.f + 1.f, y + 1.f);
                sh.setFillColor(sf::Color(0, 0, 0, 65));
                rt.draw(sh);
                // Pip
                sf::RectangleShape pip({ 22.f, 22.f });
                pip.setPosition(x + i * 26.f, y);
                if (alive) {
                    pip.setFillColor(barCol);
                    pip.setOutlineColor(sf::Color(
                        std::min(255, (int)barCol.r + 45),
                        std::min(255, (int)barCol.g + 45),
                        std::min(255, (int)barCol.b + 45)));
                }
                else {
                    pip.setFillColor(sf::Color(28, 24, 20));
                    pip.setOutlineColor(sf::Color(55, 48, 38));
                }
                pip.setOutlineThickness(1.5f);
                rt.draw(pip);
                // Top glint on active pips
                if (alive) {
                    sf::RectangleShape glint({ 16.f, 4.f });
                    glint.setPosition(x + i * 26.f + 3.f, y + 3.f);
                    glint.setFillColor(sf::Color(255, 255, 255, 24));
                    rt.draw(glint);
                }
            }
            // Fraction
            sf::Text hpT = makeText(std::to_string(hp) + "/" + std::to_string(maxhp),
                14, sf::Color(168, 158, 135));
            hpT.setPosition(x + maxhp * 26.f + 6.f, y + 4.f);
            rt.draw(hpT);
        };

    drawHP(40.f, WIN_H - 42.f, playerHP_, playerMaxHP(), "ИГРОК",
        sf::Color(72, 198, 78));
    drawHP(40.f, 44.f, dealerHP_, dealerMaxHP(), "ДИЛЕР",
        sf::Color(198, 72, 72));

    // Score
    std::ostringstream oss;
    oss << playerWins_ << " - " << dealerWins_;
    sf::Text scoreSh = makeText(oss.str(), 22, sf::Color(0, 0, 0, 120));
    { sf::FloatRect b = scoreSh.getLocalBounds(); scoreSh.setOrigin(b.left + b.width / 2.f, 0.f); }
    scoreSh.setPosition(WIN_W / 2.f + 1.f, 13.f);
    rt.draw(scoreSh);
    sf::Text scoreT = makeText(oss.str(), 22, gold);
    { sf::FloatRect b = scoreT.getLocalBounds(); scoreT.setOrigin(b.left + b.width / 2.f, 0.f); }
    scoreT.setPosition(WIN_W / 2.f, 12.f);
    rt.draw(scoreT);

    sf::Text rT = makeText("Раунд " + std::to_string(round_), 16, textCream);
    { sf::FloatRect b = rT.getLocalBounds(); rT.setOrigin(b.left + b.width / 2.f, 0.f); }
    rT.setPosition(WIN_W / 2.f, 42.f);
    rt.draw(rT);

    sf::Text diffT = makeText("Сложность: " + difficultyName(), 13, sf::Color(165, 155, 130));
    { sf::FloatRect b = diffT.getLocalBounds(); diffT.setOrigin(b.left + b.width / 2.f, 0.f); }
    diffT.setPosition(WIN_W / 2.f, 66.f);
    rt.draw(diffT);
}

// ════════════════════════════════════════════════════════ drawShells ══════════
void Game::drawShells(sf::RenderTarget& rt) {
    // Panel background — фиксированная минимальная ширина 200px
    float panW = std::max(200.f, gun_.total() * 28.f + 24.f);
    sf::RectangleShape panel({ panW, 46.f });
    panel.setOrigin(panW / 2.f, 0.f);
    panel.setPosition(WIN_W / 2.f, WIN_H / 2.f - 117.f);
    panel.setFillColor(sf::Color(10, 10, 14, 180));
    panel.setOutlineColor(sf::Color(75, 60, 38));
    panel.setOutlineThickness(1.5f);
    rt.draw(panel);

    sf::Text lbl = makeText("Магазин:", 15, sf::Color(155, 148, 130));
    { sf::FloatRect lb = lbl.getLocalBounds(); lbl.setOrigin(lb.left + lb.width / 2.f, 0.f); }
    lbl.setPosition(WIN_W / 2.f, WIN_H / 2.f - 112.f);
    rt.draw(lbl);

    std::ostringstream oss;
    oss << "Б " << gun_.liveRemaining() << " / Х " << gun_.blankRemaining();
    sf::Text countT = makeText(oss.str(), 13, sf::Color(145, 145, 155));
    { sf::FloatRect cb = countT.getLocalBounds(); countT.setOrigin(cb.left + cb.width / 2.f, 0.f); }
    countT.setPosition(WIN_W / 2.f, WIN_H / 2.f - 94.f);
    rt.draw(countT);

    shellRow_.draw(rt);
}

// ════════════════════════════════════════════════════════ drawItems ══════════
void Game::drawItems(sf::RenderTarget& rt) {
    // ── Player items panel — точно по слотам (x=40, y=WIN_H-154, slot=136x52, step=148) ──
    // Диапазон слотов: x 40..1212 (8*148-12=1172), y: WIN_H-154..WIN_H-102
    sf::RectangleShape pPanel({ 1172.f, 52.f });
    pPanel.setPosition(40.f, WIN_H - 154.f);
    pPanel.setFillColor(sf::Color(10, 14, 10, 0));          // прозрачная заливка
    pPanel.setOutlineColor(sf::Color(44, 72, 42, 180));
    pPanel.setOutlineThickness(1.5f);
    rt.draw(pPanel);

    sf::Text pLbl = makeText("ТВОИ ПРЕДМЕТЫ:", 14, sf::Color(125, 195, 130));
    pLbl.setPosition(40.f, WIN_H - 170.f);
    rt.draw(pLbl);

    // ── Dealer items panel — точно по слотам (x=760, y=24, slot=78x28, step=86/34) ──
    // Диапазон: x 760..1096 (4*86-8=336), y: 24..86 (2*34-6=62)
    sf::RectangleShape dPanel({ 336.f, 62.f });
    dPanel.setPosition(760.f, 24.f);
    dPanel.setFillColor(sf::Color(14, 10, 10, 0));          // прозрачная заливка
    dPanel.setOutlineColor(sf::Color(72, 42, 42, 180));
    dPanel.setOutlineThickness(1.5f);
    rt.draw(dPanel);

    sf::Text dLbl = makeText("ПРЕДМЕТЫ ДИЛЕРА:", 14, sf::Color(195, 130, 125));
    dLbl.setPosition(760.f, 6.f);
    rt.draw(dLbl);

    for (auto& s : playerSlots_) s.draw(rt);
    for (auto& s : dealerSlots_) s.draw(rt);

    if (adrenalineMode_ && !adrenalineIsDealer_) {
        sf::RectangleShape hint_bg({ 490.f, 34.f });
        hint_bg.setOrigin(245.f, 17.f);
        hint_bg.setPosition(WIN_W / 2.f - 160.f + 245.f, WIN_H / 2.f + 57.f);
        hint_bg.setFillColor(sf::Color(40, 8, 50, 210));
        hint_bg.setOutlineColor(sf::Color(200, 80, 220));
        hint_bg.setOutlineThickness(2.f);
        rt.draw(hint_bg);
        sf::Text hint = makeText("АДРЕНАЛИН: выбери предмет для кражи!", 18,
            sf::Color(255, 108, 255));
        { sf::FloatRect b = hint.getLocalBounds(); hint.setOrigin(b.width / 2.f, b.height / 2.f); }
        hint.setPosition(WIN_W / 2.f - 160.f + 245.f, WIN_H / 2.f + 57.f);
        rt.draw(hint);
    }
}

void Game::drawHUD(sf::RenderTarget& rt) {
    std::string turnStr;
    sf::Color   turnCol;
    bool isAnimating = (state_ == GameState::SHOOT_SELF ||
        state_ == GameState::SHOOT_OPPONENT);

    if (isAnimating) {
        turnStr = (firedShell_ == ShellType::LIVE) ? ">>> BANG! <<<" : ">>> click... <<<";
        turnCol = (firedShell_ == ShellType::LIVE) ? liveRed : blankBlue;
    }
    else if (state_ == GameState::DEALER_TURN) {
        turnStr = "ХОД ДИЛЕРА";
        turnCol = sf::Color(255, 180, 100);
    }
    else if (state_ == GameState::PLAYER_TURN) {
        turnStr = "ТВОЙ ХОД";
        turnCol = sf::Color(100, 220, 100);
    }
    else {
        turnStr = "";
        turnCol = textCream;
    }

    if (!turnStr.empty()) {
        // Background pill behind turn text
        sf::Text tmp = makeText(turnStr, 24, sf::Color::White);
        float tw2 = tmp.getLocalBounds().width + 36.f;
        sf::RectangleShape pill({ tw2, 32.f });
        pill.setOrigin(tw2 / 2.f, 16.f);
        pill.setPosition(WIN_W / 2.f, 162.f);
        pill.setFillColor(sf::Color(turnCol.r / 6, turnCol.g / 6, turnCol.b / 6, 200));
        pill.setOutlineColor(sf::Color(turnCol.r, turnCol.g, turnCol.b, 150));
        pill.setOutlineThickness(1.5f);
        rt.draw(pill);

        sf::Text tsh = makeText(turnStr, 24, sf::Color(0, 0, 0, 130));
        { sf::FloatRect b = tsh.getLocalBounds(); tsh.setOrigin(b.width / 2.f, b.height / 2.f); }
        tsh.setPosition(WIN_W / 2.f + 1.f, 162.f + 1.f);
        rt.draw(tsh);
        sf::Text tT = makeText(turnStr, 24, turnCol);
        { sf::FloatRect b = tT.getLocalBounds(); tT.setOrigin(b.width / 2.f, b.height / 2.f); }
        tT.setPosition(WIN_W / 2.f, 162.f);
        rt.draw(tT);
    }

    if (state_ == GameState::PLAYER_TURN) {
        btnShootPlayer_.draw(rt);
        btnShootSelf_.draw(rt);
        btnMenu_.draw(rt);
        if (gun_.knifeActive) {
            // Badge правее кнопок стрельбы (btnShootSelf правый край ~910), выровнен по их высоте
            sf::RectangleShape kbg({ 210.f, 48.f });
            kbg.setPosition(926.f, WIN_H - 74.f);
            kbg.setFillColor(sf::Color(46, 20, 5, 215));
            kbg.setOutlineColor(sf::Color(205, 115, 28));
            kbg.setOutlineThickness(1.5f);
            rt.draw(kbg);
            sf::Text kt = makeText("НОЖ АКТИВЕН (x2)", 16, sf::Color(255, 148, 40));
            { sf::FloatRect b = kt.getLocalBounds(); kt.setOrigin(b.width / 2.f, b.height / 2.f); }
            kt.setPosition(926.f + 105.f, WIN_H - 74.f + 24.f);
            rt.draw(kt);
        }
    }
    else if (state_ == GameState::DEALER_TURN) {
        btnMenu_.draw(rt);
        int dots = (int)(aiThinkTimer_ * 4.f) % 4;
        std::string dStr = "Дилер думает";
        for (int i = 0; i < dots; ++i) dStr += ".";
        sf::Text dt2 = makeText(dStr, 16, sf::Color(195, 175, 135));
        dt2.setPosition(WIN_W / 2.f - 70.f, WIN_H - 52.f);
        rt.draw(dt2);
    }
    else {
        btnMenu_.draw(rt);
    }

    // Handcuff labels with background
    auto drawCuff = [&](float x, float y) {
        sf::RectangleShape cbg({ 145.f, 20.f });
        cbg.setPosition(x - 2.f, y - 2.f);
        cbg.setFillColor(sf::Color(44, 36, 5, 200));
        cbg.setOutlineColor(sf::Color(212, 178, 38));
        cbg.setOutlineThickness(1.5f);
        rt.draw(cbg);
        sf::Text hc = makeText("[НАРУЧНИКИ]", 14, sf::Color(255, 200, 60));
        hc.setPosition(x, y);
        rt.draw(hc);
        };
    if (playerHandcuffed_) drawCuff(168.f, WIN_H - 42.f);
    if (dealerHandcuffed_) drawCuff(40.f, 78.f);
}

// ════════════════════════════════════════════════════ drawMainMenu ══════════
void Game::drawMainMenu(sf::RenderTarget& rt) {
    sf::RectangleShape bg({ (float)WIN_W, (float)WIN_H });
    bg.setFillColor(sf::Color(7, 5, 11));
    rt.draw(bg);

    for (int i = 0; i < 20; ++i) {
        sf::RectangleShape line({ (float)WIN_W, 1.f });
        line.setPosition(0, i * (WIN_H / 20.f));
        line.setFillColor(sf::Color(28, 22, 38, 55));
        rt.draw(line);
    }

    // Gold top/bot bars
    for (float y2 : {0.f, (float)WIN_H - 3.f}) {
        sf::RectangleShape bar({ (float)WIN_W, 3.f });
        bar.setPosition(0.f, y2);
        bar.setFillColor(gold);
        rt.draw(bar);
    }

    // Glow behind title
    for (int g = 3; g >= 1; --g) {
        sf::RectangleShape glow({ (float)WIN_W * 0.6f, (float)g * 18.f });
        glow.setOrigin(glow.getSize().x / 2.f, glow.getSize().y / 2.f);
        glow.setPosition(WIN_W / 2.f, WIN_H * 0.15f + 50.f);
        glow.setFillColor(sf::Color(110, 75, 18, (sf::Uint8)(9 * g)));
        rt.draw(glow);
    }

    // БАКШОТ — with shadow
    {
        sf::Text sh = makeText("БАКШОТ", 72, sf::Color(0, 0, 0, 160));
        sf::FloatRect b = sh.getLocalBounds(); sh.setOrigin(b.width / 2.f, 0.f);
        sh.setPosition(WIN_W / 2.f + 3.f, WIN_H * 0.15f + 3.f);
        rt.draw(sh);
        sf::Text title = makeText("БАКШОТ", 72, gold);
        b = title.getLocalBounds(); title.setOrigin(b.width / 2.f, 0.f);
        title.setPosition(WIN_W / 2.f, WIN_H * 0.15f);
        rt.draw(title);
    }
    // РУЛЕТКА — with shadow
    {
        sf::Text sh = makeText("РУЛЕТКА", 52, sf::Color(0, 0, 0, 145));
        sf::FloatRect b = sh.getLocalBounds(); sh.setOrigin(b.width / 2.f, 0.f);
        sh.setPosition(WIN_W / 2.f + 2.f, WIN_H * 0.15f + 82.f + 2.f);
        rt.draw(sh);
        sf::Text sub = makeText("РУЛЕТКА", 52, sf::Color(180, 80, 60));
        b = sub.getLocalBounds(); sub.setOrigin(b.width / 2.f, 0.f);
        sub.setPosition(WIN_W / 2.f, WIN_H * 0.15f + 82.f);
        rt.draw(sub);
    }

    // Divider line
    sf::RectangleShape div({ 480.f, 2.f });
    div.setOrigin(240.f, 1.f);
    div.setPosition(WIN_W / 2.f, WIN_H * 0.15f + 148.f);
    div.setFillColor(sf::Color(gold.r, gold.g, gold.b, 145));
    rt.draw(div);

    // Shell decorations — with glow rings (same positions as original)
    for (int i = 0; i < 6; ++i) {
        bool isLive = (i % 2 == 0);
        float sx = WIN_W / 2.f - 90.f + i * 36.f;
        float sy = WIN_H * 0.4f;
        sf::CircleShape glow(18.f);
        glow.setOrigin(18.f, 18.f); glow.setPosition(sx, sy);
        glow.setFillColor(isLive ? sf::Color(175, 36, 26, 38) : sf::Color(26, 36, 155, 38));
        rt.draw(glow);
        sf::CircleShape c(16.f);
        c.setOrigin(16.f, 16.f); c.setPosition(sx, sy);
        c.setFillColor(isLive ? liveRed : blankBlue);
        c.setOutlineColor(sf::Color(205, 182, 138)); c.setOutlineThickness(2.f);
        rt.draw(c);
        sf::Text lbl = makeText(isLive ? "Б" : "Х", 14, sf::Color::White);
        sf::FloatRect b = lbl.getLocalBounds();
        lbl.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        lbl.setPosition(sx, sy); rt.draw(lbl);
    }

    // Info lines (same y positions as original)
    std::vector<std::string> lines2 = {
        "Выбери сложность:", "",
        "Лёгкая: у тебя 5 жизней, у дилера 3, дилер чаще ошибается.",
        "Нормальная: классический баланс 4 на 4.",
        "Сложная: у дилера 5 жизней, больше предметов и почти нет ошибок.",
        "", "Стреляй в дилера или в себя: холостой в себя сохраняет ход.",
        "1-й раунд без предметов. Со 2-го раунда предметы случайные.",
        "", "Нажми кнопку сложности, чтобы начать"
    };
    float y2 = WIN_H * 0.47f;
    for (const auto& line : lines2) {
        sf::Color col = line.empty() ? sf::Color::Transparent : textCream;
        if (line == "Выбери сложность:" || line == "Нажми кнопку сложности, чтобы начать")
            col = gold;
        sf::Text t = makeText(line, 16, col);
        sf::FloatRect b = t.getLocalBounds(); t.setOrigin(b.width / 2.f, 0.f);
        t.setPosition(WIN_W / 2.f, y2);
        rt.draw(t);
        y2 += 24.f;
    }

    btnEasy_.draw(rt);
    btnNormal_.draw(rt);
    btnHard_.draw(rt);

    if (!fontLoaded_) {
        sf::Text warn = makeText("ВНИМАНИЕ: системный шрифт не найден. Текст может быть невидимым.",
            14, sf::Color(255, 100, 100));
        warn.setPosition(10.f, WIN_H - 24.f);
        rt.draw(warn);
    }
}

// ════════════════════════════════════════════════════ drawRoundEnd ══════════
void Game::drawRoundEnd(sf::RenderTarget& rt) {
    sf::RectangleShape overlay({ (float)WIN_W, (float)WIN_H });
    overlay.setFillColor(sf::Color(0, 0, 0, 168));
    rt.draw(overlay);

    bool playerWon = (roundResult_ == RoundResult::PLAYER_WIN);
    sf::Color resultCol = playerWon ? sf::Color(72, 255, 80) : sf::Color(255, 56, 56);

    // Glow behind text
    for (int g = 3; g >= 1; --g) {
        sf::RectangleShape glow({ 600.f, (float)(g * 16) });
        glow.setOrigin(300.f, g * 8.f);
        glow.setPosition(WIN_W / 2.f, WIN_H / 2.f - 40.f);
        glow.setFillColor(sf::Color(resultCol.r, resultCol.g, resultCol.b, (sf::Uint8)(10 * g)));
        rt.draw(glow);
    }

    sf::Text rsh = makeText(playerWon ? "РАУНД ВЫИГРАН!" : "РАУНД ПРОИГРАН", 56,
        sf::Color(0, 0, 0, 165));
    { sf::FloatRect b = rsh.getLocalBounds(); rsh.setOrigin(b.width / 2.f, b.height / 2.f); }
    rsh.setPosition(WIN_W / 2.f + 3.f, WIN_H / 2.f - 40.f + 3.f);
    rt.draw(rsh);
    sf::Text result = makeText(playerWon ? "РАУНД ВЫИГРАН!" : "РАУНД ПРОИГРАН", 56, resultCol);
    { sf::FloatRect b = result.getLocalBounds(); result.setOrigin(b.width / 2.f, b.height / 2.f); }
    result.setPosition(WIN_W / 2.f, WIN_H / 2.f - 40.f);
    rt.draw(result);

    // Divider
    sf::RectangleShape div({ 400.f, 2.f });
    div.setOrigin(200.f, 1.f); div.setPosition(WIN_W / 2.f, WIN_H / 2.f + 12.f);
    div.setFillColor(sf::Color(gold.r, gold.g, gold.b, 130));
    rt.draw(div);

    std::ostringstream oss;
    oss << "Счёт: " << playerWins_ << " - " << dealerWins_;
    sf::Text score = makeText(oss.str(), 28, gold);
    { sf::FloatRect b = score.getLocalBounds(); score.setOrigin(b.width / 2.f, 0.f); }
    score.setPosition(WIN_W / 2.f, WIN_H / 2.f + 22.f);
    rt.draw(score);

    sf::Text cont = makeText("Нажми, чтобы продолжить...", 20, sf::Color(168, 158, 138));
    { sf::FloatRect b = cont.getLocalBounds(); cont.setOrigin(b.width / 2.f, 0.f); }
    cont.setPosition(WIN_W / 2.f, WIN_H / 2.f + 68.f);
    rt.draw(cont);
}

// ════════════════════════════════════════════════════ drawGameOver ══════════
void Game::drawGameOver(sf::RenderTarget& rt) {
    sf::RectangleShape bg({ (float)WIN_W, (float)WIN_H });
    bg.setFillColor(sf::Color(5, 4, 9));
    rt.draw(bg);

    bool playerWon = (playerWins_ >= ROUNDS_TO_WIN);
    sf::Color mc = playerWon ? gold : sf::Color(200, 50, 50);

    // Glow
    for (int g = 5; g >= 1; --g) {
        sf::CircleShape glow(g * 70.f);
        glow.setOrigin(g * 70.f, g * 70.f);
        glow.setPosition(WIN_W / 2.f, WIN_H / 2.f - 80.f);
        glow.setFillColor(sf::Color(mc.r, mc.g, mc.b, (sf::Uint8)(4 * g)));
        rt.draw(glow);
    }

    sf::Text rsh = makeText(playerWon ? "ТЫ ВЫИГРАЛ!" : "ИГРА ОКОНЧЕНА", 72,
        sf::Color(0, 0, 0, 165));
    { sf::FloatRect b = rsh.getLocalBounds(); rsh.setOrigin(b.width / 2.f, b.height / 2.f); }
    rsh.setPosition(WIN_W / 2.f + 4.f, WIN_H / 2.f - 80.f + 4.f);
    rt.draw(rsh);
    sf::Text result = makeText(playerWon ? "ТЫ ВЫИГРАЛ!" : "ИГРА ОКОНЧЕНА", 72, mc);
    { sf::FloatRect b = result.getLocalBounds(); result.setOrigin(b.width / 2.f, b.height / 2.f); }
    result.setPosition(WIN_W / 2.f, WIN_H / 2.f - 80.f);
    rt.draw(result);

    // Divider
    sf::RectangleShape div({ 480.f, 2.f });
    div.setOrigin(240.f, 1.f); div.setPosition(WIN_W / 2.f, WIN_H / 2.f + 2.f);
    div.setFillColor(sf::Color(mc.r, mc.g, mc.b, 115));
    rt.draw(div);

    std::string subStr = playerWon ? "Поздравляю, выживший." : "Дилер снова победил.";
    sf::Text sub2 = makeText(subStr, 28, textCream);
    { sf::FloatRect b = sub2.getLocalBounds(); sub2.setOrigin(b.width / 2.f, 0.f); }
    sub2.setPosition(WIN_W / 2.f, WIN_H / 2.f + 12.f);
    rt.draw(sub2);

    sf::Text back = makeText("Нажми, чтобы вернуться в меню", 20, sf::Color(175, 175, 175));
    { sf::FloatRect b = back.getLocalBounds(); back.setOrigin(b.width / 2.f, 0.f); }
    back.setPosition(WIN_W / 2.f, WIN_H / 2.f + 72.f);
    rt.draw(back);
}

// ════════════════════════════════════════════════════ drawPeekResult ══════════
void Game::drawPeekResult(sf::RenderTarget& rt) {
    if (!peekResult_) return;
    bool isLive = (*peekResult_ == ShellType::LIVE);
    sf::Color col = isLive ? liveRed : blankBlue;

    // Pill background
    sf::Text tmp = makeText(isLive ? "ТЕКУЩИЙ ПАТРОН: БОЕВОЙ!" : "ТЕКУЩИЙ ПАТРОН: холостой",
        22, col);
    float tw2 = tmp.getLocalBounds().width + 28.f;
    sf::RectangleShape pill({ tw2, 32.f });
    pill.setOrigin(tw2 / 2.f, 16.f);
    pill.setPosition(WIN_W / 2.f, WIN_H / 2.f + 55.f + 16.f);
    pill.setFillColor(sf::Color(col.r / 5, col.g / 5, col.b / 5, 215));
    pill.setOutlineColor(col);
    pill.setOutlineThickness(2.f);
    rt.draw(pill);

    sf::Text t = makeText(isLive ? "ТЕКУЩИЙ ПАТРОН: БОЕВОЙ!" : "ТЕКУЩИЙ ПАТРОН: холостой",
        22, col);
    { sf::FloatRect b = t.getLocalBounds(); t.setOrigin(b.width / 2.f, 0.f); }
    t.setPosition(WIN_W / 2.f, WIN_H / 2.f + 55.f);
    rt.draw(t);
}
void Game::addLog(const std::string& msg, sf::Color col) {
    log_.add(msg, col);
}

void Game::triggerCameraShake(float amt) {
    cameraShake_ = amt;
}

void Game::triggerFlash(float alpha) {
    flashAlpha_ = alpha;
}

sf::Text Game::makeText(const std::string& s, unsigned size, sf::Color col) const {
    sf::Text t;
    if (fontLoaded_) t.setFont(font_);
    t.setString(utf8(s));
    t.setCharacterSize(size);
    t.setFillColor(col);
    return t;
}


// ─────────────────────────────────────────────────────────────────── main ────

int main() {
    Game game;
    game.run();
    return 0;
}