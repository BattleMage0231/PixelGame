#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

#include "game.h"
#include "map.h"

int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "Couldn't initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return -1;
    }

    Game g;
    g.launch();

    SDL_Quit();
    return 0;
}
