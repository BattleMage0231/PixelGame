#ifndef PIXELGAME_GAME_H_
#define PIXELGAME_GAME_H_

#include <memory>
#include <string>
#include <SDL.h>

#include "map.h"
#include "player.h"

constexpr size_t WIN_WIDTH = 1024;
constexpr size_t WIN_HEIGHT = 800;

constexpr size_t FPS_MAX = 120;

const std::string TEXTURES_PATH = "../assets/wolftexturesobj.gif";

class Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* textures;

    std::shared_ptr<GamePlayer> player;
    GameMap map;

    void setup();
    void cleanup();

    void handleEvent(SDL_Event event);

    void renderMinimap();
    void renderMap();

public:
    Game();
    ~Game();

    void launch();
};

#endif
