#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Game.h"

constexpr float WINDOW_W = 1024.f;
constexpr float WINDOW_H = 768.f;
constexpr float CENTER_X = WINDOW_W / 2.f;

sf::Text createText(const std::string& str, const sf::Font& font, unsigned int size,
    sf::Color color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

void centerTextX(sf::Text& text, float centerX, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(centerX, y);
}

void centerTextInRect(sf::Text& text, const sf::FloatRect& rect) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.f,
                   bounds.top + bounds.height / 2.f);
    text.setPosition(rect.left + rect.width / 2.f,
                     rect.top + rect.height / 2.f);
}

class Button {
public:
    Button(sf::FloatRect rect, const std::string& label, const sf::Font& font, unsigned int fontSize)
        : rect(rect), label(label) {
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(fontSize);
        text.setFillColor(sf::Color::White);

        shape.setSize(sf::Vector2f(rect.width, rect.height));
        shape.setPosition(rect.left, rect.top);
        shape.setFillColor(sf::Color(70, 70, 70));
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::White);

        centerTextInRect(text, rect);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    bool contains(sf::Vector2f point) const {
        return rect.contains(point);
    }

    void setFillColor(const sf::Color& color) {
        shape.setFillColor(color);
    }

    void setOutlineColor(const sf::Color& color) {
        shape.setOutlineColor(color);
    }

private:
    sf::FloatRect rect;
    std::string label;
    sf::RectangleShape shape;
    sf::Text text;
};

