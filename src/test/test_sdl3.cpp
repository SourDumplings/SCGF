#include "test_sdl3.h"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <vector>
#include <filesystem>
#include "util/performance.h"
#include "util/SDL_helper.h"
#include "util/UTF8_string.h"
#include "util/file_helper.h"

void hello_world() { SDL_Log("Hello SDL World!"); }

int test_perf()
{
	// according to output from list_graphic_engines()
	const char *renderName = "opengl";

	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("Hello, SDL3!", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		SDL_Log("Could not create a window: %s", SDL_GetError());
		return -1;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_Log("Using render engine: %s", renderName);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, renderName);
	if (!renderer)
	{
		SDL_Log("Create renderer failed: %s", SDL_GetError());
		return -1;
	}

	// int vsync = 1;
	int vsync = 0;
	SDL_SetRenderVSync(renderer, vsync);
	SDL_Log("VSync: %d", vsync);

	std::array<SDL_Vertex, 3> origin_vertices = {
		SDL_Vertex{{150, 100}, {1.0f, 0.0f, 0.0f, 1.0f}},  // top
		SDL_Vertex{{000, 300}, {0.0f, 1.0f, 0.0f, 1.0f}},  // left bottom
		SDL_Vertex{{300, 300}, {0.0f, 0.0f, 1.0f, 1.0f}}   // right bottom
	};
	SDL_RenderGeometry(
		renderer,
		nullptr,
		origin_vertices.data(),
		origin_vertices.size(),
		nullptr,
		0
	);

	SDL_Event event{};
	bool keep_going = true;

	uint64_t last_tickets = SDL_GetTicks();
	float position		  = 0.0f;
	float direction		  = 1.0f;

	scgf::util::CPerformance p;
	while (keep_going)
	{
		p.print_every_second();
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT: keep_going = false; break;

				case SDL_EVENT_KEY_DOWN:
				{
					keep_going = keep_going && (event.key.key != SDLK_ESCAPE);
					break;
				}
			}
		}

		uint64_t current_ticks	= SDL_GetTicks();
		float delta_time		= (current_ticks - last_tickets) / 1000.0f;
		last_tickets			= current_ticks;
		position			   += 200.0f * delta_time * direction;

		int width = 0;
		SDL_GetRenderOutputSize(renderer, &width, nullptr);
		float max_position = static_cast<float>(width) -
			(origin_vertices[2].position.x - origin_vertices[1].position.x);

		if (position > max_position) { direction = -1.0f; }
		else if (position < 0.0f)
		{
			position  = 0.0f;
			direction = 1.0f;
		}

		std::vector<SDL_Vertex> vertices;
		for (const auto &vertex : origin_vertices)
		{
			vertices.push_back(vertex);
			vertices.back().position.x += position;
		}

		SDL_SetRenderDrawColor(renderer, 16, 0, 16, 255);
		SDL_RenderClear(renderer);
		SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0);
		SDL_RenderPresent(renderer);
		p.increase_frame_count();
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

int test_ttf()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL init failed.");
		return -1;
	}

	if (!TTF_Init())
	{
		SDL_Log("SDL_ttf init failed.");
		return -1;
	}

	std::filesystem::path cwd	   = scgf::util::get_executable_dir();
	std::filesystem::path fontPath = cwd / std::filesystem::path("../res/dist/font/Jinglei.ttf");

	TTF_Font *font = TTF_OpenFont(fontPath.string().c_str(), 48);
	if (!font) { SDL_Log("Load font error!"); }

	const int WindowWidth  = 800;
	const int WindowHeight = 600;

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "hello SDL");
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, WindowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, WindowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_RESIZABLE);
	SDL_Window *window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, "opengl");

	const char *str = reinterpret_cast<const char *>(u8"你好，世界！lalala");

	int extent, count;
	if (!TTF_MeasureString(font, str, 0, 10, &extent, &count))
	{
		SDL_Log("Error: %s", SDL_GetError());
	}

	SDL_Surface *shadedSurface =
		TTF_RenderText_Shaded(font, str, 0, {0, 0, 0, 255}, {255, 255, 255, 255});
	SDL_Texture *shadedTexture = SDL_CreateTextureFromSurface(renderer, shadedSurface);
	SDL_FRect shadedRect	   = {
		  0,
		  50,
		  static_cast<float>(shadedSurface->w),
		  static_cast<float>(shadedSurface->h)};
	SDL_DestroySurface(shadedSurface);

	// SDL_Surface *lcdSurface =
	// 	TTF_RenderText_LCD_Wrapped(font, str, 0, {0, 0, 0, 255}, {255, 255, 255, 255}, 50);
	SDL_Surface *lcdSurface =
		TTF_RenderText_LCD(font, str, 0, {0, 0, 0, 255}, {255, 255, 255, 255});
	SDL_Texture *lcdTexture = SDL_CreateTextureFromSurface(renderer, lcdSurface);
	SDL_FRect lcdRect		= {
		  0,
		  100,
		  static_cast<float>(lcdSurface->w),
		  static_cast<float>(lcdSurface->h)};
	SDL_DestroySurface(lcdSurface);

	SDL_Surface *blendedSurface = TTF_RenderText_Blended(font, str, 0, {0, 0, 0, 255});
	SDL_Texture *blendedTexture = SDL_CreateTextureFromSurface(renderer, blendedSurface);
	SDL_FRect blendedRect		= {
		  0,
		  150,
		  static_cast<float>(blendedSurface->w),
		  static_cast<float>(blendedSurface->h)};
	SDL_DestroySurface(blendedSurface);

	bool isQuit = false;
	SDL_Event event;

	// 设置透明混合
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	while (!isQuit)
	{
		// 窗口闪烁直到获得焦点
		SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					isQuit = true;
					break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		// 清屏
		SDL_RenderClear(renderer);

		SDL_RenderTexture(renderer, shadedTexture, nullptr, &shadedRect);
		SDL_RenderTexture(renderer, lcdTexture, nullptr, &lcdRect);
		SDL_RenderTexture(renderer, blendedTexture, nullptr, &blendedRect);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(shadedTexture);
	SDL_DestroyTexture(lcdTexture);
	SDL_DestroyTexture(blendedTexture);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

	return 0;
}

