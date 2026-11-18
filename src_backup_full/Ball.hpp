#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BallType { CueBall, Solid, Stripe };

class Ball {
public:
    Ball(int id, BallType type, sf::Vector2f pos, sf::Color color);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;
    void applyFriction(float dt);

    // Getters
    int getId() const { return m_id; }
    BallType getType() const { return m_type; }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }
    sf::Vector2f getVelocity() const { return m_velocity; }
    float getRadius() const { return m_radius; }
    bool isPocketed() const { return m_pocketed; }
    bool isMoving() const;

    // Setters
    void setPosition(sf::Vector2f pos);
    void setVelocity(sf::Vector2f vel) { m_velocity = vel; }
    void setPocketed(bool p) { m_pocketed = p; }

    static constexpr float RADIUS = 14.f;
    static constexpr float FRICTION = 0.985f;
    static constexpr float MIN_SPEED = 2.f;

private:
    int m_id;
    BallType m_type;
    sf::CircleShape m_shape;
    sf::CircleShape m_stripe;   // inner stripe band for stripe balls
    float m_radius;
    sf::Vector2f m_velocity;
    bool m_pocketed;

    static sf::Font s_font;
    static bool s_fontLoaded;
};
