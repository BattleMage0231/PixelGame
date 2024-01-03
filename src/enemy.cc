#include <queue>
#include <utility>
#include <glm/geometric.hpp>

#include "data.h"
#include "enemy.h"

EnemyActor::EnemyActor(glm::dvec2 pos, glm::dvec2 dir, SDL_Rect texture, std::shared_ptr<GamePlayer> player, GameMap& map) 
    : pos(pos), dir(dir), texture(texture), vel(0.0), player(player), map(map) {}

EnemyActor::~EnemyActor() {}

glm::dvec2 EnemyActor::pathfind() {
    double deltaX = glm::abs(pos.x - glm::floor(pos.x) - 0.5);
    double deltaY = glm::abs(pos.y - glm::floor(pos.y) - 0.5);
    if(glm::max(deltaX, deltaY) > ENEMY_BLOCK_TOLERANCE) {
        return glm::dvec2(glm::floor(pos.x) + 0.5, glm::floor(pos.y) + 0.5) - pos;
    }
    size_t width = map.getWidth();
    size_t height = map.getHeight();
    bool visited[width][height];
    visited[static_cast<int>(pos.x)][static_cast<int>(pos.y)] = true;
    std::queue<std::pair<glm::dvec2, glm::dvec2>> toVisit;
    for(glm::dvec2 delta : POS_DELTA) {
        glm::dvec2 newPos = pos + delta;
        if(!map.isSolid(pos.x, newPos.y) && !map.isSolid(newPos.x, pos.y) && !map.isSolid(newPos.x, newPos.y)) {
            visited[static_cast<int>(newPos.x)][static_cast<int>(newPos.y)] = true;
            toVisit.emplace(delta, newPos);
        }
    } 
    while(!toVisit.empty()) {
        auto entry = toVisit.front();
        glm::dvec2 pos = entry.second;
        toVisit.pop();
        if(glm::length(pos - player->pos) < ENEMY_CLOSE_DIST) {
            return entry.first;
        }
        for(glm::dvec2 delta : POS_DELTA) {
            glm::dvec2 newPos = pos + delta;
            int mapX = static_cast<int>(newPos.x);
            int mapY = static_cast<int>(newPos.y);
            if(!map.isSolid(pos.x, newPos.y) && !map.isSolid(newPos.x, pos.y) && !map.isSolid(newPos.x, newPos.y) && !visited[mapX][mapY]) {
                visited[mapX][mapY] = true;
                toVisit.emplace(entry.first, newPos);
            }
        }
    }
    return glm::dvec2(0.0, 0.0);
}

void EnemyActor::update(size_t timeDelta) {
    double dist = glm::length(player->pos - pos);
    if(dist < ENEMY_CLOSE_DIST || dist > ENEMY_FAR_DIST) {
        vel = 0.0;
        return;
    }
    dir = pathfind();
    vel = 0.001;
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
