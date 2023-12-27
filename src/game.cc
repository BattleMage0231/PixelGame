#include <iostream>
#include <SDL_image.h>

#include "game.h"

Game::Game() {}

Game::~Game() {}

void Game::setup() {
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &window, &renderer);
    textures = IMG_LoadTexture(renderer, TEXTURES_PATH.c_str());
    if(!textures) {
        std::cerr << "Couldn't load textures " << std::endl;
    }
    map.loadMap(MAP_1, MAP_1_WIDTH, MAP_1_HEIGHT);
}

void Game::cleanup() {
    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
    SDL_DestroyWindow(this->window);
    this->window = nullptr;
}

void Game::handleEvent(SDL_Event event) {}

void Game::launch() {
    setup();

    // game loop
    while(true) {
        // handle events
        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                break;
            } else {
                handleEvent(event);
            }
        }

        // test code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for(int i = 0; i < map.getWidth(); ++i) {
            for(int j = 0; j < map.getHeight(); ++j) {
                if(map.isOpaque(i, j)) {
                    SDL_Rect src = map.getTexture(i, j);
                    SDL_Rect dest = {64 * i, 64 * j, 64, 64};
                    SDL_RenderCopy(renderer, textures, &src, &dest);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    cleanup();
}
