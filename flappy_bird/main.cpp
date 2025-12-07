// Copyright 2025 - David Brown <d.brown@bigdavedev.com>
// SPDX-License-Identifier: MIT

#include "entt/entity/registry.hpp"
#include "glm/glm.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"

#include <span>
#include <string_view>
#include <tuple>

namespace flappy_bird
{
namespace
{
constexpr int screen_width = 320;
constexpr int screen_height = 640;

enum class game_state
{
	running,
	paused,
	exiting
};

namespace tags
{
struct bird final
{};

struct pipe final
{};
} // namespace tags

enum class sprite_ids
{
	bird,
	pipe
};

struct transform final
{
	glm::vec2 position{};
	float     rotation{};
	glm::vec2 scale{};
};

struct sprite final
{
	SDL_Texture* texture{};

	glm::vec2 size;
};

struct player_input final
{
	bool flap = false;
};

struct velocity final
{
	glm::vec2 velocity;
};

using sprite_lookup = std::unordered_map<sprite_ids, sprite>;

void apply_gravity(entt::registry& registry, float dt)
{
	registry.view<flappy_bird::tags::bird, flappy_bird::velocity>().each(
	    [dt](auto& vel) { vel.velocity.y += 1200.0f * dt; });
}

void update_velocity(entt::registry& registry, float dt)
{
	auto view = registry.view<transform, velocity>();
	for (auto entity: view)
	{
		auto [t, v] = view.get(entity);
		t.position += v.velocity * dt;
	}
}

void handle_player_input(entt::registry& registry)
{
	auto view = registry.view<player_input, velocity>();
	for (auto entity: view)
	{
		auto [input, vel] = view.get(entity);
		if (input.flap)
		{
			vel.velocity.y = -450.0f;
			input.flap = false;
		}
	}
}

void render_sprites(entt::registry const& registry, SDL_Renderer* renderer)
{
	auto renderable_view =
	    registry.view<flappy_bird::transform, flappy_bird::sprite>();
	for (auto entity: renderable_view)
	{
		auto const& [transform, sprite] = renderable_view.get(entity);

		auto const src_rect = SDL_FRect{.w = sprite.size.x, .h = sprite.size.y};
		auto const dst_rect = SDL_FRect{.x = transform.position.x,
		                                .y = transform.position.y,
		                                .w = sprite.size.x * transform.scale.x,
		                                .h = sprite.size.y * transform.scale.y};

		SDL_RenderTextureRotated(renderer,
		                         sprite.texture,
		                         &src_rect,
		                         &dst_rect,
		                         transform.rotation,
		                         nullptr,
		                         SDL_FlipMode::SDL_FLIP_NONE);
	}
}

void create_bird_entity(entt::registry& registry)
{
	auto bird = registry.create();
	registry.emplace<flappy_bird::transform>(bird,
	                                         flappy_bird::transform{
	                                             .position{0, 100},
	                                             .rotation{},
	                                             .scale{0.1f, 0.1f}
    });

	auto const& sprite_resources = registry.ctx().get<sprite_lookup>();

	registry.emplace<flappy_bird::sprite>(
	    bird,
	    sprite_resources.at(sprite_ids::bird));
	registry.emplace<flappy_bird::tags::bird>(bird);

	registry.emplace<velocity>(bird);

	registry.emplace<player_input>(bird);
}

void load_all_sprites(SDL_Renderer* renderer, sprite_lookup& sprites)
{
	auto const load_sprite = [renderer](std::string_view path) {
		SDL_Texture* texture = IMG_LoadTexture(renderer, path.data());

		float width, height;
		SDL_GetTextureSize(texture, &width, &height);

		return sprite{
		    .texture = texture,
		    .size = glm::vec2{width, height}
        };
	};

	sprites[sprite_ids::bird] = load_sprite("data/sprites/bird-2.png");
}
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

	SDL_SetRenderVSync(renderer, 1);

	auto game_state = flappy_bird::game_state::running;

	entt::registry registry{};

	registry.ctx().emplace<flappy_bird::sprite_lookup>();

	load_all_sprites(renderer,
	                 registry.ctx().get<flappy_bird::sprite_lookup>());

	flappy_bird::create_bird_entity(registry);

	while (game_state != flappy_bird::game_state::exiting)
	{
		SDL_Event sdl_event{};
		while (SDL_PollEvent(&sdl_event))
		{
			if (sdl_event.type == SDL_EVENT_QUIT)
			{
				game_state = flappy_bird::game_state::exiting;
			}

			else if (sdl_event.type == SDL_EVENT_KEY_DOWN)
			{
				if (sdl_event.key.key == SDLK_SPACE)
				{
					registry.view<flappy_bird::player_input>().each(
					    [](auto& input) { input.flap = true; });
				}
			}
		}

		flappy_bird::handle_player_input(registry);

		flappy_bird::apply_gravity(registry, 0.016f);
		flappy_bird::update_velocity(registry, 0.016f);

		SDL_RenderClear(renderer);

		flappy_bird::render_sprites(registry, renderer);

		SDL_RenderPresent(renderer);
	}

	for (auto& [key, sprite]: registry.ctx().get<flappy_bird::sprite_lookup>())
	{
		SDL_DestroyTexture(sprite.texture);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
