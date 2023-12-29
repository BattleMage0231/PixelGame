#ifndef PIXELGAME_ACTOR_H_
#define PIXELGAME_ACTOR_H_

#include <glm/vec2.hpp>
#include <SDL.h>

class GameActor {
public:
    virtual void update(size_t timeDelta) = 0;
    virtual glm::dvec2 getPosition() = 0;
    virtual SDL_Rect getTexture() = 0;
};

#endif
