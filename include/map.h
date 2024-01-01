#ifndef PIXELGAME_MAP_H_
#define PIXELGAME_MAP_H_

#include <vector>
#include <glm/vec2.hpp>
#include <SDL.h>

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

        glm::ivec2 getCoords(double x, double y);
        bool isOpaque(double x, double y);
        bool isSolid(double x, double y);
        SDL_Rect getTexture(double x, double y);
        SDL_Rect getColumn(double x, double y, double amt);

        glm::dvec2 castRay(glm::dvec2 r0, glm::dvec2 u);
};

#endif
