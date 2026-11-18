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

// ─── Helper: make centred Text (requires font already loaded) ─────────────────
static std::unique_ptr<sf::Text> makeCentredText(
    const sf::Font& font,
    const std::string& label,
    sf::Vector2f center,
    unsigned int size)
{
    auto t = std::make_unique<sf::Text>(font, label, size);
    sf::FloatRect b = t->getLocalBounds();
    t->setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f,
                              b.position.y + b.size.y / 2.f));
    t->setPosition(center);
    return t;
}

// ─── Constructor ─────────────────────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode({WIN_W, WIN_H}), "Billiards"),
      m_state(GameState::Menu),
      m_shooting(false),
      m_shootPower(0.f),
      m_solidsPocketed(0),
      m_stripesPocketed(0)
{
    m_window.setFramerateLimit(60);

    // Load a system font (macOS)
    bool ok = m_font.openFromFile("/System/Library/Fonts/Helvetica.ttc");
    if (!ok) ok = m_font.openFromFile("/Library/Fonts/Arial.ttf");
    if (!ok) ok = m_font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf");
    if (!ok) m_font.openFromFile("/System/Library/Fonts/Geneva.ttf");

    setupUI();
    setupBalls();
}

// ─── UI setup ────────────────────────────────────────────────────────────────
sf::Text Game::makeButton(const std::string& label, sf::Vector2f center, unsigned int size) {
    sf::Text t(m_font, label, size);
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f,
                             b.position.y + b.size.y / 2.f));
    t.setPosition(center);
    return t;
}

void Game::setupUI() {
    float cx = WIN_W / 2.f;
    float cy = WIN_H / 2.f;

    // ── Menu ──
    m_titleText  = makeCentredText(m_font, "BILLIARDS",  {cx, cy - 100.f}, 60);
    m_titleText->setFillColor(sf::Color(240, 210, 50));

    m_playBtn    = makeCentredText(m_font, "Play",        {cx, cy + 10.f},  42);
    m_playBtn->setFillColor(sf::Color::White);

    m_quitBtn    = makeCentredText(m_font, "Quit",        {cx, cy + 80.f},  42);
    m_quitBtn->setFillColor(sf::Color(220, 80, 80));

    // ── Victory ──
    m_victoryText = makeCentredText(m_font, "Game Over!", {cx, cy - 80.f}, 54);
    m_victoryText->setFillColor(sf::Color(240, 210, 50));

    m_winnerText  = makeCentredText(m_font, "",           {cx, cy},         40);
    m_winnerText->setFillColor(sf::Color::White);

    m_continueBtn = makeCentredText(m_font, "Continue",   {cx, cy + 100.f}, 40);
    m_continueBtn->setFillColor(sf::Color(100, 220, 100));
}

// ─── Ball setup ──────────────────────────────────────────────────────────────
void Game::setupBalls() {
    m_balls.clear();
    m_solidsPocketed = 0;
    m_stripesPocketed = 0;

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

// ─── Helpers ─────────────────────────────────────────────────────────────────
Ball* Game::getCueBall() {
    for (auto& b : m_balls)
        if (b.getId() == 0) return &b;
    return nullptr;
}

void Game::respawnCueBall() {
    Ball* cue = getCueBall();
    if (cue) {
        cue->setPosition(m_cueBallStart);
        cue->setVelocity({0.f, 0.f});
        cue->setPocketed(false);
    }
}

std::string Game::determineWinner() const {
    if (m_solidsPocketed >= 7) return "Solids has won!";
    return "Stripes has won!";
}

void Game::checkVictory() {
    int activeSolids = 0, activeStripes = 0;
    for (auto& b : m_balls) {
        if (b.getId() >= 1 && b.getId() <= 7 && !b.isPocketed()) activeSolids++;
        if (b.getId() >= 9 && b.getId() <= 15 && !b.isPocketed()) activeStripes++;
    }
    if (activeSolids == 0 || activeStripes == 0) {
        std::string winner = (activeSolids == 0) ? "Solids has won!" : "Stripes has won!";
        m_winnerText->setString(winner);
        sf::FloatRect b = m_winnerText->getLocalBounds();
        m_winnerText->setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f,
                                             b.position.y + b.size.y / 2.f));
        m_winnerText->setPosition({WIN_W / 2.f, WIN_H / 2.f});
        m_state = GameState::Victory;
    }
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

        switch (m_state) {
            case GameState::Menu:    handleMenuEvent(event);    break;
            case GameState::Playing: handlePlayingEvent(event); break;
            case GameState::Victory: handleVictoryEvent(event); break;
        }
    }
}

static bool hitTest(const sf::Text& text, sf::Vector2f pt) {
    sf::FloatRect bounds = text.getGlobalBounds();
    bounds.position  -= sf::Vector2f(10.f, 10.f);
    bounds.size      += sf::Vector2f(20.f, 20.f);
    return bounds.contains(pt);
}

