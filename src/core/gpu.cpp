#include "tx8/core/gpu.hpp"

#include "tx8/core/cpu.hpp"
#include "tx8/core/log.hpp"

namespace tx {
    GPU::GPU(CPU& cpu) : cpu(cpu) {};

    void GPU::destroy() {
        tx::log_debug("[gpu] shutting down...");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool GPU::init_window() {
        tx::log_debug("[gpu] initializing sdl...");

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            tx::log_err("[gpu] failed to initialize SDL: {}", SDL_GetError());
            return false;
        }

        window = SDL_CreateWindow("tx8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 288, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            tx::log_err("[gpu] failed to create window: {}", SDL_GetError());
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            tx::log_err("[gpu] failed to create renderer: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    bool GPU::window_events() { // NOLINT
        SDL_Event event;
        bool      should_quit = false;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) { should_quit = true; }
        }
        return should_quit;
    }

    void GPU::window_loop() {
        while (true) {
            if (window_events()) break;
            draw_frame_if_needed();
        }
    }

    void GPU::draw_frame() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // TODO draw stuff here
        SDL_RenderPresent(renderer);
    }

    void GPU::draw_frame_if_needed() {
        tx::uint32 time = SDL_GetTicks();
        if (time - last_frame_time > frame_time) {
            draw_frame();
            last_frame_time = time;
        }
    }
} // namespace tx
