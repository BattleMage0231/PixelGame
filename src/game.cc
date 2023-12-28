#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
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
    player = std::shared_ptr<GamePlayer>(new GamePlayer(
        glm::dvec2 { 2.5, 3.5 }, 
        glm::dvec2 { 1.0, 1.0 }, 
        glm::dvec2 { -0.66, 0.66},
        map
    ));
    player->angVel = 0.001;
}

void Game::cleanup() {
    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
    SDL_DestroyWindow(this->window);
    this->window = nullptr;
    player.reset();
}

void Game::handleEvent(SDL_Event event) {}

void Game::renderMinimap() {
    for(int i = 0; i < map.getWidth(); ++i) {
        for(int j = 0; j < map.getHeight(); ++j) {
            if(map.isOpaque(i, j)) {
                SDL_Rect src = map.getTexture(i, j);
                SDL_Rect dest = {64 * i, 64 * j, 64, 64};
                SDL_RenderCopy(renderer, textures, &src, &dest);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        // calculate the scaled distance for the ith ray
        double t = -1.0 + (2.0 * i) / (WIN_WIDTH - 1);
        glm::dvec2 projDir = player->camDir + t * player->camPlane;
        glm::dvec2 wall = map.castRay(player->pos, projDir);
        glm::dvec2 delta = wall - player->pos;

        SDL_RenderDrawLine(renderer, 64 * player->pos.x, 64 * player->pos.y, 64 * wall.x, 64 * wall.y);
    }
}

void Game::renderMap() {
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        // calculate the scaled distance for the ith ray
        double t = -1.0 + (2.0 * i) / (WIN_WIDTH - 1);
        glm::dvec2 projDir = player->camDir + t * player->camPlane;
        glm::dvec2 wall = map.castRay(player->pos, projDir);
        glm::dvec2 delta = wall - player->pos;
        double scaledDist = glm::abs(glm::dot(delta, glm::normalize(player->camDir)));

        // get the right column to draw
        double amtX = glm::abs(wall.x - glm::round(wall.x));
        double amtY = glm::abs(wall.y - glm::round(wall.y));
        double amt = glm::max(amtX, amtY);
        SDL_Rect src = map.getColumn(wall.x, wall.y, amt);

        // calculate the height
        double height = WIN_HEIGHT / scaledDist;
        SDL_Rect dest { static_cast<int>(i), static_cast<int>(WIN_HEIGHT / 2.0 - height / 2.0), 1, static_cast<int>(height) };

        SDL_RenderCopy(renderer, textures, &src, &dest);
    }
}

void Game::launch() {
    setup();

    size_t timer = SDL_GetTicks();

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

        size_t newTime = SDL_GetTicks();
        if(newTime - timer <= 1000.0 / FPS_MAX) continue;

        player->update(newTime);
        timer = newTime;

        // test code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        renderMap();

        SDL_RenderPresent(renderer);
    }

    cleanup();
}
