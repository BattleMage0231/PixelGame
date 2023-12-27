#include "game.h"

Game::Game() {}

Game::~Game() {}

void Game::setup() {
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &this->window, &this->renderer);
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
    }

    cleanup();
}
