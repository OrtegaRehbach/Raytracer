#include "texture.h"

Texture::Texture(const std::string& filePath) : textureSurface(nullptr) {
    // Load texture using SDL_image
    textureSurface = IMG_Load(filePath.c_str());
    if (!textureSurface) {
        // Handle error, e.g., print an error message and return or throw an exception
    }

    // Optional: Convert the surface to the desired format (e.g., RGBA)
    textureSurface = SDL_ConvertSurfaceFormat(textureSurface, SDL_PIXELFORMAT_RGBA32, 0);
}

Texture::~Texture() {
    if (textureSurface) {
        SDL_FreeSurface(textureSurface);
    }
}

Color Texture::sample(float u, float v) const {
    // Ensure the surface is valid
    if (!textureSurface) {
        // Handle error, return a default color or throw an exception
        return Color(255, 0, 255); // Magenta color as an error indicator
    }

    // Map UV coordinates to pixel coordinates
    int x = static_cast<int>(u * (textureSurface->w - 1));
    int y = static_cast<int>(v * (textureSurface->h - 1));

    if (x >= 0 && x < textureSurface->w && y >= 0 && y < textureSurface->h) {
        // Get the pixel at the specified coordinates
        Uint32 pixel = *reinterpret_cast<Uint32*>(reinterpret_cast<Uint8*>(textureSurface->pixels) + y * textureSurface->pitch + x * textureSurface->format->BytesPerPixel);

        // Extract color components
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, textureSurface->format, &r, &g, &b, &a);

        // Return the sampled color
        return Color(r, g, b, a);
    } else {
        // Handle out-of-bounds access, e.g., return a default color or throw an exception
        return Color(255, 0, 255); // Magenta color as an error indicator
    }
}