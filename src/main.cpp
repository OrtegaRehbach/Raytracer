#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <sstream>
#include <glm/glm.hpp>

#include "globals.h"

SDL_Window* window;
SDL_Renderer* renderer;
const Uint8* KeyboardState;
double deltaTime;
bool running;

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("RAYTRACER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.width, screen.height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void drawPoint(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    if (rayDirection.x < 0)
        return C_GREEN;
    return C_BLUE;
}

void render() {
    for (int y = 0; y < screen.height; y++) {
        for (int x = 0; x < screen.width; x++) {
            float screenX =   (2.0f * x) / screen.width - 1.0f;
            float screenY = - (2.0f * y) / screen.height - 1.0f;
            screenX *= screen.aspectRatio;

            glm::vec3 rayDirection = glm::normalize(glm::vec3(screenX, screenY, -1.0f));
            Color pixelColor = castRay(glm::vec3(0, 0, 0), rayDirection);

            drawPoint(glm::vec2(x, y), pixelColor);
        }
    }
}

int main() {
    init();
    
    uint64_t perfFrequency = SDL_GetPerformanceFrequency();
    uint64_t frameStart = SDL_GetPerformanceCounter();

    SDL_Event event;
    running = true;
    while (running) {
        clear();
        KeyboardState = SDL_GetKeyboardState(nullptr);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;
                }
            }
        }

        render();

        SDL_RenderPresent(renderer);

        // Delta time (in seconds)

        uint64_t frameEnd = SDL_GetPerformanceCounter();
        deltaTime = (double)((frameEnd - frameStart) / (double)perfFrequency);
        
        // FPS counter

        float fps = 1 / deltaTime;
        std::ostringstream titleStream;
        if(deltaTime > 0) {
            titleStream << "FPS: " << static_cast<int>(fps);
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
        frameStart = frameEnd;
    }

    quit();
    exit(0);
}
