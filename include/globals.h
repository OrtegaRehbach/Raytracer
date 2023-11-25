#include <SDL2/SDL.h>

#include "color.h"
#include "material.h"
#include "light.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const Uint8* KeyboardState;
extern double deltaTime;
extern bool running;
extern Light light;

struct Screen {
    const int width;
    const int height;
	int halfWidth;
	int halfHeight;
	float aspectRatio;

	Screen(int w, int h) : width(w), height(h) {
		halfWidth = width / 2;
		halfHeight = height / 2;
		aspectRatio = (float)width / (float)height;
	}
} screen = {800, 600};

const Material MAT_RUBBER_RED = {Color(80, 0, 0), 0.9f, 0.1f, 10.0f, 0.0f, 0.0f, 0.0f};
const Material MAT_RUBBER_GREEN = {Color(0, 80, 0), 0.9f, 0.1f, 10.0f, 0.0f, 0.0f, 0.0f, new Texture("../assets/grass.jpg")};
const Material MAT_RUBBER_BLUE = {Color(0, 0, 80), 0.9f, 0.1f, 10.0f, 0.0f, 0.0f, 0.0f};
const Material MAT_IVORY = {Color(100, 100, 80), 0.5f, 0.5f, 50.0f, 0.4f, 0.0f};
const Material MAT_MIRROR = {Color(255, 255, 255), 0.0f, 10.0f, 1425.0f, 0.9f, 0.0f};
const Material MAT_GLASS = {Color(255, 255, 255), 0.0f, 10.0f, 1425.0f, 0.2f, 1.0f};

// New mats
Material MAT_GRASS = {C_GREEN, 0.8f, 0.5f, 32.0f, 0.1f, 0.0f, 1.5f, nullptr};
Material MAT_DIRT = {C_SADDLE_BROWN, 0.8f, 0.5f, 32.0f, 0.1f, 0.0f, 1.5f, nullptr};
Material MAT_STONE = {C_GRAY, 0.8f, 0.5f, 32.0f, 0.1f, 0.0f, 1.5f, nullptr};
Material MAT_WOOD = {C_BROWN, 0.8f, 0.5f, 32.0f, 0.1f, 0.0f, 1.5f, nullptr};
Material MAT_LEAVES = {C_GREEN, 0.8f, 0.5f, 32.0f, 0.1f, 0.0f, 1.5f, nullptr};
Material MAT_WATER = {
    Color(0, 127, 255),     // Diffuse color (light blue)
    0.8f,                   // Albedo
    0.8f,                   // Specular albedo
    50.0f,                  // Specular coefficient
    0.1f,                   // Reflectivity
    0.9f,                   // Transparency
    1.33f,                  // Refraction index (typical value for water)
    nullptr                 // No texture for now, you can add a water texture if desired
};