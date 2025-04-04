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

constexpr double RAYCAST_DELTA = 0.01;

constexpr size_t FPS_MAX = 200;

#ifdef __EMSCRIPTEN__
const std::string TEXTURES_PATH = "/assets/textures.gif";
const std::string FONT_PATH = "/assets/RubikLines-Regular.ttf";
const std::string MAP_1_PATH = "/assets/map1";
#else
const std::string TEXTURES_PATH = "../assets/textures.gif";
const std::string FONT_PATH = "../assets/RubikLines-Regular.ttf";
const std::string MAP_1_PATH = "../assets/map1";
#endif

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double EPS = 1e-6;

constexpr size_t ITEM_FRAME_TIME = 100;
constexpr size_t ITEM_FRAME_CNT = 5;
constexpr size_t ITEM_SIZE_MULT = 16;

constexpr size_t KNIFE_SLOT = 1;
constexpr int KNIFE_DAMAGE = 5;
constexpr double KNIFE_RANGE = 1.0;

constexpr size_t GUN_SLOT = 2;
constexpr int GUN_DAMAGE = 20;
constexpr double GUN_RANGE = 7.0;

constexpr size_t CROSSHAIR_LENGTH = 10;

constexpr int PLAYER_MAX_HEALTH = 100;

constexpr int ENEMY_MAX_HEALTH = 30;
constexpr double ENEMY_BLOCK_TOLERANCE = 0.5;
constexpr double ENEMY_CLOSE_DIST = 3.0;
constexpr double ENEMY_FAR_DIST = 8.0;
constexpr size_t ENEMY_RUN_FRAME_TIME = 200;
constexpr size_t ENEMY_RUN_FRAME_CNT = 4;
constexpr size_t ENEMY_SHOOT_FRAME_TIME = 100;
constexpr size_t ENEMY_SHOOT_TIME = 1500;
constexpr size_t ENEMY_SHOOT_DAMAGE = 20;

const std::vector<glm::dvec2> POS_DELTA = { 
    { 1.0, 0.0 }, { -1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, -1.0}, 
    { 1.0, 1.0 }, { -1.0, 1.0 }, { 1.0, -1.0 }, { -1.0, -1.0}
};

constexpr size_t MAP_1_WIDTH = 16;
constexpr size_t MAP_1_HEIGHT = 20;

#endif
