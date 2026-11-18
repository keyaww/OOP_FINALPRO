#include "Ball.hpp"
#include <cmath>

sf::Font Ball::s_font;
bool Ball::s_fontLoaded = false;

Ball::Ball(int id, BallType type, sf::Vector2f pos, sf::Color color)
    : m_id(id), m_type(type), m_radius(RADIUS), m_pocketed(false)
{
    m_shape.setRadius(m_radius);
    m_shape.setOrigin(sf::Vector2f(m_radius, m_radius));
    m_shape.setPosition(pos);
    m_shape.setFillColor(color);
    m_shape.setOutlineColor(sf::Color(40, 20, 10));
    m_shape.setOutlineThickness(1.5f);

    // For stripe balls: draw a white band in the middle
    if (type == BallType::Stripe) {
        m_stripe.setRadius(m_radius * 0.6f);
        m_stripe.setOrigin(sf::Vector2f(m_radius * 0.6f, m_radius * 0.6f));
        m_stripe.setPosition(pos);
        m_stripe.setFillColor(sf::Color(240, 240, 240));
    }

    m_velocity = {0.f, 0.f};
}

void Ball::setPosition(sf::Vector2f pos) {
    m_shape.setPosition(pos);
    if (m_type == BallType::Stripe)
        m_stripe.setPosition(pos);
}

void Ball::applyFriction(float /*dt*/) {
    m_velocity *= FRICTION;
    if (std::hypot(m_velocity.x, m_velocity.y) < MIN_SPEED)
        m_velocity = {0.f, 0.f};
}

void Ball::update(float dt) {
    if (m_pocketed) return;
    setPosition(m_shape.getPosition() + m_velocity * dt);
    applyFriction(dt);
}

bool Ball::isMoving() const {
    return std::hypot(m_velocity.x, m_velocity.y) >= MIN_SPEED;
}

void Ball::draw(sf::RenderWindow& window) const {
    if (m_pocketed) return;
    window.draw(m_shape);
    if (m_type == BallType::Stripe)
        window.draw(m_stripe);
}
