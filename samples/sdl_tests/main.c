// Copyright © Amer Koleci and Contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include <SDL.h>
#include <SDL_syswm.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main()
{
    // Get SDL version
    SDL_version version;
    SDL_GetVersion(&version);

    // initialize SDL
    int sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD;
    if (SDL_Init(sdl_init_flags) != 0)
    {
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Enable native IME.
    //SDL_SetHint(SDL_HINT_IME_SHOW_UI, true);

    // create the window
    SDL_WindowFlags flags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    SDL_Window* window = SDL_CreateWindowWithPosition("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);
    SDL_ShowWindow(window);

    SDL_WindowID id = SDL_GetWindowID(window);
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(window, &info, SDL_SYSWM_CURRENT_VERSION);

    SDL_ShowWindow(window);

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
            }
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);

    return 0;
}
