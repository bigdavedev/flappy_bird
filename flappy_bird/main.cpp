// Copyright 2025 - David Brown <d.brown@bigdavedev.com>
// SPDX-License-Identifier: MIT

#include "entt/entity/registry.hpp"
#include "glm/glm.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <chrono>
#include <print>
#include <ranges>
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

enum class font_ids
{
	hud
};

struct transform final
{
	glm::vec2 position{};
	float     rotation{};
	glm::vec2 scale{1, 1};
};

struct sprite final
{
	sprite_ids id;

	glm::vec2 size;

	SDL_FlipMode flip_mode{SDL_FlipMode::SDL_FLIP_NONE};
};

struct player_input final
{
	bool flap = false;
};

struct velocity final
{
	glm::vec2 velocity;
};

struct aabb_collider final
{
	glm::vec2 size;
};

struct trigger final
{
	bool triggered = false;
	bool consumed = false;
};

struct text final
{
	font_ids font_id;

	std::string string;
};

struct score final
{
	int score{};
};

struct texture final
{
	SDL_Texture* texture;

	glm::vec2 size{};
};

using sprite_lookup = std::unordered_map<sprite_ids, texture>;

struct font final
{
	TTF_Font* font{};
};

using font_lookup = std::unordered_map<font_ids, font>;

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

void render_sprites(entt::registry& registry, SDL_Renderer* renderer)
{
	auto group = registry.group<sprite>(entt::get<transform>);

	for (auto [entity, sprite, transform]: group.each())
	{
		auto texture = registry.ctx().get<sprite_lookup>().at(sprite.id);

		auto const src_rect =
		    SDL_FRect{.w = texture.size.x, .h = texture.size.y};

		auto const dst_rect = SDL_FRect{.x = transform.position.x,
		                                .y = transform.position.y,
		                                .w = sprite.size.x,
		                                .h = sprite.size.y};

		SDL_RenderTextureRotated(renderer,
		                         texture.texture,
		                         &src_rect,
		                         &dst_rect,
		                         static_cast<double>(transform.rotation),
		                         nullptr,
		                         sprite.flip_mode);
	}
}

void render_score(entt::registry& registry, TTF_TextEngine* text_renderer)
{
	auto  score = registry.ctx().get<flappy_bird::score>().score;
	auto* font = registry.ctx()
	                 .get<flappy_bird::font_lookup>()
	                 .at(flappy_bird::font_ids::hud)
	                 .font;

	auto  string = std::format("{}", score);
	auto* text =
	    TTF_CreateText(text_renderer, font, string.c_str(), string.size());

	int text_width{};
	TTF_GetTextSize(text, &text_width, nullptr);

	TTF_DrawRendererText(
	    text,
	    (flappy_bird::screen_width * 0.5F) - (text_width * 0.5F),
	    100);

	TTF_DestroyText(text);
}

void create_bird_entity(entt::registry& registry)
{
	auto bird = registry.create();
	registry.emplace<transform>(bird,
	                            transform{
	                                .position{64, 128},
    });

	auto const bird_size = glm::vec2{32, 32};

	registry.emplace<sprite>(bird, sprite_ids::bird, bird_size);
	registry.emplace<aabb_collider>(bird, bird_size);
	registry.emplace<velocity>(bird);
	registry.emplace<player_input>(bird);
	registry.emplace<tags::bird>(bird);
}

