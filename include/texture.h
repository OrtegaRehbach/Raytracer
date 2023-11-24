#pragma once

#include <string>
#include <SDL2/SDL_image.h>

#include "color.h"

class Texture {
public:
    Texture(const std::string& filePath);
    ~Texture();
    Color sample(float u, float v) const;

private:
    SDL_Surface* textureSurface; // SDL surface to store the loaded texture
};
