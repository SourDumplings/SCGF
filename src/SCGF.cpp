#include "SCGF.h"

#include "SDL3/SDL_main.h"
#include "test/test.h"
#include "util/misc.h"

int main(int argc, char *argv[])
{
	scgf::util::print_envir_info();

	test();
	return 0;
}