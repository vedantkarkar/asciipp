#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

class Map;

class Enemy : public Entity {
private:
    int behaviorType;
    int detectionRadius;
    int patrolDx;
    int patrolDy;

    Entity* player;
    Map* map;

    // Movement control
    float moveTimer;
    float moveInterval;

public:
    Enemy(int x, int y, int type);

    void attachPlayer(Entity* p);
    void attachMap(Map* m);
    void setPatrolAxis(char axis);
    void setPatrolDirection(int dx, int dy);
    void setDetectionRadius(int radius);
    void setMoveInterval(float seconds);

    void update(float dt) override;

    // Rendering override (optional animation)
    char getRenderSymbol() const override;

private:
    bool inBounds(int nextX, int nextY) const;
    bool canOccupy(int nextX, int nextY) const;
    void moveRandom();
    void moveTowardPlayer();
    void patrol();
};

#endif
