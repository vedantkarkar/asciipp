#include "Entity.h"

Entity::Entity(int startX, int startY, char sym, float interval)
    : x(startX),
      y(startY),
      symbol(sym),
      isAlive(true),
      animTimer(0.0f),
      animInterval(interval),
      animState(false) {}

Entity::~Entity() {}

void Entity::updateAnimation(float dt) {
    animTimer += dt;

    if (animTimer >= animInterval) {
        animTimer = 0.0f;
        animState = !animState;
    }
}

char Entity::getRenderSymbol() const {
    return symbol;
}

int Entity::getX() const {
    return x;
}

int Entity::getY() const {
    return y;
}

bool Entity::alive() const {
    return isAlive;
}

void Entity::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Entity::kill() {
    isAlive = false;
}