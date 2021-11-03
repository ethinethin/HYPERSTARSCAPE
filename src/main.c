#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "audio.h"
#include "display.h"
#include "drawing.h"
#include "entity.h"
#include "main.h"
#include "rand.h"
#include "title.h"

/* Function prototype */
static Uint32	callback(Uint32 interval, void* param);
static void	new_round(struct game *cur_game);
static void	new_game(struct game *cur_game);
static void	game_over(struct game *cur_game, Uint32 interval);

int
main()
{
	SDL_bool down;
	SDL_Event event;
	SDL_TimerID anim_timer;
	struct game cur_game = {
		{ -1, -1, "HYPER STARSCAPE", DISP_WINDOW,
		  DISP_VSYNC_OFF, DISP_1280X720, NULL, NULL,
		  NULL },					/* display */
		0,						/* num_stars */
		NULL,						/* stars */
		{ 205, 112, { 255, 255, 255}, 0, 30, 0, 5 },	/* player */
		0,						/* num_goodies */
		NULL,						/* goodies */
		0,						/* num_baddies */
		NULL,						/* baddies */
		{ SDL_FALSE, SDL_FALSE, SDL_FALSE, SDL_FALSE },	/* keys */
		{ 0, 500, 1200, TITLE, 1.0, MUSIC_ON, SOUND_ON },/* game */
		SDL_FALSE					/* running */
	};
	
	/* Initialize SDL */
	SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER);
	/* Load high scores */
	load_highscores();
	point_to_highscores();
	/* Seed rng, initialize display, and setup stars */
	seed_rng();
	init_disp(&cur_game);
	init_stars(&cur_game, rand_num(500, 1500));
	init_goodies(&cur_game, 5);
	init_baddies(&cur_game, 10);
	/* Set up audio */
	init_audio(&cur_game);
	/* Set up callback loop - this just sends all animations/movement to another thread */
	anim_timer = SDL_AddTimer(16, callback, &cur_game);
	/* Enter main loop */
	while (cur_game.running == SDL_TRUE) {
		/* Check if you need to refresh display */
		if (cur_game.game.state == REFRESH_DISPLAY) {
			SDL_Delay(256);
			kill_disp(&cur_game);
			SDL_Delay(256);
			init_disp(&cur_game);
			cur_game.game.state = OPTIONS;
			SDL_Delay(256);
			anim_timer = SDL_AddTimer(16, callback, &cur_game);
		}
		/* Wait for an event */
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT) {
			if (cur_game.game.state == TITLE) {
				cur_game.running = SDL_FALSE;
			} else {
				cur_game.game.state = TITLE;
			}
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			mouse_click(&cur_game,
				    (int) ((event.motion.x / 3.0) * (1280.0 / cur_game.display.w)),
				    (int) ((event.motion.y / 3.0) * (720.0 / cur_game.display.h)));
		} else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			if (event.type == SDL_KEYDOWN) {
				down = SDL_TRUE;
			} else {
				down = SDL_FALSE;
			}
			switch (event.key.keysym.sym) {
			case SDLK_w:
			case SDLK_UP:
				cur_game.keys.w = down;
				break;
			case SDLK_a:
			case SDLK_LEFT:
				cur_game.keys.a = down;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				cur_game.keys.s = down;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				cur_game.keys.d = down;
				break;
			case SDLK_SPACE:
				if (down == SDL_TRUE) {
					if (cur_game.game.state == GAMEOVER) {
						cur_game.game.state = NEWGAME;
					} else if (cur_game.game.state == TRANSITION) {
						cur_game.game.state = ACTIVE;
					}
				}
				break;
			case SDLK_ESCAPE:
				if (down == SDL_FALSE) break;
				if (cur_game.game.state == GAMEOVER ||
				    cur_game.game.state == ACTIVE ||
				    cur_game.game.state == TRANSITION ||
				    cur_game.game.state == OPTIONS ||
				    cur_game.game.state == HOWTOPLAY ||
				    cur_game.game.state == HIGHSCORE ||
				    cur_game.game.state == CREDITS) {
					cur_game.game.state = TITLE;
					cur_game.game.percent = 1.0;
				} else if (cur_game.game.state == TITLE) {
					if (cur_game.player.hp > 0) {
						cur_game.game.state = TRANSITION;
					} else {
						cur_game.game.state = GAMEOVER;
					}
				}
				break;
			}
		}
	}
	/* Destroy timer, stars, display, and exit */
	SDL_RemoveTimer(anim_timer);
	kill_stars(&cur_game);
	kill_goodies(&cur_game);
	kill_baddies(&cur_game);
	kill_disp(&cur_game);
	kill_highscores();
	kill_audio();
	SDL_Quit();
	return 0;
}

