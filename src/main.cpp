#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <sstream>
#include <glm/glm.hpp>
#include <vector>

#include "globals.h"
#include "ray.h"
#include "object.h"
#include "sphere.h"
#include "box.h"
#include "camera.h"

SDL_Window* window;
SDL_Renderer* renderer;
const Uint8* KeyboardState;
double deltaTime;
bool running;
bool performanceMode = false;
Light light = {glm::vec3(-1.0f, 0.0f, 5.0f), 1.0f, C_WHITE};
Camera camera(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 10.0f);
const Color BACKGROUND_COLOR = C_CYAN;
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;

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
    // objects.push_back(new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, MAT_RUBBER_RED));
    // objects.push_back(new Sphere(glm::vec3(-1.0f, 1.0f, -4.0f), 1.0f, MAT_IVORY));
    objects.push_back(new Sphere(glm::vec3(1.0f, 1.0f, -4.0f), 1.0f, MAT_MIRROR));
    objects.push_back(new Sphere(glm::vec3(-2.0f, 0.0f, -2.0f), 1.0f, MAT_GLASS));
    objects.push_back(new Box(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAT_RUBBER_RED));
    objects.push_back(new Box(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), MAT_RUBBER_GREEN));
    // objects.push_back(new Box(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAT_RUBBER));
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.distance > 0) {
                float shadowRatio = shadowIntersect.distance / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const int recursion = 0) {
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
    if (!intersect.isIntersecting || recursion == MAX_RECURSION)
        return BACKGROUND_COLOR;
    
    Material mat = hitObject->material;

    glm::vec3 lightDirection = glm::normalize(light.position - intersect.point);
    glm::vec3 viewDirection = glm::normalize(rayOrigin - intersect.point);
    glm::vec3 reflectDirection = glm::reflect(-lightDirection, intersect.normal);

    float diffuseLightIntensity = glm::max(0.0f, glm::dot(intersect.normal, lightDirection));
    float specularReflection = glm::max(0.0f, glm::dot(viewDirection, reflectDirection));
    float specularLightIntensity = std::pow(specularReflection, mat.specularCoefficient);
    float shadowIntensity = castShadow(intersect.point, lightDirection, hitObject);

    Color reflectedColor(0, 0, 0);
    if (mat.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        reflectedColor = castRay(origin, reflectDirection, recursion + 1); 
    }

    Color refractedColor(0, 0, 0);
    if (mat.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDir = glm::refract(rayDirection, intersect.normal, mat.refractionIndex);
        refractedColor = castRay(origin, refractDir, recursion + 1); 
    }

    Color diffuseLight = mat.diffuse * light.intensity * diffuseLightIntensity * mat.albedo;
    Color specularLight = light.color * light.intensity * specularLightIntensity * mat.specularAlbedo;
    
    Color color;
    if (shadowIntensity == 1)   // Only add specular highlight on areas not in shadow
        color = (diffuseLight + specularLight) * shadowIntensity * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    else
        color = diffuseLight * shadowIntensity * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    return color;

}

Color castRay(const Ray &ray, const int recursion = 0) {
    return castRay(ray.origin, ray.direction);
}

void render() {
    float fov = M_PI_2;
    for (int y = 0; y < screen.height; y++) {
        for (int x = 0; x < screen.width; x++) {
            if (performanceMode) {
                float random_value = (float)std::rand() / (float)RAND_MAX;
                if (random_value < 0.75f) continue;
            }

            float screenX =  ((2.0f * (x + 0.5f)) / screen.width  - 1.0f) * tan(fov / 2.0f) * screen.aspectRatio;
            float screenY = (-(2.0f * (y + 0.5f)) / screen.height + 1.0f) * tan(fov / 2.0f);

            glm::vec3 cameraDirection = glm::normalize(camera.target - camera.position);
            
            glm::vec3 cameraX = glm::cross(cameraDirection, camera.up);
            glm::vec3 cameraY = glm::cross(cameraX, cameraDirection);
            glm::vec3 rayDirection = glm::normalize(cameraDirection + cameraX * screenX + cameraY * screenY);
            Ray ray = Ray(camera.position, rayDirection);
            Color pixelColor = castRay(ray);
            // Color pixelColor = castRay(camera.position, rayDirection);

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
                if (event.key.keysym.sym == SDLK_p)
                    performanceMode = !performanceMode;
            }
        }
        if (KeyboardState[SDL_SCANCODE_UP])
            camera.move(1.0f);
        if (KeyboardState[SDL_SCANCODE_DOWN])
            camera.move(-1.0f);
        if (KeyboardState[SDL_SCANCODE_LEFT])
            camera.rotate(-1.0f, 0.0f);
        if (KeyboardState[SDL_SCANCODE_RIGHT])
            camera.rotate(1.0f, 0.0f);
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
