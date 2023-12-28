#ifndef PIXELGAME_MAP_H_
#define PIXELGAME_MAP_H_

#include <vector>
#include <glm/vec2.hpp>
#include <SDL.h>

constexpr size_t MAP_1_WIDTH = 8;
constexpr size_t MAP_1_HEIGHT = 8;

const std::vector<size_t> MAP_1 = {
    1, 1, 1, 1, 2, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 2, 0, 1,
    1, 1, 0, 0, 0, 2, 0, 1,
    1, 0, 0, 0, 0, 2, 0, 1,
    2, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 2, 1, 1, 1, 1, 1
};

class GameMap {
    size_t width;
    size_t height;

    std::vector<size_t> mapData;
    
    bool outOfBounds(double x, double y);

    public:
        GameMap();
        ~GameMap();

        size_t getWidth();
        size_t getHeight();

        void loadMap(std::vector<size_t> data, size_t w, size_t h);

        bool isOpaque(double x, double y);
        bool isSolid(double x, double y);
        SDL_Rect getTexture(double x, double y);
        SDL_Rect getColumn(double x, double y, double amt);

        glm::dvec2 castRay(glm::dvec2 r0, glm::dvec2 u);
};

#endif
