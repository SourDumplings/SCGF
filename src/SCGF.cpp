#include "SCGF.h"

#include "SDL3/SDL_main.h"
#include "util/logger.h"
#include "util/misc.h"

int main(int argc, char *argv[])
{
	SCGF_LOG_INFO("Welcome to " PROJECT_NAME "!");

	scgf::util::print_envir_info();
	return 0;
}