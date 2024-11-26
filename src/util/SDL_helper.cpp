#include "SDL_helper.h"

#include <cstdio>
#include <iostream>
#include "util/logger.h"

#include "stb/stb_image.h"

namespace scgf::util
{
void list_graphic_engines()
{
	int count = SDL_GetNumRenderDrivers();
	std::ostringstream oss;
	oss << std::endl;
	oss << "-------------- Graphic engines list BEGIN -------------" << std::endl;
	for (int i = 0; i < count; ++i)
	{
		const char *name = SDL_GetRenderDriver(i);
		char res[100];
		sprintf(res, "Render driver[%d]: %s\n", i, name);
		oss << res;
	}
	oss << "-------------- Graphic engines list END -------------" << std::endl;
	SCGF_LOG_INFO(oss.str());

	/*
		输出：
-------------- Graphic engines list BEGIN -------------
Render driver[0]: gpu
Render driver[1]: direct3d11
Render driver[2]: direct3d12
Render driver[3]: direct3d
Render driver[4]: opengl
Render driver[5]: opengles2
Render driver[6]: vulkan
Render driver[7]: software
-------------- Graphic engines list END -------------
	 */
}

void list_supported_texture_formats(SDL_Renderer *renderer, const char *renderEngineName)
{
	SDL_PixelFormat *textureFormat = static_cast<SDL_PixelFormat *>(SDL_GetPointerProperty(
		SDL_GetRendererProperties(renderer),
		SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER,
		nullptr
	));

	std::ostringstream oss;
	oss << std::endl;
	oss << "-------------- Supported texture format list BEGIN -------------" << std::endl;
	oss << "Renderer engine name: " << renderEngineName << std::endl;
	int index = 0;
	char res[100];
	while (textureFormat && *textureFormat != SDL_PIXELFORMAT_UNKNOWN)
	{
		sprintf(res, "Texture format[%d]: %s\n", index, SDL_GetPixelFormatName(*textureFormat));
		oss << res;
		++textureFormat;
		++index;
	}
	oss << "-------------- Supported texture format list END -------------" << std::endl;
	SCGF_LOG_INFO(oss.str());
}

int list_supported_texture_formats(const char *renderEngineName)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SCGF_LOG_CRITICAL("SDL_Init failed: {}", SDL_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("Hello, SDL3!", 800, 600, SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		SCGF_LOG_CRITICAL("Could not create a window: {}", SDL_GetError());
		return -1;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, renderEngineName);
	if (!renderer)
	{
		SCGF_LOG_CRITICAL("Create renderer failed: {}", SDL_GetError());
		return -1;
	}

	list_supported_texture_formats(renderer, renderEngineName);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

SDL_Surface *load_image(const char *filename)
{
	Sint32 width		 = 0;
	Sint32 height		 = 0;
	Sint32 bytesPerPixel = 0;  // Since we work with bytes instead of bits here in C.

	// Now the good stuff:
	Uint8 *data = stbi_load(filename, &width, &height, &bytesPerPixel, 0);

	// What is a "pitch"? Its the "Dot Pitch" of an computer image.
	// Taken from https://en.wikipedia.org/wiki/Dot_pitch
	// Dot pitch (sometimes called line pitch, stripe pitch, or phosphor pitch) is
	// a specification for a computer display, computer printer, image scanner, or
	// other pixel-based devices that describe the distance, for example, between
	// dots (sub-pixels) on a display screen.
	Sint32 pitch = width * bytesPerPixel;
	pitch		 = (pitch + 3) & ~3;

	// Setup relevance bitmask
	/* Sint32 Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	Rmask = 0x000000FF;
	Gmask = 0x0000FF00;
	Bmask = 0x00FF0000;
	Amask = (bytesPerPixel == STBI_rgb_alpha) ? 0xFF000000 : 0;
#else
	int s = (bytesPerPixel == STBI_rgb_alpha) ? 0 : 8;
	Rmask = 0xFF000000 >> s;
	Gmask = 0x00FF0000 >> s;
	Bmask = 0x0000FF00 >> s;
	Amask = 0x000000FF >> s;
#endif
	SDL_Surface *surface = SDL_CreateSurfaceFrom(
		width,
		height,
		SDL_GetPixelFormatForMasks(bytesPerPixel * 8, Rmask, Gmask, Bmask, Amask),
		data,
		pitch
	); */
	SDL_PixelFormat format;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	format = bytesPerPixel == STBI_rgb_alpha ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_XBGR8888;
#else
	format = bytesPerPixel == STBI_rgb_alpha ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_XRGB8888;
#endif
	SDL_Surface *surface = SDL_CreateSurfaceFrom(width, height, format, data, pitch);

	if (!surface)
	{
		// NOTE: Should free stbi_load 'data' variable here
		return NULL;
	}
	return surface;
}
}  // namespace scgf::util