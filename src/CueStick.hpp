#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.hpp"

class CueStick {
public:
    CueStick();

    // Update aim and position. When dragging (charging), offset is based on dragDistance.
    void update(const sf::RenderWindow& window, const Ball& cueBall, bool isDragging, float dragDistance);

    // Draw cue when active
    void draw(sf::RenderWindow& window) const;

    // Returns shot velocity vector; call when player releases drag
    sf::Vector2f shoot(float power);

    // Show / hide the cue
    void setVisible(bool v) { m_visible = v; }
    bool isVisible() const { return m_visible; }

    // Current pull-back distance
    float getPower() const { return m_power; }
    void setPower(float p) { m_power = p; }

    static constexpr float MAX_DRAG = 200.f;  // Maximum pull back pixels
    static constexpr float CUE_LENGTH = 160.f;
    static constexpr float CUE_GAP = 6.f;      // Gap between tip and ball

private:
    sf::RectangleShape m_shaft;
    sf::Vector2f m_direction;   // unit vector from cue ball toward target
    sf::Vector2f m_cueBallPos;
    float m_power;
    bool m_visible;
};
