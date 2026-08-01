#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class InputHandler;
class Map;

class Player : public Entity {
private:
    int health;
    int score;

    InputHandler* input;
    Map* map;

    // --- Smooth movement ---
    float xFloat, yFloat;
    float vx, vy;

    // --- Movement tuning ---
    float speed;
    float friction;

    // --- Glitch system ---
    bool glitchEnabled;
    int glitchFactor;

public:
    Player(int x, int y);

    void attachInput(InputHandler* i);
    void attachMap(Map* m);

    void update(float dt) override;

    char getRenderSymbol() const override;

    int getHealth() const;
    int getScore() const;

    void enableGlitch(int factor);
    void disableGlitch();
};

#endif