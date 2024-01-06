#ifndef PIXELGAME_ENEMY_H_
#define PIXELGAME_ENEMY_H_

#include <memory>
#include <glm/vec2.hpp>
#include <SDL.h>

#include "map.h"
#include "player.h"
#include "actor.h"

class EnemyActor : public GameActor {
    std::shared_ptr<GamePlayer> player;
    GameMap& map;

    bool running;
    size_t runAnim;
    size_t runTimer;

    bool shooting;
    size_t shootAnim;
    size_t shootTimer;

public:
    glm::dvec2 pos;
    glm::dvec2 dir;
    double vel;

    int health;

    EnemyActor(glm::dvec2 pos, glm::dvec2 dir, std::shared_ptr<GamePlayer> player, GameMap& map);
    ~EnemyActor();

    glm::dvec2 pathfind();
    void update(size_t timeDelta);
    glm::dvec2 getPosition();
    SDL_Rect getTexture();
};

#endif
