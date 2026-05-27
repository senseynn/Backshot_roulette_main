#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include "Item.h"
#include "Shell.h"

// ─── Button ──────────────────────────────────────────────────────────────────
struct Button {
    sf::RectangleShape shape;
    sf::Text           label;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressColor;
    bool hovered = false;
    bool pressed = false;
    bool visible = true;
    bool enabled = true;
    std::string id;

    Button() {}

    void setup(sf::Vector2f pos, sf::Vector2f size, const std::string& text,
               const sf::Font& font, unsigned charSize = 18,
               sf::Color nc = sf::Color(60, 60, 80),
               sf::Color hc = sf::Color(90, 90, 120),
               sf::Color pc = sf::Color(40, 40, 60));

    bool contains(sf::Vector2f pt) const;
    void update(sf::Vector2f mousePos, bool mouseDown);
    void draw(sf::RenderTarget& rt) const;
    void setText(const std::string& t, const sf::Font& font);
};

// ─── Item Slot (visual item card) ────────────────────────────────────────────
struct ItemSlot {
    sf::RectangleShape bg;
    sf::Text label;
    sf::Text descLabel;
    ItemType item = ItemType::NONE;
    bool selected = false;
    bool hovered = false;
    bool visible = true;
    bool showDescription = true;

    void setup(sf::Vector2f pos, sf::Vector2f size, const sf::Font& font);
    void setItem(ItemType t);
    bool contains(sf::Vector2f pt) const;
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderTarget& rt) const;
};

// ─── Log ─────────────────────────────────────────────────────────────────────
class GameLog {
public:
    static constexpr int MAX_LINES = 8;

    GameLog() {}

    void setFont(const sf::Font& font, sf::Vector2f topLeft, float width);
    void add(const std::string& msg, sf::Color col = sf::Color(210, 200, 180));
    void draw(sf::RenderTarget& rt) const;

private:
    struct Entry { std::string text; sf::Color color; };
    std::vector<Entry> entries_;
    const sf::Font* font_ = nullptr;
    sf::Vector2f topLeft_;
    float width_ = 300.f;
};

// ─── Shell display row ────────────────────────────────────────────────────────
struct ShellIndicator {
    std::vector<sf::CircleShape> circles;
    std::vector<sf::Text> labels;

    void rebuild(const std::vector<Shell>& chambers, const sf::Font& font,
                 sf::Vector2f start, float spacing = 28.f);
    void draw(sf::RenderTarget& rt) const;
};
