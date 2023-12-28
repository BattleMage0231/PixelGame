#ifndef PIXELGAME_PLAYER_H_
#define PIXELGAME_PLAYER_H_

#include <glm/vec2.hpp>

#include "actor.h"
#include "map.h"

class GamePlayer : GameActor {
public:
    glm::dvec2 pos;

    // camera direction and plane
    glm::dvec2 camDir;
    glm::dvec2 camPlane;

    // velocity and angular velocity per ms
    double vel;
    double angVel;

    GameMap& map;

    GamePlayer(glm::dvec2 pos, glm::dvec2 dir, glm::dvec2 plane, GameMap& map);
    ~GamePlayer();

    void update(size_t timeDelta);
    void render(SDL_Renderer* renderer);
};

#endif
