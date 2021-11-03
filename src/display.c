#include <SDL2/SDL.h>
#include "display.h"
#include "main.h"
#include "title.h"

static void	get_res_code(struct game *cur_game);

void
init_disp(struct game *cur_game)
{
	SDL_Event event;
	SDL_DisplayMode dm;
	
	/* If this is the first time loading, load  display options */
	if (cur_game->display.w == -1) load_display(cur_game);
	
	/* Set up window */
	if (cur_game->display.mode == DISP_FULLSCREEN) {
		SDL_GetDesktopDisplayMode(0, &dm);
		cur_game->display.w = dm.w;
		cur_game->display.h = dm.h;
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_FULLSCREEN_DESKTOP);
		get_res_code(cur_game);
	} else if (cur_game->display.mode == DISP_FULLSCREEN_RES) {
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_FULLSCREEN);
		get_res_code(cur_game);
	} else {
		cur_game->display.window = SDL_CreateWindow(cur_game->display.name, 0, 0, cur_game->display.w, cur_game->display.h, SDL_WINDOW_SHOWN);
	}
	
	if (cur_game->display.vsync == DISP_VSYNC_ON) {
		cur_game->display.renderer = SDL_CreateRenderer(cur_game->display.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC);
	} else {
		cur_game->display.renderer = SDL_CreateRenderer(cur_game->display.window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
	}
	SDL_SetRenderDrawBlendMode(cur_game->display.renderer, SDL_BLENDMODE_BLEND);
	/* Create output texture */
	cur_game->display.output = SDL_CreateTexture(cur_game->display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	/* Poll for an event and wait 100ms - bring the window to the foreground */
	SDL_PollEvent(&event);
	SDL_Delay(100);
	/* Change options screen */
	point_to_display(cur_game);
	/* Save display options */
	save_display(cur_game);
	/* Game is running */
	cur_game->running = SDL_TRUE;
}

void
kill_disp(struct game *cur_game)
{
	/* Delay to prevent segfaults from the other thread loop */
	SDL_Delay(100);
	SDL_DestroyTexture(cur_game->display.output);
	SDL_DestroyRenderer(cur_game->display.renderer);
	SDL_DestroyWindow(cur_game->display.window);
}

static void
get_res_code(struct game *cur_game)
{
	if (cur_game->display.w == 1280 && cur_game->display.h == 720) {
		cur_game->display.res_code = DISP_1280X720;
	} else if (cur_game->display.w == 1360 && cur_game->display.h == 768) {
		cur_game->display.res_code = DISP_1360X768;
	} else if (cur_game->display.w == 1366 && cur_game->display.h == 768) {
		cur_game->display.res_code = DISP_1366X768;
	} else if (cur_game->display.w == 1440 && cur_game->display.h == 900) {
		cur_game->display.res_code = DISP_1440X900;
	} else if (cur_game->display.w == 1600 && cur_game->display.h == 900) {
		cur_game->display.res_code = DISP_1600X900;
	} else if (cur_game->display.w == 1600 && cur_game->display.h == 1080) {
		cur_game->display.res_code = DISP_1600X1080;
	} else if (cur_game->display.w == 1680 && cur_game->display.h == 1050) {
		cur_game->display.res_code = DISP_1680X1050;
	} else if (cur_game->display.w == 1920 && cur_game->display.h == 1080) {
		cur_game->display.res_code = DISP_1920X1080;
	} else if (cur_game->display.w == 1920 && cur_game->display.h == 1200) {
		cur_game->display.res_code = DISP_1920X1200;
	} else if (cur_game->display.w == 2560 && cur_game->display.h == 1440) {
		cur_game->display.res_code = DISP_2560X1440;
	} else if (cur_game->display.w == 2560 && cur_game->display.h == 1600) {
		cur_game->display.res_code = DISP_2560X1600;
	} else if (cur_game->display.w == 3840 && cur_game->display.h == 2160) {
		cur_game->display.res_code = DISP_3840X2160;
	} else {
		cur_game->display.res_code = NOSTATE;
	}
}
