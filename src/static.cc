#include "static.h"

StaticActor::StaticActor(glm::dvec2 pos, SDL_Rect texture) : pos(pos), texture(texture) {}

StaticActor::~StaticActor() {}

void StaticActor::update(size_t timeDelta) {}
    
glm::dvec2 StaticActor::getPosition() {
    return pos;
}

SDL_Rect StaticActor::getTexture() {
    return texture;
}
