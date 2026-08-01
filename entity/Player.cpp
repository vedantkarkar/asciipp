#include "Player.h"
#include "../input/InputHandler.h"
#include "../world/Map.h"

#include <cstdlib>

Player::Player(int x, int y)
    : Entity(x, y, 'P', 0.5f),
      health(100),
      score(0),
      input(nullptr),
      map(nullptr),
      xFloat(x),
      yFloat(y),
      vx(0.0f),
      vy(0.0f),
      speed(20.0f),
      friction(0.65f),
      glitchEnabled(false),
      glitchFactor(0) {}

// Too slow:	    increase speed (increase speed variable)
// Too slippery: 	increase friction (→ 0.90f)
// Too stiff:	    decrease friction (→ 0.70f)

void Player::attachInput(InputHandler* i) {
    input = i;
}

void Player::attachMap(Map* m) {
    map = m;
}

void Player::update(float dt) {
    if (!input || !map) return;

    // --- Animation ---
    updateAnimation(dt);

    // --- INPUT → ACCELERATION ---
    if (input->isKeyPressed('w')) vy -= speed;
    if (input->isKeyPressed('s')) vy += speed;
    if (input->isKeyPressed('a')) vx -= speed;
    if (input->isKeyPressed('d')) vx += speed;
        

    // --- FRICTION ---
    vx *= friction;
    vy *= friction;

    // --- PREDICT NEXT POSITION ---
    float nextX = xFloat + vx * dt;
    float nextY = yFloat + vy * dt;

    // --- COLLISION (SEPARATE AXES) ---

    // X axis
    if (!map->isBlocked((int)nextX, (int)yFloat)) {
        xFloat = nextX;
    } else {
        vx = 0;
    }

    // Y axis
    if (!map->isBlocked((int)xFloat, (int)nextY)) {
        yFloat = nextY;
    } else {
        vy = 0;
    }

    // --- APPLY TO GRID POSITION ---
    x = (int)xFloat;
    y = (int)yFloat;

    // --- TRIGGERS ---
    map->checkTrigger(x, y);
}

char Player::getRenderSymbol() const {
    char base = animState ? 'P' : 'p';

    if (glitchEnabled) {
        return (std::rand() % 10 < glitchFactor) ? '#' : base;
    }

    return base;
}

int Player::getHealth() const {
    return health;
}

int Player::getScore() const {
    return score;
}

void Player::enableGlitch(int factor) {
    glitchEnabled = true;
    glitchFactor = factor;
}

void Player::disableGlitch() {
    glitchEnabled = false;
    glitchFactor = 0;
}