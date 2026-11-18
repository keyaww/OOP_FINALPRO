#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"
#include "Table.hpp"

class Physics {
public:
    // Bounce balls off table cushions
    static void resolveWalls(Ball& ball, const sf::FloatRect& playArea);

    // Ball-to-ball elastic collision
    static void resolveBallCollision(Ball& a, Ball& b);

    // Check and resolve all pairwise ball collisions in the vector
    static void resolveAllCollisions(std::vector<Ball>& balls);

    // Check whether any ball has entered a pocket; pockets out those that have
    // Returns list of pocketed ball ids this frame
    static std::vector<int> checkPockets(std::vector<Ball>& balls, const Table& table);

    // Returns true when ALL balls have stopped moving
    static bool allAtRest(const std::vector<Ball>& balls);
};
