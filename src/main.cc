#include <iostream>
#include <memory>
#include <SDL.h>

#include "game.h"

int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    std::unique_ptr<Game> g(new Game());
    g->launch();

    SDL_Quit();
    return 0;
}
