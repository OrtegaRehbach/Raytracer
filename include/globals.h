#include <SDL2/SDL.h>

#include "color.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern const Uint8* KeyboardState;
extern double deltaTime;
extern bool running;

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