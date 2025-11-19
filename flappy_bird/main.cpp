// Copyright 2025 - David Brown <d.brown@bigdavedev.com>
// SPDX-License-Identifier: MIT

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_render.h"

namespace flappy_bird
{
namespace
{
constexpr int screen_width = 400;
constexpr int screen_height = 600;
} // namespace
} // namespace flappy_bird

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Flappy Bird",
	                                      flappy_bird::screen_width,
	                                      flappy_bird::screen_height,
	                                      SDL_WindowFlags{});

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

	bool running = true;

	while (running)
	{
		SDL_Event sdl_event{};
		while (SDL_PollEvent(&sdl_event))
		{
			if (sdl_event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
