#include <algorithm>
#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "data.h"
#include "enemy.h"
#include "game.h"
#include "static.h"

Game::Game() {}

Game::~Game() {}

void Game::setup() {
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &window, &renderer);
    textures = IMG_LoadTexture(renderer, TEXTURES_PATH.c_str());
    if(!textures) {
        std::cerr << "Couldn't load textures" << std::endl;
    }
    font = TTF_OpenFont("../assets/RubikLines-Regular.ttf", 14);
    if(!font) {
        std::cerr << "Couldn't load font" << std::endl;
    }
    map.loadMap(MAP_1, MAP_1_WIDTH, MAP_1_HEIGHT);
    actors.clear();
    ZBuffer.resize(WIN_WIDTH);
    rayBuffer.resize(WIN_WIDTH);
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        rayBuffer[i] = -1.0 + (2.0 * i) / (WIN_WIDTH - 1);
    }
    player = std::shared_ptr<GamePlayer>(new GamePlayer(
        glm::dvec2 { 3.0, 2.0 }, 
        glm::dvec2 { 1.0, 1.0 }, 
        glm::dvec2 { -0.66, 0.66},
        map
    ));
    // TEMP
    player->camDir = glm::rotate(player->camDir, 1.5);
    player->camPlane = glm::rotate(player->camPlane, 1.5);
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(4.0, 2.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(3.5, 6.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(4.2, 3.7), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<EnemyActor>(glm::dvec2(2.0, 4.0), glm::dvec2(1.0, 0.0), SDL_Rect { 448, 0, 64, 64 }, player, map));
}

void Game::cleanup() {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    player.reset();
    SDL_DestroyTexture(textures);
    textures = nullptr;
    TTF_CloseFont(font);
    font = nullptr;
}

void Game::handleEvent(SDL_Event event) {
    if(event.type == SDL_KEYUP) {
        if(event.key.keysym.sym == 'a' || event.key.keysym.sym == 'd') {
            player->angVel = 0.0;
        }
        if(event.key.keysym.sym == 'w' || event.key.keysym.sym == 's') {
            player->vel = 0.0;
        }
    } else if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == 'w') {
            player->vel = 0.003;
        } else if(event.key.keysym.sym == 's') {
            player->vel = -0.003;
        } else if(event.key.keysym.sym == 'd') {
            player->angVel = 0.005;
        } else if(event.key.keysym.sym == 'a') {
            player->angVel = -0.005;
        }
    }
}

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
    glm::dvec2 endDir = 0.5 * glm::normalize(glm::dvec2(-midpoint.y, midpoint.x));
    glm::dvec2 endA = midpoint + endDir;
    glm::dvec2 endB = midpoint - endDir;
    double tA, tB;

    // project first endpoint onto camera plane
    glm::dmat2 matA(
        -player->camPlane.x, -player->camPlane.y,
        endA.x, endA.y
    );
    double detA = glm::determinant(matA);
    if(glm::abs(detA) < EPS) {
        tA = INF * glm::sign(glm::dot(endA, player->camPlane));
    } else {
        glm::dvec2 res = glm::inverse(matA) * player->camDir;
        if(res.y < 0.0) return;
        tA = res.x;
    }

    // project second endpoint onto camera plane
    glm::dmat2 matB(
        -player->camPlane.x, -player->camPlane.y, 
        endB.x, endB.y
    );
    double detB = glm::determinant(matB);
    if(glm::abs(detB) < EPS) {
        tB = INF * glm::sign(glm::dot(endB, player->camPlane));
    } else {
        glm::dvec2 res = glm::inverse(matB) * player->camDir;
        if(res.y < 0.0) return;
        tB = res.x;
    }

    // get indices of rays
    tA = glm::clamp(tA, -1.0, 1.0);
    tB = glm::clamp(tB, -1.0, 1.0);
    double rayA = (tA + 1.0) * (WIN_WIDTH - 1) / 2.0;
    double rayB = (tB + 1.0) * (WIN_WIDTH - 1) / 2.0;
    size_t idxA = static_cast<size_t>(glm::ceil(rayA));
    size_t idxB = static_cast<size_t>(glm::floor(rayB));
    if(idxB < idxA) {
        std::swap(idxA, idxB);
    }

    for(size_t i = idxA; i <= idxB; ++i) {
        glm::dvec2 projDir = player->camDir + rayBuffer[i] * player->camPlane;

        // get intersection of ray with sprite
        glm::dmat2 mat(
            projDir.x, projDir.y,
            -endDir.x, -endDir.y
        );
        double det = glm::determinant(mat);
        if(glm::abs(det) < EPS) {
            continue;
        }
        glm::dvec2 sol = glm::inverse(mat) * midpoint;
        double t = sol.x, s = sol.y;
        glm::dvec2 spritePoint = t * projDir; // relative to player

        // calculate perpendicular distance and draw
        double perpDist = glm::abs(glm::dot(spritePoint, glm::normalize(player->camDir)));
        if(perpDist > ZBuffer[i]) {
            continue;
        }
        double height = WIN_HEIGHT / perpDist;
        size_t pixel = static_cast<size_t>(texWidth / 2.0 + s * texWidth / 2.0);
        SDL_Rect src { texture.x + static_cast<int>(pixel), texture.y, 1, texture.h };
        SDL_Rect dest { static_cast<int>(i), static_cast<int>(WIN_HEIGHT / 2.0 - height / 2.0), 1, static_cast<int>(height) };

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

void Game::renderDebug(double FPS) {
    SDL_Color textcol = {255, 255, 255};
    std::string str = "FPS: " + std::to_string(static_cast<size_t>(FPS));
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, str.c_str(), textcol);
    SDL_Rect destrec = {0, 0, 500, 100};
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopy(renderer, tex, NULL, &destrec);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(textSurface);
}

void Game::launch() {
    setup();

    size_t timer = SDL_GetTicks();
    double FPS = 0.0;

    // game loop
    while(true) {
        // handle events
        bool exit = false;
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                exit = true;
                break;
            } else {
                handleEvent(event);
            }
        }
        if(exit) {
            break;
        }

        size_t newTime = SDL_GetTicks();
        size_t deltaTime = newTime - timer;

        player->update(deltaTime);
        for(auto actor : actors) {
            actor->update(deltaTime);
        }
        FPS = (FPS * 0.8) + (1000.0 / deltaTime * 0.2);
        timer = newTime;

        populateZBuffer();

        // test code
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderMap();
        renderActors();

        //renderMinimap();
        renderDebug(FPS);

        SDL_RenderPresent(renderer);
    }

    cleanup();
}
