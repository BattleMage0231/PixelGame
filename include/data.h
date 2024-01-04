#ifndef PIXELGAME_DATA_H_
#define PIXELGAME_DATA_H_

#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <glm/vec2.hpp>

#include "actor.h"
#include "enemy.h"
#include "static.h"

constexpr size_t WIN_WIDTH = 1024;
constexpr size_t WIN_HEIGHT = 800;

constexpr size_t FPS_MAX = 120;

const std::string TEXTURES_PATH = "../assets/wolftexturesobj.gif";
const std::string FONT_PATH = "../assets/RubikLines-Regular.ttf";

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double EPS = 1e-6;

constexpr size_t ITEM_FRAME_TIME = 100;
constexpr size_t ITEM_FRAME_CNT = 5;
constexpr size_t ITEM_SIZE_MULT = 16;

constexpr size_t GUN_SLOT = 1;
constexpr size_t KNIFE_SLOT = 2;

constexpr size_t CROSSHAIR_LENGTH = 10;

constexpr double ENEMY_BLOCK_TOLERANCE = 0.5;
constexpr double ENEMY_CLOSE_DIST = 1.0;
constexpr double ENEMY_FAR_DIST = 7.0;
const std::vector<glm::dvec2> POS_DELTA = { 
    { 1.0, 0.0 }, { -1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, -1.0}, 
    { 1.0, 1.0 }, { -1.0, 1.0 }, { 1.0, -1.0 }, { -1.0, -1.0}
};

constexpr size_t MAP_1_WIDTH = 16;
constexpr size_t MAP_1_HEIGHT = 20;
const std::string MAP_1_PATH = "../assets/map1";

#endif