void spawn_pipe(entt::registry& registry)
{
	constexpr float gap_size = 150.0f;
	constexpr float gap_width = 64.0f;

	float gap_y = 150.0f + (std::rand() % 250);

	auto const create_pipe = [&](glm::vec2 position, glm::vec2 size) {
		auto entity = registry.create();
		registry.emplace<tags::pipe>(entity);
		registry.emplace<transform>(entity,
		                            transform{
		                                .position = position,
		                            });
		registry.emplace<sprite>(entity,
		                         sprite{.id = sprite_ids::pipe, .size = size});
		registry.emplace<velocity>(entity, glm::vec2{-150.0f, 0.0f});
		registry.emplace<aabb_collider>(entity, size);
	};

	auto const top_pipe_position = glm::vec2{screen_width, 0.0F};
	auto const top_pipe_size = glm::vec2{gap_width, gap_y - (gap_size * 0.5F)};
	create_pipe(top_pipe_position, top_pipe_size);

	auto const bottom_pipe_position =
	    glm::vec2{screen_width, gap_y + (gap_size * 0.5F)};
	auto const bottom_pipe_size =
	    glm::vec2{gap_width, screen_height - gap_y - (gap_size * 0.5f)};
	create_pipe(bottom_pipe_position, bottom_pipe_size);

	auto score_trigger = registry.create();
	registry.emplace<transform>(
	    score_trigger,
	    transform{
	        .position = glm::vec2{screen_width + gap_width, top_pipe_size.y}
    });
	registry.emplace<aabb_collider>(score_trigger, glm::vec2{10.0F, gap_size});
	registry.emplace<velocity>(score_trigger, glm::vec2{-150.0F, 0.0F});
	registry.emplace<trigger>(score_trigger);
}

void cleanup_offscreen(entt::registry& registry)
{
	auto pipes = registry.view<tags::pipe, transform, sprite>();

	auto const offscreen = [&](auto entity) {
		auto const& [t, s] = pipes.get(entity);
		return t.position.x + s.size.x < 0;
	};

	using std::ranges::to;
	using std::views::filter;

	auto const entities = pipes | filter(offscreen) | to<std::vector>();

	std::ranges::for_each(entities,
	                      [&](auto entity) { registry.destroy(entity); });
}

SDL_FRect make_collider_box(transform const& t, aabb_collider const& collider)
{
	return {.x = t.position.x,
	        .y = t.position.y,
	        .w = collider.size.x,
	        .h = collider.size.y};
}

void check_triggers(entt::registry& registry)
{
	auto bird_view = registry.view<tags::bird, transform, aabb_collider>();

	auto bird_entity = bird_view.front();

	auto const bird_box =
	    make_collider_box(bird_view.get<transform>(bird_entity),
	                      bird_view.get<aabb_collider>(bird_entity));

	auto triggers =
	    registry.group<trigger>(entt::get<transform, aabb_collider>);

	auto const untriggered = [&](auto entity) {
		return not triggers.get<trigger>(entity).triggered;
	};

	for (auto entity: triggers | std::views::filter(untriggered))
	{
		auto const trigger_box =
		    make_collider_box(triggers.get<transform>(entity),
		                      triggers.get<aabb_collider>(entity));

		auto& trig = triggers.get<trigger>(entity);
		trig.triggered = SDL_HasRectIntersectionFloat(&bird_box, &trigger_box);
	}
}

bool check_collision(entt::registry& registry)
{
	auto bird_view = registry.view<tags::bird, transform, aabb_collider>();

	auto bird_entity = bird_view.front();

	auto const bird_box =
	    make_collider_box(bird_view.get<transform>(bird_entity),
	                      bird_view.get<aabb_collider>(bird_entity));

	if (bird_box.y < 0.0f || bird_box.y + bird_box.h > screen_height)
	{
		return true;
	}

	auto pipe_view = registry.view<tags::pipe, transform, aabb_collider>();
	return std::ranges::any_of(pipe_view, [&](auto entity) {
		auto const pipe_box =
		    make_collider_box(pipe_view.get<transform>(entity),
		                      pipe_view.get<aabb_collider>(entity));

		return SDL_HasRectIntersectionFloat(&bird_box, &pipe_box);
	});
}

void load_all_sprites(SDL_Renderer* renderer, sprite_lookup& sprites)
{
	auto const load_sprite = [renderer](std::string_view path) -> texture {
		SDL_Texture* texture = IMG_LoadTexture(renderer, path.data());

		glm::vec2 size{};
		SDL_GetTextureSize(texture, &size.x, &size.y);

		SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);

		return {.texture = texture, .size = size};
	};

	sprites[sprite_ids::bird] = load_sprite("data/sprites/bird-2.png");
	sprites[sprite_ids::pipe] = load_sprite("data/sprites/Pipe1.png");
}

