#ifndef PIXELGAME_GAME_H_
#define PIXELGAME_GAME_H_

#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include <SDL.h>

#include "actor.h"
#include "map.h"
#include "player.h"

class Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* textures;
    TTF_Font* font;

    std::shared_ptr<GamePlayer> player;
    std::vector<std::shared_ptr<GameActor>> actors;

    GameMap map;
    std::vector<double> ZBuffer;
    std::vector<double> rayBuffer;

    void setup();
    void cleanup();

    void loadMap1();

    void handleEvent(SDL_Event event);

    void populateZBuffer();
    glm::dvec2 getEndpoint(size_t ray);

    void renderMinimap();
    void renderMap();
    void renderActor(std::shared_ptr<GameActor> actor);
    void renderActors();
    void renderPlayer();
    void renderDebug(double FPS);

    void useItem();

public:
    Game();
    ~Game();

    void launch();
};

#endif
