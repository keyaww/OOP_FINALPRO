#pragma once
#include <SFML/Graphics.hpp>
#include <array>

struct Pocket {
    sf::Vector2f center;
    float radius;
};

class Table {
public:
    Table();
    void draw(sf::RenderWindow& window) const;

    // Returns the felt (play area) bounds
    sf::FloatRect getPlayArea() const { return m_playArea; }

    // All 6 pockets
    const std::array<Pocket, 6>& getPockets() const { return m_pockets; }

    // Check if a point is within the pocket radius
    bool isInPocket(sf::Vector2f pos, Pocket& hitPocket) const;

    static constexpr float RAIL_W = 36.f;

private:
    sf::RectangleShape m_rail;      // outer wood frame
    sf::RectangleShape m_felt;      // green play surface
    std::array<sf::CircleShape, 6> m_pocketShapes;
    std::array<Pocket, 6> m_pockets;
    sf::FloatRect m_playArea;
};
