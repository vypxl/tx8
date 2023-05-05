#pragma once

#include "tx8/core/types.hpp"

#include <SDL2/SDL.h>


namespace tx {
    class CPU;

    class GPU {
      private:
        CPU& cpu;

        SDL_Window*   window   = nullptr;
        SDL_Renderer* renderer = nullptr;

        tx::uint32 last_frame_time = 0;

      public:
        tx::uint32 frame_time = 1000 / 60;

        explicit GPU(CPU& cpu);
        // Destroys the SDL instance
        void destroy();

        // Initializes SDL; returns true if successful, false on error
        bool init_window();
        // Handles (polls) window events; returns true if the window should close
        bool window_events();
        // Keeps drawing the window until it is closed
        void window_loop();
        void draw_frame();
        // Only draws a frame if enough time has passed since the last one, determined by frame_time
        void draw_frame_if_needed();
    };
} // namespace tx