int main() {
    sf::RenderWindow window(sf::VideoMode((unsigned int)WINDOW_W, (unsigned int)WINDOW_H),
        "Buckshot Roulette", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("BenzinBold.ttf")) {
        std::cerr << "Failed to load font BenzinBold.ttf" << std::endl;
        return -1;
    }

    Game game;

    // ФОН
    sf::RectangleShape background(sf::Vector2f(WINDOW_W, WINDOW_H));
    background.setFillColor(sf::Color(18, 18, 28));

    // СТОЛ
    sf::RectangleShape tableTop(sf::Vector2f(820.f, 24.f));
    tableTop.setFillColor(sf::Color(60, 38, 24));
    tableTop.setPosition(102.f, 420.f);

    sf::RectangleShape tableFront(sf::Vector2f(820.f, 125.f));
    tableFront.setFillColor(sf::Color(38, 24, 16));
    tableFront.setPosition(102.f, 444.f);

    // ДИЛЕР
    sf::RectangleShape dealerBody(sf::Vector2f(72.f, 100.f));
    dealerBody.setFillColor(sf::Color(28, 28, 36));
    dealerBody.setPosition(105.f, 300.f);

    sf::CircleShape dealerHead(30.f);
    dealerHead.setFillColor(sf::Color(225, 225, 230));
    dealerHead.setPosition(108.f, 258.f);

    sf::RectangleShape dealerHat(sf::Vector2f(54.f, 12.f));
    dealerHat.setFillColor(sf::Color(25, 25, 35));
    dealerHat.setPosition(96.f, 254.f);

    sf::RectangleShape dealerHatTop(sf::Vector2f(36.f, 20.f));
    dealerHatTop.setFillColor(sf::Color(25, 25, 35));
    dealerHatTop.setPosition(105.f, 234.f);

    sf::CircleShape dealerEyeL(8.f);
    dealerEyeL.setFillColor(sf::Color(255, 210, 0));
    dealerEyeL.setPosition(117.f, 278.f);

    sf::CircleShape dealerEyeR(8.f);
    dealerEyeR.setFillColor(sf::Color(255, 210, 0));
    dealerEyeR.setPosition(144.f, 278.f);

    sf::CircleShape dealerPupilL(3.f);
    dealerPupilL.setFillColor(sf::Color::Black);
    dealerPupilL.setPosition(121.f, 282.f);

    sf::CircleShape dealerPupilR(3.f);
    dealerPupilR.setFillColor(sf::Color::Black);
    dealerPupilR.setPosition(148.f, 282.f);

    // ИГРОК
    sf::RectangleShape playerBody(sf::Vector2f(72.f, 100.f));
    playerBody.setFillColor(sf::Color(42, 42, 52));
    playerBody.setPosition(848.f, 300.f);

    sf::CircleShape playerHead(30.f);
    playerHead.setFillColor(sf::Color(255, 220, 185));
    playerHead.setPosition(835.f, 258.f);

    sf::CircleShape playerEyeL(8.f);
    playerEyeL.setFillColor(sf::Color::White);
    playerEyeL.setPosition(846.f, 278.f);

    sf::CircleShape playerEyeR(8.f);
    playerEyeR.setFillColor(sf::Color::White);
    playerEyeR.setPosition(873.f, 278.f);

    sf::CircleShape playerPupilL(3.f);
    playerPupilL.setFillColor(sf::Color::Black);
    playerPupilL.setPosition(850.f, 282.f);

    sf::CircleShape playerPupilR(3.f);
    playerPupilR.setFillColor(sf::Color::Black);
    playerPupilR.setPosition(877.f, 282.f);

    // ДРОБОВИК
    sf::RectangleShape shotgunStock(sf::Vector2f(82.f, 14.f));
    shotgunStock.setFillColor(sf::Color(92, 58, 26));
    shotgunStock.setPosition(350.f, 406.f);

    sf::RectangleShape shotgunBarrel(sf::Vector2f(150.f, 10.f));
    shotgunBarrel.setFillColor(sf::Color(120, 120, 128));
    shotgunBarrel.setPosition(432.f, 408.f);

    // HP БАРЫ
    sf::RectangleShape dealerHPBg(sf::Vector2f(200.f, 18.f));
    dealerHPBg.setFillColor(sf::Color(35, 35, 35));
    dealerHPBg.setPosition(30.f, 548.f);

    sf::RectangleShape dealerHP(sf::Vector2f(200.f, 18.f));
    dealerHP.setFillColor(sf::Color(190, 45, 45));
    dealerHP.setPosition(30.f, 548.f);

    sf::RectangleShape dealerFrame(sf::Vector2f(200.f, 18.f));
    dealerFrame.setFillColor(sf::Color::Transparent);
    dealerFrame.setOutlineThickness(1.f);
    dealerFrame.setOutlineColor(sf::Color::White);
    dealerFrame.setPosition(30.f, 548.f);

    sf::RectangleShape playerHPBg(sf::Vector2f(200.f, 18.f));
    playerHPBg.setFillColor(sf::Color(35, 35, 35));
    playerHPBg.setPosition(794.f, 548.f);

    sf::RectangleShape playerHP(sf::Vector2f(200.f, 18.f));
    playerHP.setFillColor(sf::Color(50, 185, 50));
    playerHP.setPosition(794.f, 548.f);

    sf::RectangleShape playerFrame(sf::Vector2f(200.f, 18.f));
    playerFrame.setFillColor(sf::Color::Transparent);
    playerFrame.setOutlineThickness(1.f);
    playerFrame.setOutlineColor(sf::Color::White);
    playerFrame.setPosition(794.f, 548.f);

    // OVERLAY
    sf::RectangleShape gameOverlay(sf::Vector2f(WINDOW_W, WINDOW_H));
    gameOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    // КНОПКИ
    Button btnShootSelf(sf::FloatRect(100.f, 620.f, 180.f, 45.f), "Shoot yourself", font, 20);
    Button btnShootOpponent(sf::FloatRect(744.f, 620.f, 180.f, 45.f), "Shoot dealer", font, 20);
    std::vector<std::unique_ptr<Button>> itemButtons;

    // ТЕКСТЫ
    sf::Text stageText = createText("STAGE 1/3", font, 26, sf::Color::White, 0.f, 0.f);
    sf::Text turnText = createText("YOUR TURN", font, 26, sf::Color::White, 0.f, 0.f);
    sf::Text shellText = createText("CURRENT SHELL: ???", font, 22, sf::Color(255, 210, 120), 0.f, 0.f);
    sf::Text messageText = createText("", font, 20, sf::Color(200, 200, 200), 220.f, 508.f);

    sf::Text dealerHPLabel = createText("DEALER", font, 16, sf::Color(160, 160, 160), 30.f, 528.f);
    sf::Text playerHPLabel = createText("YOU", font, 16, sf::Color(160, 160, 160), 794.f, 528.f);
    sf::Text itemsLabel = createText("YOUR ITEMS:", font, 18, sf::Color(150, 150, 210), 30.f, 675.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (game.isPlayerTurn() && !game.isGameOver()) {
                    if (btnShootSelf.contains(mousePos)) {
                        game.playerShootSelf();
                    }
                    else if (btnShootOpponent.contains(mousePos)) {
                        game.playerShootOpponent();
                    }
                    else {
                        for (size_t i = 0; i < itemButtons.size(); ++i) {
                            if (itemButtons[i]->contains(mousePos)) {
                                game.playerUseItem(static_cast<int>(i));
                                break;
                            }
                        }
                    }
                }
            }
        }

        int maxHP = (game.getStage() == 1) ? 2 : (game.getStage() == 2) ? 4 : 5;

        int aiHP = game.getAIHealth();
        if (aiHP < 0) aiHP = 0;
        if (aiHP > maxHP) aiHP = maxHP;

        int plHP = game.getPlayerHealth();
        if (plHP < 0) plHP = 0;
        if (plHP > maxHP) plHP = maxHP;

        dealerHP.setSize(sf::Vector2f(200.f * aiHP / maxHP, 18.f));
        playerHP.setSize(sf::Vector2f(200.f * plHP / maxHP, 18.f));

        stageText.setString("STAGE " + std::to_string(game.getStage()) + "/3");
        turnText.setString(game.isPlayerTurn() ? "YOUR TURN" : "DEALER'S TURN");
        shellText.setString("CURRENT SHELL: " + game.getCurrentShellInfo());
        messageText.setString(game.getLastMessage());

        centerTextX(stageText, CENTER_X, 28.f);
        centerTextX(turnText, CENTER_X, 62.f);
        centerTextX(shellText, CENTER_X, 95.f);

        const auto& items = game.getPlayerItems();
        itemButtons.clear();

        float x = 200.f;
        float y = 690.f;

        for (size_t i = 0; i < items.size(); ++i) {
            std::string label;
            sf::Color btnColor;

            switch (items[i].type) {
            case ItemType::Soda:
                label = "SODA";
                btnColor = sf::Color(60, 120, 180);
                break;
            case ItemType::Saw:
                label = "SAW";
                btnColor = sf::Color(150, 150, 150);
                break;
            case ItemType::Handcuffs:
                label = "CUFFS";
                btnColor = sf::Color(120, 120, 120);
                break;
            case ItemType::Magnifier:
                label = "LENS";
                btnColor = sf::Color(185, 185, 85);
                break;
            case ItemType::Apple:
                label = "APPLE";
                btnColor = sf::Color(180, 65, 65);
                break;
            default:
                label = "?";
                btnColor = sf::Color(90, 90, 90);
                break;
            }

            auto btn = std::make_unique<Button>(sf::FloatRect(x, y, 100.f, 40.f), label, font, 16);
            btn->setFillColor(btnColor);
            btn->setOutlineColor(sf::Color(220, 220, 220));
            itemButtons.push_back(std::move(btn));

            x += 115.f;
        }

        window.clear();

        window.draw(background);

        window.draw(tableTop);
        window.draw(tableFront);

        window.draw(dealerBody);
        window.draw(dealerHead);
        window.draw(dealerHat);
        window.draw(dealerHatTop);
        window.draw(dealerEyeL);
        window.draw(dealerEyeR);
        window.draw(dealerPupilL);
        window.draw(dealerPupilR);

        window.draw(playerBody);
        window.draw(playerHead);
        window.draw(playerEyeL);
        window.draw(playerEyeR);
        window.draw(playerPupilL);
        window.draw(playerPupilR);

        window.draw(shotgunStock);
        window.draw(shotgunBarrel);

        window.draw(dealerHPBg);
        window.draw(dealerHP);
        window.draw(dealerFrame);

        window.draw(playerHPBg);
        window.draw(playerHP);
        window.draw(playerFrame);

        window.draw(stageText);
        window.draw(turnText);
        window.draw(shellText);
        window.draw(messageText);
        window.draw(dealerHPLabel);
        window.draw(playerHPLabel);
        window.draw(itemsLabel);

        btnShootSelf.draw(window);
        btnShootOpponent.draw(window);

        for (auto& btn : itemButtons) {
            btn->draw(window);
        }

        if (game.isGameOver()) {
            window.draw(gameOverlay);

            sf::Text gameOverText = createText("GAME OVER", font, 42, sf::Color::Red, 0.f, 0.f);
            centerTextX(gameOverText, CENTER_X, WINDOW_H / 2.f);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
