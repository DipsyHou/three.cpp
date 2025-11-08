#ifndef VIEWPOINT_HPP
#define VIEWPOINT_HPP

#include "vector3.hpp"
#include "space.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

class Viewpoint
{
public:
    Viewpoint()
        : position(0.0, 0.0, 0.0), yaw(0.0), pitch(0.0), fov(60.0), 
          screenWidth(800), screenHeight(600),
          window(nullptr), renderer(nullptr), texture(nullptr)
    {
        pixelBuffer.resize(screenWidth * screenHeight, 0);
    }

    Viewpoint(const Vector3& pos, double yaw, double pitch, double fov, Space* space, int screenWidth = 800, int screenHeight = 600)
        : position(pos), yaw(yaw), pitch(pitch), fov(fov), 
          screenWidth(screenWidth), screenHeight(screenHeight),
          space(space),
          window(nullptr), renderer(nullptr), texture(nullptr)
    {
        pixelBuffer.resize(screenWidth * screenHeight, 0);
    }

    ~Viewpoint()
    {
        cleanup();
    }

    bool initSDL()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            return false;
        }

        window = SDL_CreateWindow(
            "Three.cpp",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            screenWidth, screenHeight,
            SDL_WINDOW_SHOWN
        );

        if (!window) {
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            return false;
        }

        texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            screenWidth, screenHeight
        );

        if (!texture) {
            return false;
        }

        return true;
    }

    void cleanup()
    {
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Getters
    Vector3 getPosition() const { return position; }
    double getYaw() const { return yaw; }
    double getPitch() const { return pitch; }
    double getFOV() const { return fov; }
    Space* getSpace() const { return space; }
    int getScreenWidth() const { return screenWidth; }
    int getScreenHeight() const { return screenHeight; }

    // Setters
    void setPosition(const Vector3& pos) { position = pos; }
    void setYaw(double y) { yaw = y; }
    void setPitch(double p) { pitch = p; }
    void setFOV(double f) { fov = f; }
    void setScreenWidth(int w) { screenWidth = w; }
    void setScreenHeight(int h) { screenHeight = h; }

    struct CastRayResult
    {
        bool hit;
        double distance;
        const Plane* hitPlane;

        CastRayResult() : hit(false), distance(std::numeric_limits<double>::infinity()), hitPlane(nullptr) {}
    };

    // Cast a ray given a direction vector
    CastRayResult castRayDir(const Vector3& dir) const
    {
        CastRayResult result;

        if (space == nullptr) return result;

        const double EPS = 1e-9;
        double closestT = std::numeric_limits<double>::infinity();

        const auto& planes = space->getPlanes();
        for (const auto& plane : planes) {
            const Vector3& a = plane.getA();
            const Vector3& b = plane.getB();
            const Vector3& c = plane.getC();

            Vector3 edge1 = b - a;
            Vector3 edge2 = c - a;

            Vector3 h = dir.cross(edge2);
            double det = edge1.dot(h);
            
            // parallel check
            if (det > -EPS && det < EPS) continue;

            double invDet = 1.0 / det;
            Vector3 s = position - a;
            double u = invDet * s.dot(h);
            if (u < 0.0 || u > 1.0) continue;

            Vector3 q = s.cross(edge1);
            double v = invDet * dir.dot(q);
            if (v < 0.0 || u + v > 1.0) continue;

            double t = invDet * edge2.dot(q);
            if (t > EPS && t < closestT) {
                closestT = t;
                result.hit = true;
                result.distance = t;
                result.hitPlane = &plane;
            }
        }

        return result;
    }

    // Cast a ray given yaw and pitch
    CastRayResult castRay(double rayYaw, double rayPitch) const
    {
        // convert degrees to radians
        const double yawRad = rayYaw * M_PI / 180.0;
        const double pitchRad = rayPitch * M_PI / 180.0;

        // calculate direction vector
        Vector3 dir(
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        );

        return castRayDir(dir.normalize());
    }

    // Render a slice of rows (for multi-threading)
    void renderSlice(int startY, int endY, const Vector3& forward, const Vector3& right, const Vector3& up, double aspectRatio, double tanHalfFov)
    {
        const int width = screenWidth;
        const int height = screenHeight;
        
        for (int y = startY; y < endY; ++y) {
            for (int x = 0; x < width; ++x) {
                double ndcX = (2.0 * (x + 0.5) / width - 1.0) * aspectRatio * tanHalfFov;
                double ndcY = (1.0 - 2.0 * (y + 0.5) / height) * tanHalfFov;
                
                Vector3 rayDir = (forward + right * ndcX + up * ndcY).normalize();
                
                auto result = castRayDir(rayDir);
                
                if (!result.hit) continue;
                
                Vector3 normal = result.hitPlane->normal();
                
                // calculate brightness
                double brightness = std::abs(normal.dot(rayDir));
                
                double distanceFade = std::max(0.0, 1.0 - result.distance / 30.0);
                brightness *= distanceFade;
                
                uint8_t r = static_cast<uint8_t>(brightness * 200);
                uint8_t g = static_cast<uint8_t>(brightness * 150);
                uint8_t b = static_cast<uint8_t>(brightness * 100);
                uint32_t planeColor = 0xFF000000 | (r << 16) | (g << 8) | b;
                
                pixelBuffer[y * width + x] = planeColor;
            }
        }
    }

    void render()
    {
        if (space == nullptr) return;

        const int width = screenWidth;
        const int height = screenHeight;
        const int halfHeight = height / 2;

        // render ceiling and floor
        for (int y = 0; y < height; ++y) {
            uint32_t color = (y < halfHeight) ? 0xFF1A1A2E : 0xFF3A3A3A;
            for (int x = 0; x < width; ++x) {
                pixelBuffer[y * width + x] = color;
            }
        }

        // Prepare camera vectors (shared by all threads)
        double aspectRatio = static_cast<double>(width) / static_cast<double>(height);
        double fovRad = fov * M_PI / 180.0;
        double tanHalfFov = std::tan(fovRad / 2.0);
        
        double yawRad = yaw * M_PI / 180.0;
        double pitchRad = pitch * M_PI / 180.0;
        
        Vector3 forward(
            std::cos(pitchRad) * std::cos(yawRad),
            std::sin(pitchRad),
            std::cos(pitchRad) * std::sin(yawRad)
        );
        forward = forward.normalize();
        
        Vector3 worldUp(0, 1, 0);
        Vector3 right = forward.cross(worldUp).normalize();
        Vector3 up = right.cross(forward).normalize();
        
        // Multi-threaded rendering
        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        
        std::vector<std::future<void>> futures;
        int rowsPerThread = height / numThreads;
        
        for (unsigned int i = 0; i < numThreads; ++i) {
            int startY = i * rowsPerThread;
            int endY = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
            
            futures.push_back(std::async(std::launch::async, 
                [this, startY, endY, forward, right, up, aspectRatio, tanHalfFov]() {
                    renderSlice(startY, endY, forward, right, up, aspectRatio, tanHalfFov);
                }
            ));
        }
        
        // Wait for all threads to complete
        for (auto& f : futures) {
            f.get();
        }

        SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), width * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

private:
    Vector3 position;
    double yaw; // -180..180
    double pitch; // -90..90
    double fov; // 0..360
    int screenWidth;
    int screenHeight;

    Space* space = nullptr;

    // SDL resources
    std::vector<uint32_t> pixelBuffer;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif