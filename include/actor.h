#ifndef PIXELGAME_ACTOR_H_
#define PIXELGAME_ACTOR_H_

#include <SDL.h>

class GameActor {
public:
    virtual void update(size_t timeDelta) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};

#endif
