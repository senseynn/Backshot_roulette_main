#include "UI.h"
#include "Utils.h"
#include <algorithm>

namespace {
    sf::Color withAlpha(sf::Color c, sf::Uint8 a) {
        c.a = a;
        return c;
    }

    sf::Color mix(sf::Color a, sf::Color b, float t) {
        t = std::clamp(t, 0.f, 1.f);
        auto lerp = [t](sf::Uint8 x, sf::Uint8 y) -> sf::Uint8 {
            return static_cast<sf::Uint8>(x + (y - x) * t);
            };
        return sf::Color(lerp(a.r, b.r), lerp(a.g, b.g), lerp(a.b, b.b), lerp(a.a, b.a));
    }
}

void Button::setup(sf::Vector2f pos, sf::Vector2f size, const std::string& text,
    const sf::Font& font, unsigned charSize,
    sf::Color nc, sf::Color hc, sf::Color pc) {
    normalColor = nc;
    hoverColor = hc;
    pressColor = pc;

    shape.setSize(size);
    shape.setPosition(pos);
    shape.setFillColor(nc);
    shape.setOutlineColor(sf::Color(188, 156, 102));
    shape.setOutlineThickness(2.f);

    label.setFont(font);
    label.setString(utf8(text));
    label.setCharacterSize(charSize);
    label.setFillColor(sf::Color(240, 230, 210));

    while (label.getLocalBounds().width > size.x - 18.f && label.getCharacterSize() > 9)
        label.setCharacterSize(label.getCharacterSize() - 1);

    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    label.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f - 1.f);
}

bool Button::contains(sf::Vector2f pt) const {
    return visible && enabled && shape.getGlobalBounds().contains(pt);
}

void Button::update(sf::Vector2f mousePos, bool mouseDown) {
    if (!visible || !enabled) {
        shape.setFillColor(sf::Color(38, 38, 46, 180));
        shape.setOutlineColor(sf::Color(92, 88, 82, 150));
        label.setFillColor(sf::Color(150, 146, 138));
        return;
    }

    hovered = contains(mousePos);
    pressed = hovered && mouseDown;

    shape.setFillColor(pressed ? pressColor : (hovered ? hoverColor : normalColor));
    shape.setOutlineColor(pressed ? sf::Color(226, 182, 104) : (hovered ? sf::Color(214, 174, 102) : sf::Color(188, 156, 102)));
    label.setFillColor(pressed ? sf::Color(255, 235, 204) : (hovered ? sf::Color(255, 242, 220) : sf::Color(240, 230, 210)));
}

void Button::draw(sf::RenderTarget& rt) const {
    if (!visible) return;

    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f sz = shape.getSize();

    sf::RectangleShape shadow(sz);
    shadow.setPosition(pos.x, pos.y + 4.f);
    shadow.setFillColor(sf::Color(0, 0, 0, pressed ? 70 : 95));
    rt.draw(shadow);

    rt.draw(shape);

    sf::RectangleShape topSheen({ sz.x - 6.f, std::max(8.f, sz.y * 0.34f) });
    topSheen.setPosition(pos.x + 3.f, pos.y + 3.f);
    topSheen.setFillColor(sf::Color(255, 236, 204, hovered ? 44 : 26));
    rt.draw(topSheen);

    sf::RectangleShape inner({ sz.x - 4.f, sz.y - 4.f });
    inner.setPosition(pos.x + 2.f, pos.y + 2.f);
    inner.setFillColor(sf::Color::Transparent);
    inner.setOutlineColor(sf::Color(255, 228, 180, hovered ? 70 : 42));
    inner.setOutlineThickness(1.f);
    rt.draw(inner);

    sf::RectangleShape bottomEdge({ sz.x - 4.f, 3.f });
    bottomEdge.setPosition(pos.x + 2.f, pos.y + sz.y - 5.f);
    bottomEdge.setFillColor(sf::Color(0, 0, 0, pressed ? 115 : 78));
    rt.draw(bottomEdge);

    sf::Text labelShadow = label;
    labelShadow.move(0.f, 1.5f);
    labelShadow.setFillColor(sf::Color(0, 0, 0, 120));
    rt.draw(labelShadow);
    rt.draw(label);
}

