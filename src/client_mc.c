/*
A GUI client program to allow for friendly control of the motorcontrol network system.
Uses SDL2 and SDL2_ttf.

TODO:
- Actual networking (duh...)
- ...
*/


#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <garbaz/netlib.h>

#define FONT_PATH "/usr/share/fonts/truetype/freefont/FreeMono.ttf"
#define FONT_SIZE 24

#define INFO_TEXT \
"W / UP    -- Forward\n"\
"S / DOWN  -- Backward\n"\
"A / LEFT  -- Left\n"\
"D / RIGHT -- Right\n"\
"Q / ESC   -- Quit\n"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define ERR(...) fprintf(stderr,__VA_ARGS__)
#define SDLERR ERR("SDL Error: %s\n", SDL_GetError())
#define TTFERR ERR("TTF Error: %s\n", TTF_GetError())

SDL_Window *win;
SDL_Surface *sfc, *txt;
SDL_Event e;
const SDL_Color black = {255, 255, 255};
TTF_Font *font;

unsigned char key_state[] = {0, 0, 0, 0};
unsigned char state_changed = 0;

void quit(int c)
{
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(sfc);
	SDL_FreeSurface(txt);
	SDL_DestroyWindow(win);
	SDL_Quit();
	exit(c);
}

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		SDLERR;
		quit(1);
	}
	
	if((win = SDL_CreateWindow("MotorControl Client", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL)
	{
		SDLERR;
		quit(2);
	}
	
	if(TTF_Init())
	{
		TTFERR;
		quit(3);
	}

	if((font = TTF_OpenFont(FONT_PATH,FONT_SIZE)) == NULL)
	{
		TTFERR;
		quit(4);
	}

	sfc = SDL_GetWindowSurface(win);
	
	if((txt = TTF_RenderText_Blended_Wrapped(font, INFO_TEXT, black, WIN_WIDTH)) == NULL)
	{
		TTFERR;
		quit(5);
	}
	
	SDL_BlitSurface(txt, NULL, sfc, NULL);

	SDL_UpdateWindowSurface(win);

	while(1)
	{
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				quit(0);
			}
			else if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
					case SDLK_UP:
						printf("UP pressed\n");
						key_state[UP] = 1;
						state_changed = 1;
						break;
					case SDLK_s:
					case SDLK_DOWN:
						printf("DOWN pressed\n");
						key_state[DOWN] = 1;
						state_changed = 1;
						break;
					case SDLK_a:
					case SDLK_LEFT:
						printf("LEFT pressed\n");
						key_state[LEFT] = 1;
						state_changed = 1;
						break;
					case SDLK_d:
					case SDLK_RIGHT:
						printf("RIGHT pressed\n");
						key_state[RIGHT] = 1;
						state_changed = 1;
						break;
					case SDLK_q:
					case SDLK_ESCAPE:
						quit(0);
						break;
				}
			}
			else if(e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
					case SDLK_UP:
						printf("UP released\n");
						key_state[UP] = 0;
						state_changed = 1;
						break;
					case SDLK_s:
					case SDLK_DOWN:
						printf("DOWN released\n");
						key_state[DOWN] = 0;
						state_changed = 1;
						break;
					case SDLK_a:
					case SDLK_LEFT:
						printf("LEFT released\n");
						key_state[LEFT] = 0;
						state_changed = 1;
						break;
					case SDLK_d:
					case SDLK_RIGHT:
						printf("RIGHT released\n");
						key_state[RIGHT] = 0;
						state_changed = 1;
						break;
				}

			}
		}

		if(state_changed)
		{
			//TODO: Change state!
			state_changed = 0;
		}
		SDL_Delay(10);
	}

	return 0;
}
