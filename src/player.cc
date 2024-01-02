#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "data.h"
#include "player.h"

GamePlayer::~GamePlayer() {}

GamePlayer::GamePlayer(glm::dvec2 pos, glm::dvec2 dir, glm::dvec2 plane, GameMap& map) 
    : pos(pos), camDir(dir), camPlane(plane), vel(0.0), angVel(0.0), map(map), slot(0), animStep(0) {}

void GamePlayer::update(size_t timeDelta) {
    // apply rotation
    camDir = glm::rotate(camDir, timeDelta * angVel);
    camPlane = glm::rotate(camPlane, timeDelta * angVel);
    // apply velocity
    glm::dvec2 newPos = pos + timeDelta * vel * glm::normalize(camDir);
    if(map.isSolid(newPos.x, pos.y)) {
        newPos.x = pos.x;
    } 
    if(map.isSolid(pos.x, newPos.y)) {
        newPos.y = pos.y;
    }
    pos = newPos;
    // update texture
    if(1 <= animStep && animStep < ITEM_FRAME_CNT) {
        animTimer += timeDelta;
        if(animTimer >= ITEM_FRAME_TIME) {
            animStep = (animStep + 1) % ITEM_FRAME_CNT;
            animTimer = 0;
        }
    }
}

glm::dvec2 GamePlayer::getPosition() {
    return pos;
};

SDL_Rect GamePlayer::getTexture() {
    if(slot == GUN_SLOT) {
        return SDL_Rect { static_cast<int>(64 * animStep), 64, 64, 64 };
    } else if(slot == KNIFE_SLOT) {
        return SDL_Rect { static_cast<int>(320 + 64 * animStep), 64, 64, 64 };
    } else {
        return SDL_Rect { 0, 0, 0, 0 };
    }
}
