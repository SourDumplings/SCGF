// Ref: https://www.codingwiththomas.com/blog/an-entity-component-system-from-scratch
#include "test_ecs.h"

#include <iostream>
#include <unordered_map>
#include "SDL3/SDL.h"
#include "util/SDL_helper.h"
#include <filesystem>
#include "util/file_helper.h"

// an alias to make it more expressive
using entity = std::size_t;
// we need the maximum count, this isn't the best solution, but it does the job for now
entity max_entity = 0;
// and here we create some unique ID's
std::size_t create_entity()
{
	static std::size_t entities = 0;
	++entities;
	max_entity = entities;
	return entities;
}

// a sdl rect holds width, height, x and y
// a sprite takes a source  rectangle
// and places it to the given destination on the screen
struct sprite_component
{
	SDL_FRect src;
	SDL_FRect dst;
	SDL_Texture* texture;
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

struct registry
{
	std::unordered_map<entity, sprite_component> sprites;
	std::unordered_map<entity, transform_component> transforms;
	std::unordered_map<entity, keyinputs_component> keys;
};

struct sprite_system
{
	// first we'll update all sprites
	void update(registry& reg)
	{
		// we iterate over all entities
		for (entity e = 1; e <= max_entity; e++)
		{
			// all entities with a sprite AND a transform
			// will update their position
			if (reg.sprites.contains(e) && reg.transforms.contains(e))
			{
				reg.sprites[e].dst.x = reg.transforms[e].pos_x;
				reg.sprites[e].dst.y = reg.transforms[e].pos_y;
			}
		}
	}
	void render(registry& reg, SDL_Renderer* renderer)
	{
		// and finally all sprites are rendered in this render method
		// to their appropriate position on the screen
		for (entity e = 1; e <= max_entity; e++)
		{
			if (reg.sprites.contains(e))
			{
				SDL_RenderTexture(
					renderer,
					reg.sprites[e].texture,
					&reg.sprites[e].src,
					&reg.sprites[e].dst
				);
			}
		}
	}
};

struct transform_system
{
	// we'll use a constant time for this demonstration
	float dt = 0.1f;

	void update(registry& reg)
	{
		for (entity e = 1; e <= max_entity; e++)
		{
			// and each entity with a transform compnent will update
			// their position with respect to their velocity
			if (reg.transforms.contains(e))
			{
				reg.transforms[e].pos_x += reg.transforms[e].vel_x * dt;
				reg.transforms[e].pos_y += reg.transforms[e].vel_y * dt;
			}
		}
	}
};

struct movement_system
{
	void update(registry& reg)
	{
		// here we get an array of all keys from sdl
		const bool* keys = SDL_GetKeyboardState(NULL);

		for (entity e = 1; e <= max_entity; e++)
		{

			// and all entities with a movement component get their velocity
			// set if A, S, D or W is pressed on the keyboard
			if (reg.transforms.contains(e) && reg.keys.contains(e))
			{

				if (keys[SDL_SCANCODE_A]) { reg.transforms[e].vel_x = -1.0f; }
				if (keys[SDL_SCANCODE_S]) { reg.transforms[e].vel_y = 1.0f; }
				if (keys[SDL_SCANCODE_W]) { reg.transforms[e].vel_y = -1.0f; }
				if (keys[SDL_SCANCODE_D]) { reg.transforms[e].vel_x = 1.0f; }

				if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D])
				{
					reg.transforms[e].vel_x = 0.0f;
				}
				if (!keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_W])
				{
					reg.transforms[e].vel_y = 0.0f;
				}
			}
		}
	}
};

class ECSGame
{
public:
	ECSGame(std::size_t width, std::size_t height)
		: m_width(width)
		, m_height(height)
	{
		m_window = SDL_CreateWindow("sdl window", m_width, m_height, SDL_WINDOW_RESIZABLE);

		if (m_window == NULL)
		{
			std::cout << "Could not create window: " << SDL_GetError() << '\n';
			m_is_running = false;
		}
		m_renderer = SDL_CreateRenderer(m_window, "opengl");
		if (!m_renderer)
		{
			std::cout << "Error creating SDL renderer.\n";
			m_is_running = false;
		}

		m_is_running = true;
	}

	~ECSGame()
	{
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	// this we'll use in our mainloop to check if our game is running
	bool is_running() { return m_is_running; }
	// read keyboard / mouse input
	void read_input()
	{
		// read sdl events
		SDL_Event sdl_event;
		SDL_PollEvent(&sdl_event);
		// get all keys here
		const bool* keystates = SDL_GetKeyboardState(NULL);

		// quit / close our window by pressing escape
		if (keystates[SDL_SCANCODE_ESCAPE]) { m_is_running = false; }
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

	registry& get_registry() { return m_registry; }
	SDL_Renderer* get_renderer() { return m_renderer; }

private:
	// our members for now ...
	std::size_t m_width;
	std::size_t m_height;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	bool m_is_running = true;

	// and here the members
	registry m_registry;

	sprite_system m_sprite_system;
	transform_system m_transform_system;
	movement_system m_movement_system;
};

// ....

// which will create a window by running this main
// and closes the window by pressing escape

int test_ecs()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return -1;
	}

	// create a game on a 800x600 window
	ECSGame game(800, 600);

	// here we create the entity, remember this is just an alias for std::size_t
	entity bird = create_entity();

	std::filesystem::path cwd	   = scgf::util::get_executable_dir();
	std::filesystem::path birdPath = cwd / std::filesystem::path("../res/dist/sprite/bird.png");

	SDL_Surface* surface	 = scgf::util::load_image(birdPath.string().c_str());
	SDL_Texture* birdTexture = SDL_CreateTextureFromSurface(game.get_renderer(), surface);
	SDL_DestroySurface(surface);

	// here we add a sprite component with the [] operator from the unordered map
	game.get_registry().sprites[bird] = sprite_component{
		SDL_FRect{0, 0, 300, 230},
		SDL_FRect{10, 10, 100, 73},
		// this sdl function creates a texture by it's path
		// bird_path is the path to the bird.png generated by cmake
		// so the path is not relative or depends on my system
		birdTexture
	};

	// a transform component
	game.get_registry().transforms[bird] = transform_component{10, 10, 0, 0};
	// and we add key inputs
	game.get_registry().keys[bird] = keyinputs_component{};
	while (game.is_running())
	{
		game.read_input();
		game.update();
		game.render();
	}

	SDL_DestroyTexture(birdTexture);
	return 0;
}