int test_mixer()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL init failed.");
		return -1;
	}

	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
	SDL_AudioSpec spec;

	/* Initialize variables */
	spec.freq	  = MIX_DEFAULT_FREQUENCY;
	spec.format	  = MIX_DEFAULT_FORMAT;
	spec.channels = MIX_DEFAULT_CHANNELS;

	Mix_OpenAudio(0, &spec);

	std::filesystem::path cwd = scgf::util::get_executable_dir();
	std::filesystem::path bgmPath =
		cwd / std::filesystem::path("../res/dist/sound/WhenTheMorningComes.mp3");

	Mix_Music *bgm = Mix_LoadMUS(bgmPath.string().c_str());
	Mix_VolumeMusic(100);
	Mix_PlayMusic(bgm, -1);

	Mix_Chunk *sound1 =
		Mix_LoadWAV((cwd / std::filesystem::path("../res/dist/sound/Hit.mp3")).string().c_str());
	Mix_Chunk *sound2 =
		Mix_LoadWAV((cwd / std::filesystem::path("../res/dist/sound/Failed.mp3")).string().c_str());

	const int WindowWidth  = 800;
	const int WindowHeight = 600;

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "hello SDL");
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, WindowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, WindowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_RESIZABLE);
	SDL_Window *window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, "opengl");

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	bool isQuit = false;
	SDL_Event event;
	while (!isQuit)
	{
		SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					isQuit = true;
					break;
				}
				case SDL_EVENT_KEY_DOWN:
				{
					if (event.key.key == SDLK_1) { Mix_PlayChannel(-1, sound1, 0); }
					else if (event.key.key == SDLK_2) { Mix_PlayChannel(-1, sound2, 0); }

					break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		// 清屏
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);
	}

	Mix_FreeMusic(bgm);
	bgm = NULL;

	Mix_FreeChunk(sound1);
	Mix_FreeChunk(sound2);
	sound1 = nullptr;
	sound2 = nullptr;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	Mix_CloseAudio();

	Mix_Quit();
	SDL_Quit();

	return 0;
}

void DrawFont(
	SDL_Renderer *renderer,
	TTF_Font *font,
	const scgf::util::CUTF8String &text,
	int x,
	int y
)
{
	if (text.empty()) { return; }

	SDL_Surface *surface = TTF_RenderText_Blended(
		font,
		reinterpret_cast<const char *>(text.c_str()),
		0,
		SDL_Color{0, 0, 0, 255}
	);
	if (!surface) { SDL_Log(SDL_GetError()); }

	SDL_FRect dstRect = {
		static_cast<float>(x),
		static_cast<float>(y),
		static_cast<float>(surface->w),
		static_cast<float>(surface->h)};
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) { SDL_Log(SDL_GetError()); }
	SDL_DestroySurface(surface);
	SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
	SDL_DestroyTexture(texture);
}

