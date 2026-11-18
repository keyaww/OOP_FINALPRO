#include "Physics.hpp"
#include <cmath>
#include <algorithm>

void Physics::resolveWalls(Ball& ball, const sf::FloatRect& playArea) {
    sf::Vector2f pos = ball.getPosition();
    sf::Vector2f vel = ball.getVelocity();
    float r = ball.getRadius();

    float left   = playArea.position.x + r;
    float right  = playArea.position.x + playArea.size.x - r;
    float top    = playArea.position.y + r;
    float bottom = playArea.position.y + playArea.size.y - r;

    bool changed = false;
    if (pos.x < left)  { pos.x = left;   vel.x = std::abs(vel.x);  changed = true; }
    if (pos.x > right) { pos.x = right;  vel.x = -std::abs(vel.x); changed = true; }
    if (pos.y < top)   { pos.y = top;    vel.y = std::abs(vel.y);  changed = true; }
    if (pos.y > bottom){ pos.y = bottom; vel.y = -std::abs(vel.y); changed = true; }

    if (changed) {
        ball.setPosition(pos);
        ball.setVelocity(vel);
    }
}

void Physics::resolveBallCollision(Ball& a, Ball& b) {
    if (a.isPocketed() || b.isPocketed()) return;

    sf::Vector2f delta = b.getPosition() - a.getPosition();
    float dist = std::hypot(delta.x, delta.y);
    float minDist = a.getRadius() + b.getRadius();

    if (dist >= minDist || dist < 0.001f) return;

    // Separate overlapping balls
    sf::Vector2f norm = delta / dist;
    float overlap = minDist - dist;
    a.setPosition(a.getPosition() - norm * (overlap * 0.5f));
    b.setPosition(b.getPosition() + norm * (overlap * 0.5f));

    // 1-D elastic collision along normal (equal masses)
    sf::Vector2f va = a.getVelocity();
    sf::Vector2f vb = b.getVelocity();

    float va_n = va.x * norm.x + va.y * norm.y;
    float vb_n = vb.x * norm.x + vb.y * norm.y;

    if (va_n - vb_n <= 0.f) return; // already separating

    // Swap normal components
    sf::Vector2f va_new = va + (vb_n - va_n) * norm;
    sf::Vector2f vb_new = vb + (va_n - vb_n) * norm;

    a.setVelocity(va_new);
    b.setVelocity(vb_new);
}

void Physics::resolveAllCollisions(std::vector<Ball>& balls) {
    for (std::size_t i = 0; i < balls.size(); ++i)
        for (std::size_t j = i + 1; j < balls.size(); ++j)
            resolveBallCollision(balls[i], balls[j]);
}

std::vector<int> Physics::checkPockets(std::vector<Ball>& balls, const Table& table) {
    std::vector<int> pocketed;
    for (auto& ball : balls) {
        if (ball.isPocketed()) continue;
        Pocket hit{};
        if (table.isInPocket(ball.getPosition(), hit)) {
            ball.setPocketed(true);
            ball.setVelocity({0.f, 0.f});
            pocketed.push_back(ball.getId());
        }
    }
    return pocketed;
}

bool Physics::allAtRest(const std::vector<Ball>& balls) {
    for (auto& b : balls)
        if (!b.isPocketed() && b.isMoving())
            return false;
    return true;
}
