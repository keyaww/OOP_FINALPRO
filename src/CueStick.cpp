#include "CueStick.hpp"
#include <cmath>

CueStick::CueStick()
    : m_direction(1.f, 0.f), m_cueBallPos(0.f, 0.f), m_power(0.f), m_visible(false)
{
    m_shaft.setSize(sf::Vector2f(CUE_LENGTH, 7.f));
    m_shaft.setOrigin(sf::Vector2f(0.f, 3.5f));
    m_shaft.setFillColor(sf::Color(160, 100, 40));
    m_shaft.setOutlineColor(sf::Color(80, 50, 10));
    m_shaft.setOutlineThickness(1.f);
}

void CueStick::update(const sf::RenderWindow& window, const Ball& cueBall, bool isDragging, float dragDistance) {
    if (!m_visible) return;

    m_cueBallPos = cueBall.getPosition();
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouse = window.mapPixelToCoords(mousePixel);

    if (!isDragging) {
        // Aiming mode: aim direction points from cue ball towards mouse
        sf::Vector2f diff = mouse - m_cueBallPos;
        float len = std::hypot(diff.x, diff.y);
        if (len > 0.f)
            m_direction = diff / len;
        else
            m_direction = {1.f, 0.f};

        m_power = 0.f;
    } else {
        // Dragging mode: power is the dragged distance (clamped to MAX_DRAG)
        m_power = std::clamp(dragDistance, 0.f, MAX_DRAG);
    }

    // Cue tip position: starts behind the ball opposite to aim direction
    // As player pulls back (m_power increases), the cue moves further back
    float offset = cueBall.getRadius() + CUE_GAP + m_power;
    sf::Vector2f tipPos = m_cueBallPos - m_direction * offset;

    // Rotate: atan2 of direction
    float angle = std::atan2(m_direction.y, m_direction.x) * 180.f / 3.14159265f;
    m_shaft.setPosition(tipPos);
    m_shaft.setRotation(sf::degrees(angle + 180.f)); // tip points toward ball
}

void CueStick::draw(sf::RenderWindow& window) const {
    if (!m_visible) return;
    window.draw(m_shaft);
}

sf::Vector2f CueStick::shoot(float power) {
    // Shot impulse proportional to pull-back distance
    float speed = (power / MAX_DRAG) * 900.f;
    return m_direction * speed;
}
