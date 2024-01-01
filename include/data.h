#ifndef PIXELGAME_DATA_H_
#define PIXELGAME_DATA_H_

#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "actor.h"
#include "static.h"

constexpr size_t WIN_WIDTH = 1024;
constexpr size_t WIN_HEIGHT = 800;

constexpr size_t FPS_MAX = 120;

const std::string TEXTURES_PATH = "../assets/wolftexturesobj.gif";
const std::string FONT_PATH = "../assets/RubikLines-Regular.ttf";

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double EPS = 1e-6;

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

inline void MAP_1_LOAD_ACTORS(std::vector<std::shared_ptr<GameActor>>& actors) {
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(4.0, 2.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(3.5, 6.5), SDL_Rect { 512, 0, 64, 64 }));
    actors.push_back(std::make_shared<StaticActor>(glm::dvec2(4.2, 3.7), SDL_Rect { 512, 0, 64, 64 }));
}

#endif
