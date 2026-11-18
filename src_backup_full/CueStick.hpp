#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.hpp"

class CueStick {
public:
    CueStick();

    // Update aim direction toward mouse, only when cue ball is at rest
    void update(const sf::RenderWindow& window, const Ball& cueBall);

    // Draw cue when active
    void draw(sf::RenderWindow& window) const;

    // Returns shot velocity vector; call when player clicks
    sf::Vector2f shoot(float power);

    // Show / hide the cue
    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }

    // Current pull-back distance (0–MAX_POWER)
    float getPower() const { return m_power; }
    void setPower(float p) { m_power = p; }

    static constexpr float MAX_POWER = 900.f;
    static constexpr float CUE_LENGTH = 160.f;
    static constexpr float CUE_GAP = 6.f;   // gap between tip and ball

private:
    sf::RectangleShape m_shaft;
    sf::Vector2f m_direction;   // unit vector from ball toward mouse
    sf::Vector2f m_cueBallPos;
    float m_power;
    bool m_visible;
};
