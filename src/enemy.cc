#include <glm/geometric.hpp>

#include "enemy.h"

EnemyActor::EnemyActor(glm::dvec2 pos, glm::dvec2 dir, SDL_Rect texture, std::shared_ptr<GamePlayer> player, GameMap& map) : pos(pos), dir(dir), texture(texture), vel(0.0), player(player), map(map) {}

EnemyActor::~EnemyActor() {}

void EnemyActor::update(size_t timeDelta) {
    dir = player->pos - pos;
    double dist = glm::length(dir);
    if(2.0 < dist && dist < 6.0) {
        vel = 0.001;
    } else {
        vel = 0.0;
    }
    // apply velocity
    glm::dvec2 newPos = pos + timeDelta * vel * glm::normalize(dir);
    if(map.isSolid(newPos.x, pos.y)) {
        newPos.x = pos.x;
    } 
    if(map.isSolid(pos.x, newPos.y)) {
        newPos.y = pos.y;
    }
    pos = newPos;
}

glm::dvec2 EnemyActor::getPosition() {
    return pos;
}

SDL_Rect EnemyActor::getTexture() {
    return texture;
}