void Button::setText(const std::string& t, const sf::Font& font) {
    label.setFont(font);
    label.setString(utf8(t));
    sf::Vector2f sz = shape.getSize();
    while (label.getLocalBounds().width > sz.x - 18.f && label.getCharacterSize() > 9)
        label.setCharacterSize(label.getCharacterSize() - 1);
    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    sf::Vector2f pos = shape.getPosition();
    label.setPosition(pos.x + sz.x / 2.f, pos.y + sz.y / 2.f - 1.f);
}

void ItemSlot::setup(sf::Vector2f pos, sf::Vector2f size, const sf::Font& font) {
    bg.setSize(size);
    bg.setPosition(pos);
    bg.setFillColor(sf::Color(18, 18, 24, 190));
    bg.setOutlineColor(sf::Color(126, 102, 70));
    bg.setOutlineThickness(1.5f);

    label.setFont(font);
    label.setCharacterSize(12);
    label.setFillColor(sf::Color(226, 214, 194));
    label.setPosition(pos.x + 6.f, pos.y + 4.f);

    descLabel.setFont(font);
    descLabel.setCharacterSize(9);
    descLabel.setFillColor(sf::Color(170, 156, 134));
    descLabel.setPosition(pos.x + 6.f, pos.y + 24.f);
}

void ItemSlot::setItem(ItemType t) {
    item = t;
    label.setCharacterSize(showDescription ? 12 : 10);
    descLabel.setCharacterSize(showDescription ? 9 : 8);

    if (t == ItemType::NONE) {
        label.setString(utf8("--"));
        descLabel.setString(utf8(""));
        bg.setFillColor(sf::Color(18, 18, 24, 132));
        bg.setOutlineColor(sf::Color(82, 78, 74, 130));
    }
    else {
        label.setString(utf8(itemName(t)));
        descLabel.setString(showDescription ? utf8(itemDesc(t)) : utf8(""));
        const float maxTextWidth = bg.getSize().x - 10.f;
        while (label.getLocalBounds().width > maxTextWidth && label.getCharacterSize() > 7)
            label.setCharacterSize(label.getCharacterSize() - 1);
        while (descLabel.getLocalBounds().width > maxTextWidth && descLabel.getCharacterSize() > 7)
            descLabel.setCharacterSize(descLabel.getCharacterSize() - 1);
        bg.setFillColor(sf::Color(46, 34, 24, 230));
        bg.setOutlineColor(sf::Color(160, 122, 74));
    }
}

bool ItemSlot::contains(sf::Vector2f pt) const {
    return visible && item != ItemType::NONE && bg.getGlobalBounds().contains(pt);
}

void ItemSlot::update(sf::Vector2f mousePos) {
    hovered = contains(mousePos);
    if (!visible) {
        bg.setFillColor(sf::Color(18, 18, 24, 132));
        return;
    }

    if (selected) {
        bg.setFillColor(sf::Color(98, 72, 36, 242));
        bg.setOutlineColor(sf::Color(230, 180, 88));
    }
    else if (hovered && item != ItemType::NONE) {
        bg.setFillColor(sf::Color(70, 50, 30, 240));
        bg.setOutlineColor(sf::Color(206, 156, 84));
    }
    else if (item != ItemType::NONE) {
        bg.setFillColor(sf::Color(46, 34, 24, 230));
        bg.setOutlineColor(sf::Color(160, 122, 74));
    }
    else {
        bg.setFillColor(sf::Color(18, 18, 24, 132));
        bg.setOutlineColor(sf::Color(82, 78, 74, 130));
    }
}

void ItemSlot::draw(sf::RenderTarget& rt) const {
    if (!visible) return;

    sf::Vector2f pos = bg.getPosition();
    sf::Vector2f sz = bg.getSize();

    sf::RectangleShape shadow(sz);
    shadow.setPosition(pos.x, pos.y + 3.f);
    shadow.setFillColor(sf::Color(0, 0, 0, item == ItemType::NONE ? 25 : 65));
    rt.draw(shadow);

    rt.draw(bg);

    sf::RectangleShape topSheen({ sz.x - 4.f, std::max(6.f, sz.y * 0.30f) });
    topSheen.setPosition(pos.x + 2.f, pos.y + 2.f);
    topSheen.setFillColor(sf::Color(255, 232, 196, item == ItemType::NONE ? 8 : (selected ? 40 : 22)));
    rt.draw(topSheen);

    sf::RectangleShape inner({ sz.x - 4.f, sz.y - 4.f });
    inner.setPosition(pos.x + 2.f, pos.y + 2.f);
    inner.setFillColor(sf::Color::Transparent);
    inner.setOutlineColor(sf::Color(255, 228, 180, item == ItemType::NONE ? 10 : 26));
    inner.setOutlineThickness(1.f);
    rt.draw(inner);

    if (item != ItemType::NONE) {
        sf::Text shadowLabel = label;
        shadowLabel.move(0.f, 1.f);
        shadowLabel.setFillColor(sf::Color(0, 0, 0, 110));
        rt.draw(shadowLabel);
        rt.draw(label);
        if (showDescription) rt.draw(descLabel);
    }
}

