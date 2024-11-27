#include "test_entt.h"

#include <iostream>
#include "entt/entt.hpp"
#include "SDL3/SDL.h"
#include "util/SDL_helper.h"
#include <filesystem>
#include "util/file_helper.h"

struct Position
{
	float x, y;
};

struct Velocity
{
	float dx, dy;
};

int simple_test_entt()
{
	// 创建一个 ECS 注册表
	entt::registry registry;

	// 创建一个实体并添加组件
	auto entity = registry.create();
	registry.emplace<Position>(entity, 0.0f, 0.0f);
	registry.emplace<Velocity>(entity, 1.0f, 1.0f);

	// 更新实体的状态
	for (int i = 0; i < 5; ++i)
	{
		// 遍历所有具有 Position 和 Velocity 组件的实体
		registry.view<Position, Velocity>().each(
			[](auto entity, Position &pos, Velocity &vel)
			{
				pos.x += vel.dx;
				pos.y += vel.dy;
				std::cout << "Entity " << (int)entity << " Position: (" << pos.x << ", " << pos.y
						  << ")\n";
			}
		);
	}

	return 0;
}

// a sdl rect holds width, height, x and y
// a sprite takes a source  rectangle
// and places it to the given destination on the screen
struct sprite_component
{
	SDL_FRect src;
	SDL_FRect dst;
	SDL_Texture *texture;
};

// a position and a velocity represented as transform
struct transform_component
{
	float pos_x;
	float pos_y;
	float vel_x;
	float vel_y;
};

// we'll only create a type for keyinputs, we get the pressed keys later
struct keyinputs_component
{
};

struct sprite_system
{
	// we pass in entt::registry now
	void update(entt::registry &reg)
	{
		// we create a view for sprite and transform
		auto view = reg.view<sprite_component, transform_component>();
		// we iterate over all entities with a sprite and transform component
		// the logic inside the lambda is the same as in the previous for loop
		view.each(
			[](auto &s, auto &t)
			{
				s.dst.x = t.pos_x;
				s.dst.y = t.pos_y;
			}
		);
	}
	void render(entt::registry &reg, SDL_Renderer *renderer)
	{
		// we iterate over all entities
		auto view = reg.view<sprite_component>();
		view.each([renderer](auto &s) { SDL_RenderTexture(renderer, s.texture, &s.src, &s.dst); });
	}
};

struct transform_system
{
	float dt = 0.1f;

	void update(entt::registry &reg)
	{
		auto view = reg.view<transform_component>();
		view.each(
			[](auto &t)
			{
				t.pos_x += t.vel_x * 0.1f;
				t.pos_y += t.vel_y * 0.1f;
			}
		);
	}
};

struct movement_system
{
	void update(entt::registry &reg)
	{
		const bool *keys = SDL_GetKeyboardState(NULL);

		auto view = reg.view<transform_component, keyinputs_component>();
		view.each(
			[&keys](auto &t)
			{
				if (keys[SDL_SCANCODE_A]) { t.vel_x = -1.0f; }
				if (keys[SDL_SCANCODE_S]) { t.vel_y = 1.0f; }
				if (keys[SDL_SCANCODE_W]) { t.vel_y = -1.0f; }
				if (keys[SDL_SCANCODE_D]) { t.vel_x = 1.0f; }
				if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) { t.vel_x = 0.0f; }
				if (!keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_W]) { t.vel_y = 0.0f; }
			}
		);
	}
};

class EnttGame
{
public:
	EnttGame(std::size_t width, std::size_t height)
		: m_width(width)
		, m_height(height)
	{
		std::cout << "Game ctor" << std::endl;
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
			m_is_running = false;
			return;
		}
		std::cout << "SDL initialized" << std::endl;

		m_window = SDL_CreateWindow("sdl window", m_width, m_height, SDL_WINDOW_RESIZABLE);

		if (m_window == NULL)
		{
			std::cout << "Could not create window: " << SDL_GetError() << '\n';
			m_is_running = false;
			return;
		}
		std::cout << "Window created" << std::endl;

		m_renderer = SDL_CreateRenderer(m_window, "opengl");
		if (!m_renderer)
		{
			std::cout << "Error creating SDL renderer.\n";
			m_is_running = false;
			return;
		}
		std::cout << "Renderer created" << std::endl;

