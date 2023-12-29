#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "player.h"

GamePlayer::~GamePlayer() {}

GamePlayer::GamePlayer(glm::dvec2 pos, glm::dvec2 dir, glm::dvec2 plane, GameMap& map) 
    : pos(pos), camDir(dir), camPlane(plane), vel(0.0), angVel(0.0), map(map) {}

void GamePlayer::update(size_t timeDelta) {
    // apply rotation
    camDir = glm::rotate(camDir, angVel);
    camPlane = glm::rotate(camPlane, angVel);
    // apply velocity
    glm::dvec2 newPos = pos + timeDelta * vel * glm::normalize(camDir);
    if(map.isSolid(newPos.x, pos.y)) {
        newPos.y = pos.y;
    } 
    if(map.isSolid(pos.x, newPos.y)) {
        newPos.x = pos.x;
    }
    pos = newPos;
}

glm::dvec2 GamePlayer::getPosition() {
    return pos;
};

SDL_Rect GamePlayer::getTexture() {
    return SDL_Rect { 0, 0, 0, 0 };
}
