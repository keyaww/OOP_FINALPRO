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
    void setupBalls();
    void processEvents();
    void update(float dt);
    void render();

    Ball* getCueBall();

    // Window
    sf::RenderWindow m_window;

    // Game objects
    Table m_table;
    std::vector<Ball> m_balls;
    CueStick m_cue;

    // Drag-to-charge controls
    bool m_isDragging;
    sf::Vector2f m_dragStartPos;
    float m_dragDistance;

    // Cue-ball initial position
    sf::Vector2f m_cueBallStart;

    static constexpr unsigned int WIN_W = 1024;
    static constexpr unsigned int WIN_H = 600;
};
