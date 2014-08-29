#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <SDL/SDL.h>

#define debug_printf(...) fprintf(stderr, __VA_ARGS__)

static void handle_key_event(SDL_KeyboardEvent *key)
{
	if (key->type == SDL_KEYUP && key->keysym.sym == SDLK_RCTRL &&
	    SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
		debug_printf("Input grabbing disabled\n");
		SDL_WM_GrabInput(SDL_GRAB_OFF);
	}
}

static void handle_mousebutton_event(SDL_MouseButtonEvent *button)
{
	if (button->type == SDL_MOUSEBUTTONUP &&
	    SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF) {
		debug_printf("Input grabbing enabled\n");
		SDL_WM_GrabInput(SDL_GRAB_ON);
	}
}

static void stdin_to_stdout(int enable)
{
	char buf[100];
	ssize_t bytes_to_write, bytes_written;

	bytes_to_write = read(STDIN_FILENO, buf, 100);
	while (bytes_to_write > 0 && enable) {
		bytes_written = write(STDOUT_FILENO, buf, bytes_to_write);
		bytes_to_write -= bytes_written;
		if (bytes_written < 0)
			break;
	}
}

int main (void)
{
	SDL_Surface *surface;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	atexit(SDL_Quit);

	if (!(surface = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE))) {
		fprintf(stderr, "Unable to set video mode\n");
		return 1;
	}

	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

	SDL_WM_GrabInput(SDL_GRAB_ON);
	for (;;) {
		stdin_to_stdout(SDL_WM_GrabInput(SDL_GRAB_QUERY)==SDL_GRAB_ON);
		if (!SDL_PollEvent(&event))
		    continue;
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			handle_key_event(&event.key);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			handle_mousebutton_event(&event.button);
			break;

		case SDL_QUIT:
			return 0;

		default:
			break;
		}
	}

	return 0;
}
