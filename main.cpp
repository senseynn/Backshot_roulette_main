#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Game.h"

sf::Text createText(const std::string& str, const sf::Font& font, unsigned int size, sf::Color color, float x, float y) {
  sf::Text text;
  text.setFont(font);
  text.setString(str);
  text.setCharacterSize(size);
  text.setFillColor(color);
  text.setPosition(x, y);
  return text;
}

class Button {
public:
  Button(sf::FloatRect rect, const std::string& label, const sf::Font& font, unsigned int fontSize)
    : rect(rect), label(label) {
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(fontSize);
    text.setFillColor(sf::Color::White);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
    shape.setSize(sf::Vector2f(rect.width, rect.height));
    shape.setPosition(rect.left, rect.top);
    shape.setFillColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);
  }

  void draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
  }

  bool contains(sf::Vector2f point) const {
    return rect.contains(point);
  }

  void setPosition(float x, float y) {
    rect.left = x;
    rect.top = y;
    shape.setPosition(x, y);
    text.setPosition(x + rect.width / 2.0f, y + rect.height / 2.0f);
  }

  void setLabel(const std::string& newLabel) {
    label = newLabel;
    text.setString(label);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
  }

private:
  sf::FloatRect rect;
  std::string label;
  sf::RectangleShape shape;
  sf::Text text;
};

int main() {
  sf::RenderWindow window(sf::VideoMode(1024, 768), "Buckshot Roulette", sf::Style::Close);
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("BenzinBold.ttf")) {
    std::cerr << "Failed to load font arial.ttf" << std::endl;
    return -1;
  }

  Game game;

  Button btnShootSelf(sf::FloatRect(100, 600, 200, 50), "Shoot yourself", font, 24);
  Button btnShootOpponent(sf::FloatRect(350, 600, 200, 50), "Shoot the creature", font, 24);
  std::vector<std::unique_ptr<Button>> itemButtons;

  sf::Text playerHealthText, aiHealthText, stageText, turnText, shellInfoText, messageText;
  playerHealthText = createText("", font, 28, sf::Color::White, 50, 50);
  aiHealthText = createText("", font, 28, sf::Color::White, 50, 100);
  stageText = createText("", font, 28, sf::Color::White, 50, 150);
  turnText = createText("", font, 28, sf::Color::White, 50, 200);
  shellInfoText = createText("", font, 28, sf::Color::White, 50, 250);
  messageText = createText("", font, 24, sf::Color::Yellow, 50, 300);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (game.isPlayerTurn()) {
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

    playerHealthText.setString("Your health: " + std::to_string(game.getPlayerHealth()));
    aiHealthText.setString("Creature health: " + std::to_string(game.getAIHealth()));
    stageText.setString("Stage: " + std::to_string(game.getStage()));
    turnText.setString(game.isPlayerTurn() ? "Your turn" : "Creature's turn");
    shellInfoText.setString("Current shell: " + game.getCurrentShellInfo());
    messageText.setString(game.getLastMessage());

    const auto& items = game.getPlayerItems();
    itemButtons.clear();
    float x = 600, y = 600;
    for (size_t i = 0; i < items.size(); ++i) {
      std::string label;
      switch (items[i].type) {
      case ItemType::Soda: label = "Soda"; break;
      case ItemType::Saw: label = "Hand Saw"; break;
      case ItemType::Handcuffs: label = "Handcuffs"; break;
      case ItemType::Magnifier: label = "Magnifier"; break;
      case ItemType::Apple: label = "Apple"; break;
      default: label = "?";
      }
      auto btn = std::make_unique<Button>(sf::FloatRect(x, y, 150, 40), label, font, 20);
      btn->setPosition(x, y);
      itemButtons.push_back(std::move(btn));
      y += 50;
      if (y > 700) { y = 600; x += 160; }
    }

    window.clear(sf::Color(30, 30, 30));

    window.draw(playerHealthText);
    window.draw(aiHealthText);
    window.draw(stageText);
    window.draw(turnText);
    window.draw(shellInfoText);
    window.draw(messageText);

    btnShootSelf.draw(window);
    btnShootOpponent.draw(window);
    for (auto& btn : itemButtons) {
      btn->draw(window);
    }

    window.display();
  }

  return 0;
}