#pragma once

#include "SDL3/SDL.h"

namespace scgf::util
{
void list_graphic_engines();

int list_supported_texture_formats(const char *renderEngineName);

SDL_Surface *load_image(const char *filename);
}  // namespace scgf::util