void load_all_fonts(font_lookup& fonts)
{
	auto const load_font = [](std::string_view path, float size) -> font {
		TTF_Font* ttf_font = TTF_OpenFont(path.data(), size);
		return {.font = ttf_font};
	};

	fonts[font_ids::hud] =
	    load_font("data/fonts/kenney/Kenney Future.ttf", 24.0F);
}

void reset_game(entt::registry& registry)
{
	registry.clear();

	registry.ctx().insert_or_assign(score{});

	create_bird_entity(registry);
}
} // namespace
} // namespace flappy_bird

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init())
	{
		std::println("Failed to initialize SDL: {}", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Flappy Bird",
	                                      flappy_bird::screen_width,
	                                      flappy_bird::screen_height,
	                                      SDL_WindowFlags{});

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

	TTF_TextEngine* text_renderer = TTF_CreateRendererTextEngine(renderer);

	SDL_SetRenderVSync(renderer, 1);

	entt::registry registry{};

	registry.ctx().emplace<flappy_bird::sprite_lookup>();
	registry.ctx().emplace<flappy_bird::font_lookup>();

	load_all_sprites(renderer,
	                 registry.ctx().get<flappy_bird::sprite_lookup>());
	load_all_fonts(registry.ctx().get<flappy_bird::font_lookup>());

	flappy_bird::reset_game(registry);

	bool show_colliders = false;
	auto game_state = flappy_bird::game_state::running;

	auto pipe_timer = std::chrono::steady_clock::now();
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
				if (sdl_event.key.key == SDLK_P)
				{
					game_state = game_state == flappy_bird::game_state::paused
					               ? flappy_bird::game_state::running
					               : flappy_bird::game_state::paused;
				}
				if (sdl_event.key.key == SDLK_C)
				{
					show_colliders = !show_colliders;
				}
			}
		}

		flappy_bird::handle_player_input(registry);

		if (game_state == flappy_bird::game_state::running)
		{
			auto now = std::chrono::steady_clock::now();
			if (now - pipe_timer > std::chrono::seconds{2})
			{
				pipe_timer = now;
				flappy_bird::spawn_pipe(registry);
			}
			flappy_bird::cleanup_offscreen(registry);

			flappy_bird::apply_gravity(registry, 0.016f);
			flappy_bird::update_velocity(registry, 0.016f);

			if (flappy_bird::check_collision(registry))
			{
				flappy_bird::reset_game(registry);
			}

			flappy_bird::check_triggers(registry);

			registry.view<flappy_bird::trigger>().each(
			    [&](flappy_bird::trigger& trigger) {
				    if (trigger.triggered and not trigger.consumed)
				    {
					    trigger.consumed = true;
					    registry.ctx().get<flappy_bird::score>().score++;
				    }
			    });
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		flappy_bird::render_sprites(registry, renderer);

		if (show_colliders)
		{
			registry.view<flappy_bird::transform, flappy_bird::aabb_collider>()
			    .each([renderer](auto transform, auto collider) {
				    auto const box = SDL_FRect{.x = transform.position.x,
				                               .y = transform.position.y,
				                               .w = collider.size.x,
				                               .h = collider.size.y};

				    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				    SDL_RenderRect(renderer, &box);
			    });
		}

		flappy_bird::render_score(registry, text_renderer);

		SDL_RenderPresent(renderer);
	}

	for (auto& [key, sprite]: registry.ctx().get<flappy_bird::sprite_lookup>())
	{
		SDL_DestroyTexture(sprite.texture);
	}

	TTF_DestroyRendererTextEngine(text_renderer);
	for (auto& [key, font]: registry.ctx().get<flappy_bird::font_lookup>())
	{
		TTF_CloseFont(font.font);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();

	return 0;
}
