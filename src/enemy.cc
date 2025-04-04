#include <queue>
#include <utility>
#include <glm/geometric.hpp>

#include "data.h"
#include "enemy.h"

EnemyActor::EnemyActor(glm::dvec2 pos, glm::dvec2 dir, std::shared_ptr<GamePlayer> player, GameMap& map) 
    : pos(pos), dir(dir), vel(0.0), player(player), map(map), health(ENEMY_MAX_HEALTH) {}

EnemyActor::~EnemyActor() {}

glm::dvec2 EnemyActor::pathfind() {
    double deltaX = glm::abs(pos.x - glm::floor(pos.x) - 0.5);
    double deltaY = glm::abs(pos.y - glm::floor(pos.y) - 0.5);
    if(glm::max(deltaX, deltaY) > ENEMY_BLOCK_TOLERANCE) {
        return glm::dvec2(glm::floor(pos.x) + 0.5, glm::floor(pos.y) + 0.5) - pos;
    }
    size_t width = map.getWidth();
    size_t height = map.getHeight();
    bool** visited = new bool*[width];
    for(size_t i = 0; i < width; ++i) {
        visited[i] = new bool[height];
        for(size_t j = 0; j < height; ++j) visited[i][j] = false;
    }
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
            for(size_t i = 0; i < width; ++i) delete [] visited[i];
            delete [] visited;
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
    for(size_t i = 0; i < width; ++i) delete [] visited[i];
    delete [] visited;
    return glm::dvec2(0.0, 0.0);
}

void EnemyActor::update(size_t timeDelta) {
    if(health <= 0) {
        return;
    }
    double dist = glm::length(player->pos - pos);
    if(dist < ENEMY_CLOSE_DIST || (shooting && shootAnim == 1)) {
        if(shooting) {
            shootTimer += timeDelta;
            if(shootAnim == 0 && shootTimer >= ENEMY_SHOOT_TIME) {
                shootAnim = 1;
                shootTimer = 0;
                player->health -= ENEMY_SHOOT_DAMAGE;
            } else if(shootAnim == 1 && shootTimer >= ENEMY_SHOOT_FRAME_TIME) {
                shootAnim = 0;
                shootTimer = 0;
            }
        } else {
            running = false;
            shooting = true;
            shootTimer = 0;
            shootAnim = 0;
            vel = 0.0;
        }
    } else if(dist > ENEMY_FAR_DIST) {
        vel = 0.0;
        running = false;
        shooting = false;
    } else {
        if(running) {
            runTimer += timeDelta;
            if(runTimer >= ENEMY_RUN_FRAME_TIME) {
                runAnim = (runAnim + 1) % ENEMY_RUN_FRAME_CNT;
                runTimer = 0;
            }
        } else {
            shooting = false;
            running = true;
            runTimer = 0;
            runAnim = 0;
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
}

glm::dvec2 EnemyActor::getPosition() {
    if(health <= 0) {
        return glm::dvec2(INF, INF);
    } else {
        return pos;
    }
}

SDL_Rect EnemyActor::getTexture() {
    if(health <= 0) {
        return SDL_Rect { 0, 0, 0, 0 };
    } else if(running) {
        return SDL_Rect { static_cast<int>(64 * runAnim), 128, 64, 64 };
    } else if(shooting) {
        return SDL_Rect { 256 + static_cast<int>(64 * shootAnim), 128, 64, 64 };
    } else {
        return SDL_Rect { 384, 128, 64, 64 };
    }
}
