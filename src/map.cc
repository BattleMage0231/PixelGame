#include <glm/geometric.hpp>

#include "data.h"
#include "map.h"

constexpr double DELTA_T = 0.02;

enum Tile { EMPTY, WALL1, WALL2 };

const std::vector<SDL_Rect> TILE_TEXTURES = {
    SDL_Rect { 0, 0, 0, 0 },
    SDL_Rect { 0, 0, 64, 64 },
    SDL_Rect { 64, 0, 64, 64 }
};

bool GameMap::outOfBounds(double x, double y) {
    return (x < 0.0 || x >= width || y < 0.0 || y >= height);
}

GameMap::GameMap() {}

GameMap::~GameMap() {}

size_t GameMap::getWidth() {
    return width;
}

size_t GameMap::getHeight() {
    return height;
}

void GameMap::loadMap(std::vector<size_t> data, size_t w, size_t h) {
    width = w;
    height = h;
    mapData = data;
}

glm::ivec2 GameMap::getCoords(double x, double y) {
    return glm::ivec2 {
        static_cast<int>(x), static_cast<int>(y)
    };
}

bool GameMap::isOpaque(double x, double y) {
    if(outOfBounds(x, y)) return true;
    glm::uvec2 co = getCoords(x, y);
    return (mapData[co.x + height * co.y] != Tile::EMPTY);
}

bool GameMap::isSolid(double x, double y) {
    if(outOfBounds(x, y)) return true;
    glm::uvec2 co = getCoords(x, y);
    return (mapData[co.x + height * co.y] != Tile::EMPTY);
}

SDL_Rect GameMap::getTexture(double x, double y) {
    if(outOfBounds(x, y)) return SDL_Rect { 0, 0, 0, 0 };
    glm::uvec2 co = getCoords(x, y);
    return TILE_TEXTURES[mapData[co.x + height * co.y]];
}

SDL_Rect GameMap::getColumn(double x, double y, double amt) {
    SDL_Rect texture = getTexture(x, y);
    if(texture.w == 0) return texture;
    size_t col = static_cast<size_t>(texture.w * amt);
    texture.x += col;
    texture.w = 1;
    return texture;
}

glm::dvec2 GameMap::castRay(glm::dvec2 r0, glm::dvec2 u) {
    u = glm::normalize(u);
    glm::ivec2 ri0 = getCoords(r0.x, r0.y);
    glm::dvec2 r1 = r0;
    glm::ivec2 ri1 = ri0;
    while(!isOpaque(r1.x, r1.y)) {
        if(ri0.x != ri1.x && ri0.y != ri1.y) {
            double crossX = glm::max(ri0.x, ri1.x);
            double crossY = glm::max(ri0.y, ri1.y);
            double tX = (crossX - r0.x) / u.x;
            double tY = (crossY - r0.y) / u.y;
            if(tY < tX) {
                if(mapData[ri0.x + height * ri1.y] != Tile::EMPTY) {
                    break;
                }
            } else {
                if(mapData[ri1.x + height * ri0.y] != Tile::EMPTY) {
                    break;
                }
            }
        }
        r0 = r1;
        ri0 = ri1;
        r1 = r0 + u;
        ri1 = getCoords(r1.x, r1.y);
    }
    while(!isOpaque(r0.x, r0.y)) {
        r0 += DELTA_T * u;
    }
    return r0;
}
