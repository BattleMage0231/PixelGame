#ifndef PIXELGAME_ENEMY_H_
#define PIXELGAME_ENEMY_H_

#include <memory>
#include <glm/vec2.hpp>
#include <SDL.h>

#include "map.h"
#include "player.h"
#include "actor.h"

class EnemyActor : public GameActor {
public:
    glm::dvec2 pos;
    glm::dvec2 dir;
    double vel;
    SDL_Rect texture;
    std::shared_ptr<GamePlayer> player;
    GameMap& map;

    EnemyActor(glm::dvec2 pos, glm::dvec2 dir, SDL_Rect texture, std::shared_ptr<GamePlayer> player, GameMap& map);
    ~EnemyActor();

    void update(size_t timeDelta);
    glm::dvec2 getPosition();
    SDL_Rect getTexture();
};

#endif
