#include "Game.hpp"
#include <cmath>
#include <algorithm>
#include <array>

// ─── Ball colours ────────────────────────────────────────────────────────────
static const std::array<sf::Color, 15> BALL_COLORS = {{
    sf::Color(220, 180,  10),  // 1  yellow solid
    sf::Color( 20,  60, 180),  // 2  blue solid
    sf::Color(180,  20,  20),  // 3  red solid
    sf::Color(100,   0, 120),  // 4  purple solid
    sf::Color(220,  80,  10),  // 5  orange solid
    sf::Color( 10, 130,  30),  // 6  green solid
    sf::Color(140,  20,  40),  // 7  maroon solid
    sf::Color( 20,  20,  20),  // 8  black (8-ball)
    sf::Color(220, 180,  10),  // 9  yellow stripe
    sf::Color( 20,  60, 180),  // 10 blue stripe
    sf::Color(180,  20,  20),  // 11 red stripe
    sf::Color(100,   0, 120),  // 12 purple stripe
    sf::Color(220,  80,  10),  // 13 orange stripe
    sf::Color( 10, 130,  30),  // 14 green stripe
    sf::Color(140,  20,  40),  // 15 maroon stripe
}};

// ─── Triangle rack positions ──────────────────────────────────────────────────
static std::vector<sf::Vector2f> rackPositions(sf::Vector2f footSpot, float r) {
    float gap = r * 2.05f;
    std::vector<sf::Vector2f> pos;
    for (int row = 0; row < 5; ++row)
        for (int col = 0; col <= row; ++col)
            pos.push_back({
                footSpot.x + row * gap * 0.866f,
                footSpot.y + (col - row * 0.5f) * gap
            });
    return pos;
}

// ─── Constructor ─────────────────────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode({WIN_W, WIN_H}), "Billiards"),
      m_isDragging(false),
      m_dragStartPos(0.f, 0.f),
      m_dragDistance(0.f)
{
    m_window.setFramerateLimit(60);

    setupBalls();
    m_cue.setVisible(true);
}

// ─── Ball setup ──────────────────────────────────────────────────────────────
void Game::setupBalls() {
    m_balls.clear();

    sf::FloatRect play = m_table.getPlayArea();
    float r = Ball::RADIUS;

    m_cueBallStart = {play.position.x + play.size.x * 0.25f,
                      play.position.y + play.size.y * 0.5f};
    m_balls.emplace_back(0, BallType::CueBall, m_cueBallStart, sf::Color(245, 245, 245));

    sf::Vector2f footSpot = {play.position.x + play.size.x * 0.72f,
                             play.position.y + play.size.y * 0.5f};

    auto positions = rackPositions(footSpot, r);

    static const std::array<int, 15> RACK = {
        1, 9, 2, 3, 10, 4, 8, 11, 5, 12, 6, 13, 7, 14, 15
    };

    for (int i = 0; i < 15 && i < (int)positions.size(); ++i) {
        int id = RACK[i];
        BallType type = (id >= 9 && id <= 15) ? BallType::Stripe : BallType::Solid;
        sf::Color color = BALL_COLORS[id - 1];
        m_balls.emplace_back(id, type, positions[i], color);
    }
}

Ball* Game::getCueBall() {
    for (auto& b : m_balls)
        if (b.getId() == 0) return &b;
    return nullptr;
}

// ─── Main loop ───────────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        processEvents();
        update(dt);
        render();
    }
}

// ─── Events ──────────────────────────────────────────────────────────────────
void Game::processEvents() {
    while (auto eventOpt = m_window.pollEvent()) {
        const sf::Event& event = *eventOpt;
        if (event.is<sf::Event::Closed>())
            m_window.close();

        // Mouse click: begin dragging to charge
        if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mp->button == sf::Mouse::Button::Left) {
                m_isDragging = true;
                sf::Vector2i pixel = {mp->position.x, mp->position.y};
                m_dragStartPos = m_window.mapPixelToCoords(pixel);
                m_dragDistance = 0.f;
            }
        }

        // Mouse move: update drag distance if currently dragging
        if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
            if (m_isDragging) {
                sf::Vector2i pixel = {mm->position.x, mm->position.y};
                sf::Vector2f curMouse = m_window.mapPixelToCoords(pixel);
                sf::Vector2f diff = curMouse - m_dragStartPos;
                m_dragDistance = std::hypot(diff.x, diff.y);
            }
        }

        // Mouse release: release drag
        if (const auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mr->button == sf::Mouse::Button::Left && m_isDragging) {
                m_isDragging = false;
                m_dragDistance = 0.f;
                m_cue.setPower(0.f);
                // Note: Physics is not in progress yet, so ball does not shoot yet
            }
        }
    }
}

// ─── Update ──────────────────────────────────────────────────────────────────
void Game::update(float /*dt*/) {
    Ball* cue = getCueBall();
    if (cue) {
        m_cue.setVisible(true);
        // Dragging the mouse physically pulls the cue stick back
        m_cue.update(m_window, *cue, m_isDragging, m_dragDistance);
    }
}

// ─── Render ──────────────────────────────────────────────────────────────────
void Game::render() {
    m_window.clear(sf::Color(20, 10, 5));

    // 1. Draw Table (Wood rails, green cloth & 6 pockets)
    m_table.draw(m_window);

    // 2. Draw all 16 Balls
    for (const auto& b : m_balls) {
        b.draw(m_window);
    }

    // 3. Draw Cue Stick
    m_cue.draw(m_window);

    m_window.display();
}
