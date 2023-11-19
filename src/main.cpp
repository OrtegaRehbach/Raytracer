#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <sstream>
#include <glm/glm.hpp>
#include <vector>

#include "globals.h"
#include "object.h"
#include "sphere.h"

SDL_Window* window;
SDL_Renderer* renderer;
const Uint8* KeyboardState;
double deltaTime;
bool running;
Light light = {glm::vec3(-4.0f, 3.0f, 2.0f), 1.5f};

std::vector<Object*> objects;

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("RAYTRACER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.width, screen.height, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
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

void setUpObjects() {
    objects.push_back(new Sphere(glm::vec3(0.0f, 3.0f, -8.0f), 1.2f, MAT_RUBBER));
    objects.push_back(new Sphere(glm::vec3(-1.0f, 0.0f, -4.0f), 1.0f, MAT_IVORY));
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) {
    float zBuffer = 9999.0f;
    Object* hitObject = nullptr;
    Intersect intersect;
    for (const auto& object : objects) {
        Intersect i = object->rayIntersect(rayOrigin, rayDirection);
        if (i.isIntersecting && i.distance < zBuffer) {
            zBuffer = i.distance;
            hitObject = object;
            intersect = i;
        }
    }
    if (!intersect.isIntersecting)
        return C_CYAN;
    
    glm::vec3 lightDirection = glm::normalize(light.position - intersect.point);

    float diffuseLightIntensity = glm::max(0.0f, glm::dot(intersect.normal, lightDirection));

    Material mat = hitObject->material;
    Color diffuseLight = mat.diffuse * light.intensity * diffuseLightIntensity;
    Color color = diffuseLight;
    return color;

}

void render() {
    float fov = M_PI_2;
    for (int y = 0; y < screen.height; y++) {
        for (int x = 0; x < screen.width; x++) {
            float screenX =  ((2.0f * (x + 0.5f)) / screen.width  - 1.0f) * tan(fov / 2.0f) * screen.aspectRatio;
            float screenY = (-(2.0f * (y + 0.5f)) / screen.height + 1.0f) * tan(fov / 2.0f);

            glm::vec3 rayDirection = glm::normalize(glm::vec3(screenX, screenY, -1.0f));
            Color pixelColor = castRay(glm::vec3(0.0f, 0.0f, 0.0f), rayDirection);

            drawPoint(glm::vec2(x, y), pixelColor);
        }
    }
}

int main() {
    init();
    setUpObjects();
    
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
