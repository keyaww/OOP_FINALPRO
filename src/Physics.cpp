#include "Physics.hpp"

// =============================================================================
// TODO (18 - 21 November 2026): Implementasi fungsi-fungsi fisika
// - resolveWalls()
// - resolveBallCollision()
// - resolveAllCollisions()
// - checkPockets()
// - allAtRest()
// Saat ini masih dalam tahap pengerjaan (NOT IN PROGRESS / STUB)
// =============================================================================

void Physics::resolveWalls(Ball& /*ball*/, const sf::FloatRect& /*playArea*/) {
    // TODO: implement wall collision & bounce
}

void Physics::resolveBallCollision(Ball& /*a*/, Ball& /*b*/) {
    // TODO: implement elastic ball-to-ball collision
}

void Physics::resolveAllCollisions(std::vector<Ball>& /*balls*/) {
    // TODO: loop through pairwise balls and resolve collision
}

std::vector<int> Physics::checkPockets(std::vector<Ball>& /*balls*/, const Table& /*table*/) {
    // TODO: check if balls enter pockets
    return {};
}

bool Physics::allAtRest(const std::vector<Ball>& /*balls*/) {
    // Return true for now so cue stick can aim in demo
    return true;
}
