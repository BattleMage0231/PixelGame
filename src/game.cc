#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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
    ZBuffer.resize(WIN_WIDTH);
    rayBuffer.resize(WIN_WIDTH);
    for(size_t i = 0; i < WIN_WIDTH; ++i) {
        rayBuffer[i] = -1.0 + (2.0 * i) / (WIN_WIDTH - 1);
    }
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

void Game::loadMap1() {
    std::ifstream input(MAP_1_PATH, std::ios::binary);
    std::vector<size_t> mapData(std::istreambuf_iterator<char>(input), {});
    map.loadMap(mapData, MAP_1_WIDTH, MAP_1_HEIGHT);
    actors.clear();
    player = std::shared_ptr<GamePlayer>(new GamePlayer(
        glm::dvec2 { 2.5, 17.5 }, 
        glm::dvec2 { 1.0, 1.0 }, 
        glm::dvec2 { -0.66, 0.66},
        map
    ));
    player->camDir = glm::rotate(player->camDir, 1.5);
    player->camPlane = glm::rotate(player->camPlane, 1.5);
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(2.5, 16.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(2.5, 5.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(12.5, 5.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<EnemyActor>(glm::dvec2(5.5, 4.0), glm::dvec2(1.0, 0.0), player, map));
}

void Game::handleEvent(SDL_Event event) {
    if(event.type == SDL_KEYUP) {
        if(event.key.keysym.sym == 'a' || event.key.keysym.sym == 'd') {
            player->angVel = 0.0;
        } else if(event.key.keysym.sym == 'w' || event.key.keysym.sym == 's') {
            player->vel = 0.0;
        } else if('0' <= event.key.keysym.sym && event.key.keysym.sym <= '9') {
            size_t slot = static_cast<size_t>(event.key.keysym.sym - '0');
            player->slot = slot;
            player->animStep = 0;
        } else if(event.key.keysym.sym == ' ') {
            player->animStep = 1;
            player->animTimer = 0;
            useItem();
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
    // render sky and floor
    SDL_Rect skyRect { 0, 0, WIN_WIDTH, WIN_HEIGHT / 2 };
    SDL_SetRenderDrawColor(renderer, 144, 201, 222, 255);
    SDL_RenderFillRect(renderer, &skyRect);
    SDL_Rect floorRect { 0, WIN_HEIGHT / 2 + 1, WIN_WIDTH, WIN_HEIGHT / 2 };
    SDL_SetRenderDrawColor(renderer, 48, 54, 56, 255);
    SDL_RenderFillRect(renderer, &floorRect);
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

void Game::renderPlayer() {
    // render held item
    SDL_Rect itemTexture = player->getTexture();
    SDL_Rect itemDest { 
        static_cast<int>(WIN_WIDTH / 2.0 - itemTexture.w / 2.0 * ITEM_SIZE_MULT), 
        static_cast<int>(WIN_HEIGHT - ITEM_SIZE_MULT * itemTexture.h), 
        static_cast<int>(ITEM_SIZE_MULT * itemTexture.w), 
        static_cast<int>(ITEM_SIZE_MULT * itemTexture.h) 
    };
    SDL_RenderCopy(renderer, textures, &itemTexture, &itemDest);

    // render crosshair
    int midX = WIN_WIDTH / 2;
    int midY = WIN_HEIGHT / 2;
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderDrawLine(
        renderer,
        midX - CROSSHAIR_LENGTH, midY,
        midX + CROSSHAIR_LENGTH, midY
    );
    SDL_RenderDrawLine(
        renderer,
        midX, midY - CROSSHAIR_LENGTH,
        midX, midY + CROSSHAIR_LENGTH
    );

    // render health bar
    int barX = 30;
    int barY = WIN_HEIGHT - 50;
    SDL_Rect surround { barX - 10, barY - 10, 220, 50 };
    SDL_Rect bar { barX, barY, 200 * player->health / PLAYER_MAX_HEALTH, 30 };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &surround);
    SDL_SetRenderDrawColor(renderer, 102, 255, 102, 255);
    SDL_RenderFillRect(renderer, &bar);
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

void Game::useItem() {
    if(player->slot != GUN_SLOT && player->slot != KNIFE_SLOT) {
        return;
    }
    size_t midRay = WIN_WIDTH / 2;
    // todo: use isSolid instead of isOpaque in ray casting
    glm::dvec2 foot = getEndpoint(midRay);
    glm::dvec2 diff = foot - player->pos;
    bool hasHit = false;
    double hitT = INF;
    std::shared_ptr<GameActor> hitSprite;
    for(auto actor : actors) {
        glm::dvec2 midpoint = actor->getPosition() - player->pos;
        glm::dvec2 endDir = 0.5 * glm::normalize(glm::dvec2(-midpoint.y, midpoint.x));

        // intersect sprite with ray cast
        glm::dmat2 mat(
            diff.x, diff.y,
            -endDir.x, -endDir.y
        );
        double det = glm::determinant(mat);
        if(glm::abs(det) >= EPS) {
            glm::dvec2 res = glm::inverse(mat) * midpoint;
            double t = res.x, s = res.y;
            if(0.0 <= t && t <= 1.0 && -1.0 <= s && s <= 1.0) {
                if(!hasHit || t < hitT) {
                    hasHit = true;
                    hitT = t;
                    hitSprite = actor;
                }
            }
        }
    }
    if(!hasHit) {
        return;
    }
    if(auto enemy = std::dynamic_pointer_cast<EnemyActor>(hitSprite)) {
        double dist = glm::length(player->pos - enemy->pos);
        if(player->slot == GUN_SLOT && dist <= GUN_RANGE) {
            enemy->health -= GUN_DAMAGE;
        } else if(player->slot == KNIFE_SLOT && dist <= KNIFE_RANGE) {
            enemy->health -= KNIFE_DAMAGE;
        }
    }
}

void Game::launch() {
    setup();
    loadMap1();

    size_t timer = SDL_GetTicks();
    double FPS = 0.0;

    // game loop
    while(player->health > 0) {
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //renderMinimap();
        
        renderMap();
        renderActors();
        renderPlayer();

        //renderMinimap();
        renderDebug(FPS);
        

        SDL_RenderPresent(renderer);
    }

    cleanup();
}
