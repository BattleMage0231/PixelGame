#include "map.h"

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

bool GameMap::isOpaque(double x, double y) {
    if(outOfBounds(x, y)) return true;
    size_t ix = static_cast<size_t>(x);
    size_t iy = static_cast<size_t>(y);
    return (mapData[ix + height * iy] != Tile::EMPTY);
}

bool GameMap::isSolid(double x, double y) {
    if(outOfBounds(x, y)) return true;
    size_t ix = static_cast<size_t>(x);
    size_t iy = static_cast<size_t>(y);
    return (mapData[ix + height * iy] != Tile::EMPTY);
}

SDL_Rect GameMap::getTexture(double x, double y) {
    if(outOfBounds(x, y)) return SDL_Rect { 0, 0, 0, 0 };
    size_t ix = static_cast<size_t>(x);
    size_t iy = static_cast<size_t>(y);
    return TILE_TEXTURES[mapData[ix + height * iy]];
}

SDL_Rect GameMap::getColumn(double x, double y, double amt) {
    SDL_Rect texture = getTexture(x, y);
    if(texture.w == 0) return texture;
    size_t col = static_cast<size_t>(texture.w * amt);
    texture.x += col;
    texture.w = 1;
    return texture;
}