		m_is_running = true;
	}
	~EnttGame()
	{
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	entt::registry &get_registry() { return m_registry; }
	SDL_Renderer *get_renderer() { return m_renderer; }

	bool is_running() { return m_is_running; }
	void read_input()
	{
		SDL_Event sdl_event;
		SDL_PollEvent(&sdl_event);
		const bool *keystates = SDL_GetKeyboardState(NULL);

		if (keystates[SDL_SCANCODE_ESCAPE] || sdl_event.type == SDL_EVENT_QUIT)
		{
			m_is_running = false;
		}
	}
	void update()
	{
		m_transform_system.update(m_registry);
		m_movement_system.update(m_registry);
		m_sprite_system.update(m_registry);
	}
	void render()
	{
		SDL_RenderClear(m_renderer);

		m_sprite_system.render(m_registry, m_renderer);

		SDL_RenderPresent(m_renderer);
	}

private:
	std::size_t m_width;
	std::size_t m_height;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	bool m_is_running;

	entt::registry m_registry;

	sprite_system m_sprite_system;
	transform_system m_transform_system;
	movement_system m_movement_system;
};

int entt_framework()
{
	// we can create a entt::registry to store our entities
	entt::registry registry;

	// we'll create 10 entities
	for (std::size_t i = 0; i < 10; ++i)
	{
		// to create an entity, use the create function
		const auto entity = registry.create();
		// and by using emplace with a specific component
		// we add a component to an entity and forward all their arguments
		registry.emplace<Position>(entity, i * 1.f, i * 1.f);
		// if we have a even number, we'll also add a Velocity component to it
		if (i % 2 == 0) { registry.emplace<Velocity>(entity, i * .1f, i * .1f); }
	}

	// now we can create a view, which is a partial registry
	// pass in all the components we need and in the view we'll have
	// all entities with the given components
	auto view = registry.view<const Position, Velocity>();

	// now there are three different options to loop through the entities from our view
	// 1.: a basic lambda which will be called with the given components
	// note: the lambda arguments need to match the components with which we create this view
	view.each([](const auto &pos, auto &vel) { /* ... */ });

	// 2.: an extended lambda which also gives us the entity if we need it
	view.each([](const auto entity, const auto &pos, auto &vel) { /* ... */ });

	// 3. a for loop by using structured bindings
	for (auto [entity, pos, vel] : view.each())
	{
		// ...
	}

	// 4. a for loop with a forward iterator
	for (auto entity : view)
	{
		auto &vel = view.get<Velocity>(entity);
		// ...
	}
	return 0;
}

// Ref: https://www.codingwiththomas.com/blog/use-entt-when-you-need-an-ecs
int test_game()
{
	std::cout << "Creating Game object" << std::endl;
	EnttGame game(800, 600);
	std::cout << "Game object created" << std::endl;

	std::filesystem::path cwd	   = scgf::util::get_executable_dir();
	std::filesystem::path birdPath = cwd / std::filesystem::path("../res/dist/sprite/bird.png");
	SDL_Surface *surface		   = scgf::util::load_image(birdPath.string().c_str());
	SDL_Texture *birdTexture	   = SDL_CreateTextureFromSurface(game.get_renderer(), surface);
	SDL_DestroySurface(surface);

	// a bird entity
	auto bird_1 = game.get_registry().create();
	// we emplace the sprite component
	game.get_registry().emplace<sprite_component>(
		bird_1,
		SDL_FRect{0, 0, 300, 230},
		SDL_FRect{10, 10, 100, 73},
		birdTexture
	);
	// we emplace the transform component
	game.get_registry().emplace<transform_component>(bird_1, 10.0f, 10.0f, 0.0f, 0.0f);
	// and we emplace the keyinputs
	game.get_registry().emplace<keyinputs_component>(bird_1);

	// ... and two more birds
	auto bird_2 = game.get_registry().create();
	game.get_registry().emplace<sprite_component>(
		bird_2,
		SDL_FRect{0, 0, 300, 230},
		SDL_FRect{0, 0, 100, 73},
		birdTexture
	);
	game.get_registry().emplace<transform_component>(bird_2, 10.0f, 500.0f, 0.01f, -0.01f);

	auto bird_3 = game.get_registry().create();
	game.get_registry().emplace<sprite_component>(
		bird_3,
		SDL_FRect{0, 0, 300, 230},
		SDL_FRect{200, 300, 100, 73},
		birdTexture
	);

	// our game loop remains the same
	while (game.is_running())
	{
		game.read_input();
		game.update();
		game.render();
	}

	SDL_DestroyTexture(birdTexture);

	return 0;
}

int test_entt()
{
	// simple_test_entt();
	test_game();
	return 0;
}