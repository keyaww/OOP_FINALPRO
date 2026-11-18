#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Ball.hpp"
#include "CueStick.hpp"
#include "Table.hpp"
#include "Physics.hpp"

enum class GameState {
    Menu,
    Playing,
    Victory
};

class Game {
public:
    Game();
    void run();

private:
    // --- Setup ---
    void setupBalls();
    void setupUI();

    // --- Per-frame ---
    void processEvents();
    void update(float dt);
    void render();

    // --- State handlers ---
    void handleMenuEvent(const sf::Event& event);
    void handlePlayingEvent(const sf::Event& event);
    void handleVictoryEvent(const sf::Event& event);

    void updatePlaying(float dt);
    void checkVictory();

    void renderMenu();
    void renderPlaying();
    void renderVictory();

    // --- Helpers ---
    Ball* getCueBall();
    void respawnCueBall();
    std::string determineWinner() const;
    sf::Text makeButton(const std::string& label, sf::Vector2f center, unsigned int size = 36);

    // Window
    sf::RenderWindow m_window;

    // Game objects
    Table m_table;
    std::vector<Ball> m_balls;
    CueStick m_cue;

    // State
    GameState m_state;
    bool m_shooting;        // true while mouse button held (charging)
    float m_shootPower;

    // UI font + elements
    sf::Font m_font;
    // Texts are heap-allocated after font is ready (SFML 3 requires font at construction)
    std::unique_ptr<sf::Text> m_titleText;
    std::unique_ptr<sf::Text> m_playBtn;
    std::unique_ptr<sf::Text> m_quitBtn;
    std::unique_ptr<sf::Text> m_victoryText;
    std::unique_ptr<sf::Text> m_winnerText;
    std::unique_ptr<sf::Text> m_continueBtn;

    // Pocket tracking
    int m_solidsPocketed;
    int m_stripesPocketed;

    // Cue-ball reset position
    sf::Vector2f m_cueBallStart;

    static constexpr unsigned int WIN_W = 1024;
    static constexpr unsigned int WIN_H = 600;
};