int test_input()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		SDL_Log("SDL init failed.");
		return -1;
	}

	SDL_SetHint("SDL_HINT_ENABLE_SCREEN_KEYBOARD", "auto");

	if (!TTF_Init())
	{
		SDL_Log("SDL_ttf init failed.");
		return -1;
	}

	std::filesystem::path cwd	   = scgf::util::get_executable_dir();
	std::filesystem::path fontPath = cwd / std::filesystem::path("../res/dist/font/Jinglei.ttf");

	TTF_Font *font = TTF_OpenFont(fontPath.string().c_str(), 48);
	if (!font) { SDL_Log("Load font error!"); }

	const int WindowWidth  = 800;
	const int WindowHeight = 600;

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "hello SDL");
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, WindowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, WindowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_RESIZABLE);
	SDL_Window *window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, "opengl");

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	scgf::util::CUTF8String text;
	scgf::util::CUTF8String editing;

	SDL_StartTextInput(window);

	bool isQuit = false;
	SDL_Event event;
	while (!isQuit)
	{
		SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);

		while (SDL_PollEvent(&event))
		{
			SDL_Log("event type: %d", event.type);

			if (event.type == SDL_EVENT_TEXT_INPUT)
			{
				text += reinterpret_cast<const char8_t *>(event.text.text);
				continue;
			}
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					isQuit = true;
					break;
				}
				case SDL_EVENT_KEY_DOWN:
				{
					if (event.key.key == SDLK_BACKSPACE) { text.pop_back(); }
					break;
				}
				case SDL_EVENT_TEXT_EDITING:
				{
					// BUG: Not captured!
					editing = reinterpret_cast<const char8_t *>(event.text.text);
					SDL_Log(
						"Editing: editing = %s, start = %d, length = %d",
						event.edit.text,
						event.edit.start,
						event.edit.length
					);
					break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		if (0 < text.size())
		{
			SDL_Log("Text input: %s", text.c_str());

			DrawFont(renderer, font, text, 0, 0);
			DrawFont(renderer, font, editing, 0, 100);
		}

		SDL_RenderPresent(renderer);
	}

	SDL_StopTextInput(window);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
	return 0;
}

int test_moving_triangle()
{
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("Hello, SDL3!", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		SDL_Log("Could not create a window: %s", SDL_GetError());
		return -1;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
	if (!renderer)
	{
		SDL_Log("Create renderer failed: %s", SDL_GetError());
		return -1;
	}

	std::array<SDL_Vertex, 3> origin_vertices = {
		SDL_Vertex{{150, 100}, {1.0f, 0.0f, 0.0f, 1.0f}},  // top
		SDL_Vertex{{000, 300}, {0.0f, 1.0f, 0.0f, 1.0f}},  // left bottom
		SDL_Vertex{{300, 300}, {0.0f, 0.0f, 1.0f, 1.0f}}   // right bottom
	};
	SDL_RenderGeometry(
		renderer,
		nullptr,
		origin_vertices.data(),
		origin_vertices.size(),
		nullptr,
		0
	);

	SDL_Event event{};
	bool keep_going = true;

	uint64_t last_tickets = SDL_GetTicks();
	float position		  = 0.0f;
	float direction		  = 1.0f;

	while (keep_going)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT: keep_going = false; break;

				case SDL_EVENT_KEY_DOWN:
				{
					keep_going = keep_going && (event.key.key != SDLK_ESCAPE);
					break;
				}
			}

			SDL_Log("Event: %d", event.type);
		}

		uint64_t current_ticks = SDL_GetTicks();
		float delta_time	   = (current_ticks - last_tickets) / 1000.0f;

		last_tickets  = current_ticks;
		position	 += 200.0f * delta_time * direction;

		int width = 0;
		SDL_GetRenderOutputSize(renderer, &width, nullptr);
		float max_position = static_cast<float>(width) -
			(origin_vertices[2].position.x - origin_vertices[1].position.x);

		if (position > max_position) { direction = -1.0f; }
		else if (position < 0.0f)
		{
			position  = 0.0f;
			direction = 1.0f;
		}

		std::vector<SDL_Vertex> vertices;
		for (const auto &vertex : origin_vertices)
		{
			vertices.push_back(vertex);
			vertices.back().position.x += position;
		}

		SDL_SetRenderDrawColor(renderer, 16, 0, 16, 255);
		SDL_RenderClear(renderer);
		SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

int test_stb()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		SDL_Log("SDL init failed.");
		return -1;
	}

	const int WindowWidth  = 800;
	const int WindowHeight = 600;

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "hello SDL");
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, WindowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, WindowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_RESIZABLE);
	SDL_Window *window = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, "opengl");

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	std::filesystem::path cwd	   = scgf::util::get_executable_dir();
	std::filesystem::path birdPath = cwd / std::filesystem::path("../res/dist/sprite/bird.png");

	SDL_Surface *surface = scgf::util::load_image(birdPath.string().c_str());
	int w				 = surface->w;
	int h				 = surface->h;

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	bool isQuit = false;
	SDL_Event event;
	while (!isQuit)
	{
		SDL_FlashWindow(window, SDL_FLASH_UNTIL_FOCUSED);

		while (SDL_PollEvent(&event))
		{
			SDL_Log("event type: %d", event.type);

			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					isQuit = true;
					break;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);
		SDL_RenderClear(renderer);

		SDL_FRect dstRect;
		dstRect.x = 50;
		dstRect.y = 50;
		dstRect.w = w;
		dstRect.h = h;

		SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
	return 0;
}

int test_sdl3()
{
	scgf::util::list_graphic_engines();
	scgf::util::list_supported_texture_formats("opengl");

	// hello_world();
	test_perf();
	// test_ttf();
	// test_mixer();
	// test_input();
	// test_moving_triangle();
	// test_stb();
	return 0;
}
