#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL_image.h>

#include <algorithm>
#include "data.h"
#include "game.h"
#include "static.h"

Game::Game() {}

Game::~Game() {}

void Game::setup() {
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &window, &renderer);
    textures = IMG_LoadTexture(renderer, TEXTURES_PATH.c_str());
    if(!textures) {
        std::cerr << "Couldn't load textures " << std::endl;
    }
    map.loadMap(MAP_1, MAP_1_WIDTH, MAP_1_HEIGHT);
    actors.clear();
    MAP_1_LOAD_ACTORS(actors);
    ZBuffer.resize(WIN_WIDTH);
    rayBuffer.resize(WIN_WIDTH);
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        rayBuffer[i] = -1.0 + (2.0 * i) / (WIN_WIDTH - 1);
    }
    player = std::shared_ptr<GamePlayer>(new GamePlayer(
        glm::dvec2 { 2.5, 3.5 }, 
        glm::dvec2 { 1.0, 1.0 }, 
        glm::dvec2 { -0.66, 0.66},
        map
    ));
    player->angVel = 0.005;
}

void Game::cleanup() {
    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
    SDL_DestroyWindow(this->window);
    this->window = nullptr;
    player.reset();
}

void Game::handleEvent(SDL_Event event) {}

void Game::populateZBuffer() {
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        glm::dvec2 projDir = player->camDir + rayBuffer[i] * player->camPlane;
        glm::dvec2 wall = map.castRay(player->pos, projDir);
        glm::dvec2 delta = wall - player->pos;
        ZBuffer[i] = glm::abs(glm::dot(delta, glm::normalize(player->camDir)));
    }
}

glm::dvec2 Game::getEndpoint(size_t ray) {
    glm::dvec2 projDir = player->camDir + rayBuffer[ray] * player->camPlane;
    return player->pos + ZBuffer[ray] / glm::length(player->camDir) * projDir;
}

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
        glm::dvec2 wall = getEndpoint(i);

        SDL_RenderDrawLine(renderer, 64 * player->pos.x, 64 * player->pos.y, 64 * wall.x, 64 * wall.y);
    }
}

void Game::renderMap() {
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        glm::dvec2 wall = getEndpoint(i);

        // get the column to draw
        double amtX = glm::abs(wall.x - glm::round(wall.x));
        double amtY = glm::abs(wall.y - glm::round(wall.y));
        double amt = glm::max(amtX, amtY);
        SDL_Rect src = map.getColumn(wall.x, wall.y, amt);

        // calculate the height
        double height = WIN_HEIGHT / ZBuffer[i];
        SDL_Rect dest { static_cast<int>(i), static_cast<int>(WIN_HEIGHT / 2.0 - height / 2.0), 1, static_cast<int>(height) };

        SDL_RenderCopy(renderer, textures, &src, &dest);
    }
}

void Game::renderActor(std::shared_ptr<GameActor> actor) {
    glm::dvec2 pos = actor->getPosition();
    SDL_Rect texture = actor->getTexture();
    double texWidth = static_cast<double>(texture.w);

    // get endpoints
    glm::dvec2 midpoint = pos - player->pos;
    glm::dvec2 endDir = texWidth / 2.0 * glm::normalize(glm::dvec2(-midpoint.y, midpoint.x));
    glm::dvec2 endA = midpoint + endDir;
    glm::dvec2 endB = midpoint - endDir;
    double tA, tB;

    // project first endpoint onto camera plane
    glm::dmat2 matA(glm::dvec2(-player->camPlane.x, endA.x), glm::dvec2(-player->camPlane.y, endA.y));
    double detA = glm::determinant(matA);
    if(glm::abs(detA) < EPS) {
        tA = INF * glm::sign(glm::dot(endA, player->camPlane));
    } else {
        tA = (glm::inverse(matA) * player->camDir).x;
    }

    // project second endpoint onto camera plane
    glm::dmat2 matB(glm::dvec2(-player->camPlane.x, endB.x), glm::dvec2(-player->camPlane.y, endB.y));
    double detB = glm::determinant(matB);
    if(glm::abs(detB) < EPS) {
        tB = INF * glm::sign(glm::dot(endB, player->camPlane));
    } else {
        tB = (glm::inverse(matB) * player->camDir).x;
    }

    // get indices of rays
    tA = glm::clamp(tA, -1.0, 1.0);
    tB = glm::clamp(tA, -1.0, 1.0);
    double rayA = (tA + 1.0) / (2.0 * (WIN_WIDTH - 1));
    double rayB = (tB + 1.0) / (2.0 * (WIN_WIDTH - 1));
    if(rayA > rayB) {
        std::swap(rayA, rayB);
    }
    size_t idxA = static_cast<size_t>(glm::ceil(rayA));
    size_t idxB = static_cast<size_t>(glm::floor(rayB));
    if(idxA > idxB) return;

    for(size_t i = idxA; i <= idxB; ++i) {
        glm::dvec2 projDir = player->camDir + rayBuffer[i] * player->camPlane;

        // get intersection of ray with sprite
        glm::dmat2 mat(glm::dvec2(projDir.x, endDir.x), glm::dvec2(projDir.y, endDir.y));
        double det = glm::determinant(mat);
        if(glm::abs(det) < EPS) {
            continue;
        }
        glm::dvec2 sol = glm::inverse(mat) * midpoint;
        double t = sol.x, s = sol.y;
        glm::dvec2 spritePoint = t * projDir;
        if(s < -1.0 || s > 1.0) {
            continue;
        }
        // s = -s;

        // calculate perpendicular distance and draw
        double perpDist = glm::abs(glm::dot(spritePoint, glm::normalize(player->camDir)));
        if(perpDist > ZBuffer[i]) {
            continue;
        }
        double height = WIN_HEIGHT / perpDist;
        size_t pixel = static_cast<size_t>(texWidth / 2.0 + s * texWidth / 2.0);
        SDL_Rect src { texture.x + static_cast<int>(pixel), texture.y, 1, texture.h };
        SDL_Rect dest { static_cast<int>(i), static_cast<int>(WIN_HEIGHT / 2.0 - height / 2.0), 1, static_cast<int>(height) };
        std::cout << "RENDERED SOMETHING " << i << std::endl;

        SDL_RenderCopy(renderer, textures, &src, &dest);
    }
}
    
void Game::renderActors() {
    // sort the actors from farthest to closest
    std::sort(actors.begin(), actors.end(), [&](const auto& lhs, const auto& rhs) {
        glm::dvec2 lPos = lhs->getPosition();
        glm::dvec2 rPos = rhs->getPosition();
        return glm::length(player->pos - lPos) >= glm::length(player->pos - rPos);
    });
    for(auto actor : actors) {
        renderActor(actor);
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

        populateZBuffer();

        // test code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        renderMap();

        renderMinimap();
        renderActors();

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect test { (int) (64 * 3.0), (int) (64 * 3.0), 5, 5 };
        SDL_RenderDrawRect(renderer, &test);

        SDL_RenderPresent(renderer);
    }

    cleanup();
}
