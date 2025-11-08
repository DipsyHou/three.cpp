#ifndef UTILS_LOOP_HPP
#define UTILS_LOOP_HPP

#include "../viewpoint.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

// Run a main interaction loop
inline void runInteractionLoop(Viewpoint& viewpoint) {
    if (!viewpoint.initSDL()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return;
    }

    bool running = true;
    SDL_Event event;
    bool mouseCaptured = true;
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  Mouse: Look around" << std::endl;
    std::cout << "  W/A/S/D: Move" << std::endl;
    std::cout << "  Shift/Ctrl: Move up/down" << std::endl;
    std::cout << "  Arrow keys: Rotate view" << std::endl;
    std::cout << "  Alt: Release mouse" << std::endl;
    std::cout << "  ESC: Quit" << std::endl;

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(SDL_DISABLE);
    
    const double moveSpeed = 0.1;
    const double rotateSpeed = 2.0;
    const double mouseSensitivity = 0.2;
    
    // FPS counter
    Uint32 frameCount = 0;
    Uint32 lastFpsTime = SDL_GetTicks();
    double fps = 0.0;

    while (running) {
        const Uint8* keyState = SDL_GetKeyboardState(NULL);
        bool altPressed = keyState[SDL_SCANCODE_LALT] || keyState[SDL_SCANCODE_RALT];
        
        // Handle mouse capture based on Alt key
        if (altPressed && mouseCaptured) {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(SDL_ENABLE);
            mouseCaptured = false;
        } else if (!altPressed && !mouseCaptured) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_ShowCursor(SDL_DISABLE);
            mouseCaptured = true;
        }
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            if (event.type == SDL_MOUSEMOTION && mouseCaptured) {
                double yawDelta = -event.motion.xrel * mouseSensitivity;
                double pitchDelta = event.motion.yrel * mouseSensitivity;
                viewpoint.setYaw(viewpoint.getYaw() + yawDelta);
                viewpoint.setPitch(viewpoint.getPitch() + pitchDelta);
                
                // clamp pitch
                if (viewpoint.getPitch() > 89.0) viewpoint.setPitch(89.0);
                if (viewpoint.getPitch() < -89.0) viewpoint.setPitch(-89.0);
            }
        }
        
        // calculate movement direction
        Vector3 pos = viewpoint.getPosition();
        double yaw = viewpoint.getYaw();
        double yawRad = yaw * M_PI / 180.0;
        
        if (keyState[SDL_SCANCODE_W]) {
            pos.x += moveSpeed * std::cos(yawRad);
            pos.z += moveSpeed * std::sin(yawRad);
        }
        if (keyState[SDL_SCANCODE_S]) {
            pos.x -= moveSpeed * std::cos(yawRad);
            pos.z -= moveSpeed * std::sin(yawRad);
        }
        if (keyState[SDL_SCANCODE_A]) {
            pos.x += moveSpeed * std::sin(yawRad);
            pos.z -= moveSpeed * std::cos(yawRad);
        }
        if (keyState[SDL_SCANCODE_D]) {
            pos.x -= moveSpeed * std::sin(yawRad);
            pos.z += moveSpeed * std::cos(yawRad);
        }
        if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT]) {
            pos.y += moveSpeed;
        }
        if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL]) {
            pos.y -= moveSpeed;
        }
        
        if (keyState[SDL_SCANCODE_LEFT]) {
            viewpoint.setYaw(viewpoint.getYaw() - rotateSpeed);
        }
        if (keyState[SDL_SCANCODE_RIGHT]) {
            viewpoint.setYaw(viewpoint.getYaw() + rotateSpeed);
        }
        if (keyState[SDL_SCANCODE_UP]) {
            double newPitch = viewpoint.getPitch() + rotateSpeed;
            if (newPitch > 89.0) newPitch = 89.0;
            viewpoint.setPitch(newPitch);
        }
        if (keyState[SDL_SCANCODE_DOWN]) {
            double newPitch = viewpoint.getPitch() - rotateSpeed;
            if (newPitch < -89.0) newPitch = -89.0;
            viewpoint.setPitch(newPitch);
        }
        
        viewpoint.setPosition(pos);
        
        // render scene
        viewpoint.render();
        
        // FPS calculation
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFpsTime >= 1000) {
            fps = frameCount * 1000.0 / (currentTime - lastFpsTime);
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            lastFpsTime = currentTime;
        }
        
        // SDL_Delay(16); // ~60 FPS
    }

    SDL_SetRelativeMouseMode(SDL_FALSE);
    std::cout << "Exiting..." << std::endl;
}

#endif