static Uint32
callback(Uint32 interval, void *param)
{
	while (((struct game *) param)->running == SDL_TRUE) {
		if (((struct game *) param)->game.state == REFRESH_DISPLAY) {
			break;
		}
		if (((struct game *) param)->game.state == ACTIVE ||
		((struct game *) param)->game.state == TRANSITION) {
			swap_songs(0);
		} else {
			swap_songs(1);
		}
		animate_world((struct game *) param);
		draw_world((struct game *) param);
		SDL_Delay(interval);
		if (((struct game *) param)->game.round_timer < 0) {
			new_round((struct game *) param);
		}
		if (((struct game *) param)->game.state == NEWGAME) {
			new_game(((struct game *) param));
			((struct game *) param)->game.state = TRANSITION;
			((struct game *) param)->game.percent = 1.0;
		} else if (((struct game *) param)->game.state == GAMEOVER) {
			game_over((struct game *) param, interval);
		} else if (((struct game *) param)->game.state == EXITGAME) {
			((struct game *) param)->running = SDL_FALSE;
		} else if (((struct game *) param)->game.state == CONTINUE) {
			((struct game *) param)->game.state = TRANSITION;
		} else if (((struct game *) param)->game.state >= DISP_WINDOW &&
			   ((struct game *) param)->game.state <= SOUND_OFF) {
			change_display((struct game *) param);
			SDL_Delay(interval);
		}
	}
	return 0;
}



static void
new_round(struct game *cur_game)
{
	/* add bonus to score */
	cur_game->player.score += cur_game->game.bonus;
	/* Kill everything */
	kill_stars(cur_game);
	kill_goodies(cur_game);
	kill_baddies(cur_game);
	/* Increment level, move player to center, and set everything up */
	cur_game->game.level += 1;
	cur_game->player.x = 205;
	cur_game->player.y = 112;
	if (cur_game->game.level > 999) cur_game->game.level = 999;
	init_stars(cur_game, rand_num(500, 1500));
	if ((cur_game->game.level + 1) % 10 == 0) {
		init_goodies(cur_game, (5 + (cur_game->game.level + 1) / 5) * 3);
		init_baddies(cur_game, 10);
		cur_game->game.round_timer = 600;
		cur_game->game.bonus = 500;
	} else {
		init_goodies(cur_game, 5 + (cur_game->game.level + 1) / 5);
		init_baddies(cur_game, 10 + cur_game->game.level * 2);
		cur_game->game.round_timer = 1200;
		cur_game->game.bonus = 500 + (100 * cur_game->game.level);
	}
	if (cur_game->game.bonus > 99999) cur_game->game.bonus = 99999;
	/* enter transition state */
	cur_game->game.state = TRANSITION;
	cur_game->game.percent = 1.0;
}

static void
new_game(struct game *cur_game)
{
	/* zero out score */
	cur_game->player.score = 0;
	/* Kill everything */
	kill_stars(cur_game);
	kill_goodies(cur_game);
	kill_baddies(cur_game);
	/* Start at level 1 (0), 5 hp, starting location */
	cur_game->game.level = 0;
	cur_game->player.hp = 5;
	cur_game->player.x = 205;
	cur_game->player.y = 112;
	/* Set everything up */
	init_stars(cur_game, rand_num(500, 1500));
	init_goodies(cur_game, 5);
	init_baddies(cur_game, 10);
	cur_game->game.round_timer = 1200;
	cur_game->game.bonus = 500;
}

static void
game_over(struct game *cur_game, Uint32 interval)
{
	/* Check if highscore */
	check_if_highscore(cur_game);
	/* Change song */
	swap_songs(1);
	/* Loop */
	while (cur_game->running == SDL_TRUE) {
		animate_world(cur_game);
		draw_world(cur_game);
		SDL_Delay(interval);
		if (cur_game->game.state == NEWGAME) {
			new_game(cur_game);
			cur_game->game.state = TRANSITION;
			cur_game->game.percent = 1.0;
			break;
		} else if (cur_game->game.state == EXITGAME) {
			cur_game->running = SDL_FALSE;
			break;
		} else if (cur_game->game.state == CONTINUE) {
			cur_game->game.state = GAMEOVER;
		} else if (cur_game->game.state >= DISP_WINDOW &&
			   cur_game->game.state <= SOUND_OFF) {
			change_display(cur_game);
			if (cur_game->game.state == REFRESH_DISPLAY) {
				break;
			}
		}
	}
}
