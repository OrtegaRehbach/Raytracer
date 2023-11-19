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

const Material MAT_RUBBER = {Color(80, 0, 0), 0.6f};
const Material MAT_IVORY = {Color(100, 100, 80), 0.9f};