void GameLog::setFont(const sf::Font& font, sf::Vector2f topLeft, float width) {
    font_ = &font;
    topLeft_ = topLeft;
    width_ = width;
}

void GameLog::add(const std::string& msg, sf::Color col) {
    const std::size_t maxBytes = 62;
    std::istringstream iss(msg);
    std::string word, line;
    while (iss >> word) {
        if (!line.empty() && line.size() + 1 + word.size() > maxBytes) {
            entries_.push_back({ line, col });
            line.clear();
        }
        if (!line.empty()) line += " ";
        line += word;
    }
    if (!line.empty()) entries_.push_back({ line, col });
    if (line.empty() && msg.empty()) entries_.push_back({ msg, col });
    if ((int)entries_.size() > MAX_LINES)
        entries_.erase(entries_.begin(), entries_.begin() + ((int)entries_.size() - MAX_LINES));
}

void GameLog::draw(sf::RenderTarget& rt) const {
    if (!font_) return;
    float y = topLeft_.y;

    sf::RectangleShape bg(sf::Vector2f(width_, MAX_LINES * 18.f + 10.f));
    bg.setPosition(topLeft_.x - 4.f, topLeft_.y - 4.f);
    bg.setFillColor(sf::Color(11, 11, 16, 215));
    bg.setOutlineColor(sf::Color(116, 90, 58, 180));
    bg.setOutlineThickness(1.5f);
    rt.draw(bg);

    sf::RectangleShape glow({ width_ - 6.f, 18.f });
    glow.setPosition(topLeft_.x - 1.f, topLeft_.y - 1.f);
    glow.setFillColor(sf::Color(255, 224, 176, 16));
    rt.draw(glow);

    sf::Text t;
    t.setFont(*font_);
    t.setCharacterSize(13);
    for (const auto& e : entries_) {
        sf::Text sh = t;
        sh.setString(utf8(e.text));
        sh.setFillColor(sf::Color(0, 0, 0, 110));
        sh.setPosition(topLeft_.x + 1.f, y + 1.f);
        rt.draw(sh);

        t.setString(utf8(e.text));
        t.setFillColor(e.color);
        t.setPosition(topLeft_.x, y);
        rt.draw(t);
        y += 18.f;
    }
}

void ShellIndicator::rebuild(const std::vector<Shell>& chambers, const sf::Font& font,
    sf::Vector2f start, float spacing) {
    circles.clear();
    labels.clear();
    for (int i = 0; i < (int)chambers.size(); ++i) {
        const Shell& s = chambers[i];
        sf::CircleShape c(11.f);
        c.setOrigin(11.f, 11.f);
        c.setPosition(start.x + i * spacing, start.y);
        if (s.known) {
            c.setFillColor(s.isLive() ? sf::Color(194, 62, 46) : sf::Color(66, 92, 210));
            c.setOutlineColor(sf::Color(228, 206, 168));
        }
        else {
            c.setFillColor(sf::Color(48, 48, 58));
            c.setOutlineColor(sf::Color(110, 110, 126));
        }
        c.setOutlineThickness(2.f);
        circles.push_back(c);

        if (s.known) {
            sf::Text t;
            t.setFont(font);
            t.setCharacterSize(10);
            t.setString(utf8(s.isLive() ? "Б" : "Х"));
            t.setFillColor(sf::Color::White);
            sf::FloatRect b = t.getLocalBounds();
            t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
            t.setPosition(start.x + i * spacing, start.y - 0.5f);
            labels.push_back(t);
        }
    }
}

void ShellIndicator::draw(sf::RenderTarget& rt) const {
    for (auto c : circles) {
        sf::CircleShape shadow = c;
        shadow.move(0.f, 2.f);
        shadow.setFillColor(sf::Color(0, 0, 0, 80));
        shadow.setOutlineThickness(0.f);
        rt.draw(shadow);
        rt.draw(c);
    }
    for (auto t : labels) rt.draw(t);
}