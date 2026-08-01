#include "Enemy.h"
#include "../world/Map.h"

#include <cstdlib>

Enemy::Enemy(int x, int y, int type)
    : Entity(x, y, 'E', 0.6f),
      behaviorType(type),
      detectionRadius(6),
      patrolDx(1),
      patrolDy(0),
      player(nullptr),
      map(nullptr),
      moveTimer(0.0f),
      moveInterval(0.3f) {}

void Enemy::attachPlayer(Entity* p) {
    player = p;
}

void Enemy::attachMap(Map* m) {
    map = m;
}

void Enemy::setPatrolAxis(char axis) {
    if (axis == 'y' || axis == 'Y') {
        patrolDx = 0;
        patrolDy = (patrolDy < 0) ? -1 : 1;
        return;
    }

    patrolDx = (patrolDx < 0) ? -1 : 1;
    patrolDy = 0;
}

void Enemy::setPatrolDirection(int dx, int dy) {
    if (dx != 0) {
        patrolDx = (dx < 0) ? -1 : 1;
        patrolDy = 0;
        return;
    }

    if (dy != 0) {
        patrolDx = 0;
        patrolDy = (dy < 0) ? -1 : 1;
    }
}

void Enemy::setDetectionRadius(int radius) {
    detectionRadius = radius;
}

void Enemy::setMoveInterval(float seconds) {
    if (seconds > 0.0f) {
        moveInterval = seconds;
    }
}

void Enemy::update(float dt) {
    if (!map || !alive()) return;

    // Update animation (optional breathing)
    updateAnimation(dt);

    // Movement timing (prevents too fast movement)
    moveTimer += dt;
    if (moveTimer < moveInterval) return;
    moveTimer = 0.0f;

    switch (behaviorType) {
        case 0:
            moveRandom();
            break;

        case 1:
            patrol();
            break;

        case 2:
            moveTowardPlayer();
            break;

        default:
            moveRandom();
            break;
    }
}

char Enemy::getRenderSymbol() const {
    if (!alive()) {
        return ' ';
    }

    // You can disable breathing by returning 'E' directly
    return animState ? 'E' : 'e';
}

bool Enemy::inBounds(int nextX, int nextY) const {
    if (!map) return false;

    const auto& grid = map->getGrid();
    if (nextY < 0 || nextY >= static_cast<int>(grid.size())) return false;
    if (nextX < 0 || nextX >= static_cast<int>(grid[nextY].size())) return false;
    return true;
}

bool Enemy::canOccupy(int nextX, int nextY) const {
    if (!inBounds(nextX, nextY)) {
        return false;
    }

    return !map->isBlocked(nextX, nextY);
}

void Enemy::moveRandom() {
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;

    int newX = x + dx; 
    int newY = y + dy;

    if (canOccupy(newX, newY)) {
        x = newX;
        y = newY;
    }
}

void Enemy::moveTowardPlayer() {
    if (!player) return;

    int dx = player->getX() - x;
    int dy = player->getY() - y;

    int stepX = (dx != 0) ? (dx / abs(dx)) : 0; 
    int stepY = (dy != 0) ? (dy / abs(dy)) : 0;

    int newX = x + stepX; // here x + (1 or -1 or 0)
    int newY = y + stepY; // here y + (1 or -1 or 0)

    // Keep a simple detection radius for nearby aggression but still pursue decisively.
    if (abs(dx) > detectionRadius || abs(dy) > detectionRadius) {
        if (stepX != 0) newX = x + stepX;
        if (stepY != 0) newY = y + stepY;
    }

    if (canOccupy(newX, newY)) {
        x = newX;
        y = newY;
    }
}

void Enemy::patrol() {
    int newX = x + patrolDx;
    int newY = y + patrolDy;

    if (canOccupy(newX, newY)) {
        x = newX;
        y = newY;
        return;
    }

    patrolDx *= -1;
    patrolDy *= -1;

    newX = x + patrolDx;
    newY = y + patrolDy;

    if (canOccupy(newX, newY)) {
        x = newX;
        y = newY;
    }
}
