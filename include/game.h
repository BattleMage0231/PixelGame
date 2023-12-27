#ifndef PIXELGAME_GAME_H_
#define PIXELGAME_GAME_H_

#include <SDL.h>

constexpr size_t WIN_WIDTH = 1600;
constexpr size_t WIN_HEIGHT = 800;

class Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    void setup();
    void cleanup();

    void handleEvent(SDL_Event event);

public:
    Game();
    ~Game();

    void launch();
};

#endif
