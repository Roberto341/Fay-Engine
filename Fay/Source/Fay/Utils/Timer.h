#pragma once

#include <chrono>
#include <iostream>

namespace Fay
{
    class Timer {
    public:
        Timer() {
            reset();
        }

        // Call once per frame; returns delta time in seconds
        float elapsed() {
            using namespace std::chrono;

            auto now = high_resolution_clock::now();
            frameTime = duration_cast<duration<float>>(now - lastTime).count();
            lastTime = now;

            frameCount++;

            // Update FPS once per second
            if (duration_cast<seconds>(now - fpsStartTime).count() >= 1) {
                fps = frameCount;
                frameCount = 0;
                fpsStartTime = now;
            }

            return frameTime;
        }

        float getDeltaTime() const {
            return frameTime;
        }

        int getFPS() const {
            return fps;
        }

        void reset() {
            lastTime = std::chrono::high_resolution_clock::now();
            fpsStartTime = lastTime;
            frameCount = 0;
            fps = 0;
            frameTime = 0.0f;
        }

    private:
        std::chrono::high_resolution_clock::time_point lastTime;
        std::chrono::high_resolution_clock::time_point fpsStartTime;
        int frameCount = 0;
        int fps = 0;
        float frameTime = 0.0f;
    };
}