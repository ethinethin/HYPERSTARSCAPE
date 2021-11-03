#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>

struct game {
	struct {
		int w;
		int h;
		const char *name;
		int mode;
		int vsync;
		int res_code;
		SDL_Window *window;
		SDL_Renderer *renderer;
		SDL_Texture *output;
	} display;
	int num_stars;
	struct {
		int x;
		int y;
		int delta;
		unsigned char col[3];
	} *stars;
	struct {
		int x;
		int y;
		unsigned char col[3];
		int sprite;
		int sprite_counter;
		unsigned int score;
		int hp;
	} player;
	int num_goodies;
	struct entity {
		SDL_Rect coords;
		unsigned char col[3];
		int sprite;
		int sprite_counter;
		SDL_bool visible;
	} *goodies;
	int num_baddies;
	struct entity *baddies;
	struct {
		SDL_bool w;
		SDL_bool a;
		SDL_bool s;
		SDL_bool d;
	} keys;
	struct {
		int level;
		int bonus;
		int round_timer;
		int state;
		float percent;
		int music;
		int sound;
	} game;
	SDL_bool running;
};

enum states { NEWGAME, ACTIVE, GAMEOVER, TRANSITION, TITLE, OPTIONS, HOWTOPLAY, HIGHSCORE, CREDITS, EXITGAME, CONTINUE, NOSTATE, DISP_WINDOW, DISP_FULLSCREEN, DISP_FULLSCREEN_RES, DISP_VSYNC_ON, DISP_VSYNC_OFF, DISP_1280X720, DISP_1360X768, DISP_1366X768, DISP_1440X900, DISP_1600X900, DISP_1600X1080, DISP_1680X1050, DISP_1920X1080, DISP_1920X1200, DISP_2560X1440, DISP_2560X1600, DISP_3840X2160, MUSIC_ON, MUSIC_OFF, SOUND_ON, SOUND_OFF, REFRESH_DISPLAY };

void	load_display(struct game *cur_game);
void	new_color(unsigned char col[3]);

#endif
