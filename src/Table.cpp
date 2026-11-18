#include "Table.hpp"
#include <cmath>

static constexpr float WIN_W = 1024.f;
static constexpr float WIN_H = 600.f;
static constexpr float RAIL_W = 36.f;

Table::Table() {
    // --- Rail (wooden outer frame) ---
    m_rail.setSize(sf::Vector2f(WIN_W - 40.f, WIN_H - 40.f));
    m_rail.setPosition(sf::Vector2f(20.f, 20.f));
    m_rail.setFillColor(sf::Color(90, 55, 20));
    m_rail.setOutlineColor(sf::Color(60, 35, 10));
    m_rail.setOutlineThickness(3.f);

    // --- Felt (play area) ---
    float feltX = 20.f + RAIL_W;
    float feltY = 20.f + RAIL_W;
    float feltW = (WIN_W - 40.f) - 2.f * RAIL_W;
    float feltH = (WIN_H - 40.f) - 2.f * RAIL_W;
    m_felt.setSize(sf::Vector2f(feltW, feltH));
    m_felt.setPosition(sf::Vector2f(feltX, feltY));
    m_felt.setFillColor(sf::Color(34, 120, 50));
    m_playArea = sf::FloatRect(sf::Vector2f(feltX, feltY), sf::Vector2f(feltW, feltH));

    // --- Pockets (6 positions: 4 corners + 2 side midpoints) ---
    float pR = 18.f;
    std::array<sf::Vector2f, 6> centers = {{
        {feltX,            feltY},                           // top-left
        {feltX + feltW/2.f, feltY - 4.f},                   // top-mid
        {feltX + feltW,    feltY},                           // top-right
        {feltX,            feltY + feltH},                   // bottom-left
        {feltX + feltW/2.f, feltY + feltH + 4.f},           // bottom-mid
        {feltX + feltW,    feltY + feltH},                   // bottom-right
    }};

    for (int i = 0; i < 6; ++i) {
        m_pockets[i] = {centers[i], pR};
        m_pocketShapes[i].setRadius(pR);
        m_pocketShapes[i].setOrigin(sf::Vector2f(pR, pR));
        m_pocketShapes[i].setPosition(centers[i]);
        m_pocketShapes[i].setFillColor(sf::Color(10, 10, 10));
    }
}

bool Table::isInPocket(sf::Vector2f pos, Pocket& hitPocket) const {
    for (auto& p : m_pockets) {
        float dx = pos.x - p.center.x;
        float dy = pos.y - p.center.y;
        if (std::hypot(dx, dy) <= p.radius + 2.f) {
            hitPocket = p;
            return true;
        }
    }
    return false;
}

void Table::draw(sf::RenderWindow& window) const {
    window.draw(m_rail);
    window.draw(m_felt);
    // Draw head string (break line) dashed approximation
    float feltX = m_playArea.position.x;
    float feltY = m_playArea.position.y;
    float feltH = m_playArea.size.y;
    float headX = feltX + m_playArea.size.x * 0.25f;
    sf::RectangleShape line(sf::Vector2f(2.f, feltH));
    line.setPosition(sf::Vector2f(headX, feltY));
    line.setFillColor(sf::Color(255, 255, 255, 60));
    window.draw(line);
    // Pockets on top
    for (auto& ps : m_pocketShapes)
        window.draw(ps);
}
