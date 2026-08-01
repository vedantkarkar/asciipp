#ifndef ENTITY_H
#define ENTITY_H

class Entity {
protected:
    int x, y;
    char symbol;
    bool isAlive;

    // Animation system
    float animTimer;
    float animInterval;
    bool animState;

public:
    Entity(int startX, int startY, char sym, float interval = 0.5f);
    virtual ~Entity();

    // Core behavior
    virtual void update(float dt) = 0;

    // Animation
    void updateAnimation(float dt);

    // Rendering (IMPORTANT)
    virtual char getRenderSymbol() const;

    // Getters
    int getX() const;
    int getY() const;
    bool alive() const;

    // Setters
    void setPosition(int newX, int newY);
    void kill();
};

#endif