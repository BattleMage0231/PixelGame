#ifndef PIXELGAME_STATIC_H_
#define PIXELGAME_STATIC_H_

#include <glm/vec2.hpp>
#include <SDL.h>

#include "actor.h"

class StaticActor : public GameActor {
public:
    glm::dvec2 pos;
    SDL_Rect texture;

    StaticActor(glm::dvec2 pos, SDL_Rect texture);
    ~StaticActor();

    void update(size_t timeDelta);
    glm::dvec2 getPosition();
    SDL_Rect getTexture();
};

#endif
