#ifndef PIXELGAME_GAME_H_
#define PIXELGAME_GAME_H_

#include <string>
#include <SDL.h>

#include "map.h"

constexpr size_t WIN_WIDTH = 1600;
constexpr size_t WIN_HEIGHT = 800;

const std::string TEXTURES_PATH = "../assets/wolftexturesobj.gif";

class Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* textures;

    GameMap map;

    void setup();
    void cleanup();

    void handleEvent(SDL_Event event);

public:
    Game();
    ~Game();

    void launch();
};

#endif