void Game::handleMenuEvent(const sf::Event& event) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f pt = m_window.mapPixelToCoords({mp->position.x, mp->position.y});
        if (hitTest(*m_playBtn, pt)) {
            setupBalls();
            m_state = GameState::Playing;
            m_cue.setVisible(true);
        }
        if (hitTest(*m_quitBtn, pt))
            m_window.close();
    }
}

void Game::handlePlayingEvent(const sf::Event& event) {
    if (!Physics::allAtRest(m_balls)) return;

    Ball* cue = getCueBall();
    if (!cue || cue->isPocketed()) {
        respawnCueBall();
        cue = getCueBall();
    }

    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            m_shooting = true;
            m_shootPower = 0.f;
        }
    }
    if (const auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left && m_shooting) {
            m_shooting = false;
            sf::Vector2f vel = m_cue.shoot(m_shootPower);
            if (cue) cue->setVelocity(vel);
            m_cue.setPower(0.f);
        }
    }
}

void Game::handleVictoryEvent(const sf::Event& event) {
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2f pt = m_window.mapPixelToCoords({mp->position.x, mp->position.y});
        if (hitTest(*m_continueBtn, pt)) {
            m_state = GameState::Menu;
            m_cue.setVisible(false);
            setupBalls();
        }
    }
}

// ─── Update ──────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    if (m_state == GameState::Playing)
        updatePlaying(dt);
}

void Game::updatePlaying(float dt) {
    if (m_shooting) {
        m_shootPower = std::min(m_shootPower + 400.f * dt, CueStick::MAX_POWER);
        m_cue.setPower(m_shootPower);
    }

    bool atRest = Physics::allAtRest(m_balls);

    Ball* cue = getCueBall();
    if (cue && cue->isPocketed()) respawnCueBall();

    m_cue.setVisible(atRest);
    if (atRest && cue && !cue->isPocketed())
        m_cue.update(m_window, *cue);

    for (auto& b : m_balls) b.update(dt);
    Physics::resolveAllCollisions(m_balls);
    for (auto& b : m_balls)
        if (!b.isPocketed())
            Physics::resolveWalls(b, m_table.getPlayArea());

    auto pocketed = Physics::checkPockets(m_balls, m_table);
    for (int id : pocketed) {
        if (id == 0) {
            respawnCueBall();
        } else if (id >= 1 && id <= 7) {
            m_solidsPocketed++;
        } else if (id == 8) {
            checkVictory();
            return;
        } else {
            m_stripesPocketed++;
        }
    }

    checkVictory();
}

// ─── Render ──────────────────────────────────────────────────────────────────
void Game::render() {
    m_window.clear(sf::Color(20, 10, 5));
    switch (m_state) {
        case GameState::Menu:    renderMenu();    break;
        case GameState::Playing: renderPlaying(); break;
        case GameState::Victory: renderVictory(); break;
    }
    m_window.display();
}

void Game::renderMenu() {
    sf::RectangleShape bg(sf::Vector2f(WIN_W, WIN_H));
    bg.setFillColor(sf::Color(15, 60, 25));
    m_window.draw(bg);

    m_window.draw(*m_titleText);
    m_window.draw(*m_playBtn);
    m_window.draw(*m_quitBtn);
}

void Game::renderPlaying() {
    m_table.draw(m_window);
    for (auto& b : m_balls) b.draw(m_window);
    m_cue.draw(m_window);

    sf::Text hud(m_font,
        "Solids: " + std::to_string(m_solidsPocketed) + "/7   "
        "Stripes: " + std::to_string(m_stripesPocketed) + "/7",
        22);
    hud.setFillColor(sf::Color::White);
    hud.setPosition({10.f, 4.f});
    m_window.draw(hud);

    if (m_shooting) {
        float barW = 200.f;
        float filled = (m_shootPower / CueStick::MAX_POWER) * barW;
        sf::RectangleShape barBg(sf::Vector2f(barW, 14.f));
        barBg.setPosition({WIN_W - barW - 12.f, 8.f});
        barBg.setFillColor(sf::Color(50, 50, 50, 180));
        m_window.draw(barBg);

        sf::RectangleShape barFill(sf::Vector2f(filled, 14.f));
        barFill.setPosition({WIN_W - barW - 12.f, 8.f});
        float t = m_shootPower / CueStick::MAX_POWER;
        barFill.setFillColor(sf::Color(
            static_cast<uint8_t>(255 * t),
            static_cast<uint8_t>(255 * (1.f - t)),
            0));
        m_window.draw(barFill);

        sf::Text pwrLabel(m_font, "Power", 18);
        pwrLabel.setPosition({WIN_W - barW - 12.f, 24.f});
        pwrLabel.setFillColor(sf::Color::White);
        m_window.draw(pwrLabel);
    }
}

void Game::renderVictory() {
    sf::RectangleShape overlay(sf::Vector2f(WIN_W, WIN_H));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    m_window.draw(overlay);

    m_window.draw(*m_victoryText);
    m_window.draw(*m_winnerText);
    m_window.draw(*m_continueBtn);
}
