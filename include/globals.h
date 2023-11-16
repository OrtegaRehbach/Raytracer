#include <SDL2/SDL.h>

#include "color.h"

SDL_Window *window;
SDL_Renderer *renderer;
const Uint8* KeyboardState;
double deltaTime;
bool running;

struct Screen {
    const int width;
    const int height;
	int centerX;
	int centerY;

	Screen(int w, int h) : width(w), height(h) {
		centerX = width / 2;
		centerY = height / 2;
	}
} screen = {900, 900};