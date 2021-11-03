#include <stdlib.h>
#include <SDL2/SDL.h>
#include "audio.h"
#include "entity.h"
#include "main.h"
#include "rand.h"

/* Function prototypes */
static void	move_entity(struct entity *entities, int num_entities, int sprite1, int sprite2);

void
new_color(unsigned char col[3])
{
	do {
		col[0] = rand_num(0, 2) * 127;
		col[1] = rand_num(0, 2) * 127;
		col[2] = rand_num(0, 2) * 127;
	} while (col[0] == 0 && col[1] == 0 && col[2] == 0);
}

void
check_collisions(struct game *cur_game)
{
	int i;
	/* Check goodies */
	for (i = 0; i < cur_game->num_goodies; i += 1) {
		if (cur_game->goodies[i].visible == SDL_FALSE) continue;
		if (cur_game->goodies[i].coords.x >= cur_game->player.x - 6 &&
		    cur_game->goodies[i].coords.x <= cur_game->player.x + 6 &&
		    cur_game->goodies[i].coords.y >= cur_game->player.y - 6 &&
		    cur_game->goodies[i].coords.y <= cur_game->player.y + 6) {
		    	cur_game->player.score += 100;
		    	cur_game->player.hp += 1;
		    	cur_game->goodies[i].visible = SDL_FALSE;
		    	play_sound(SDL_FALSE);
		}
	}
	if (cur_game->player.hp > 10) cur_game->player.hp = 10;
	/* Check baddies */
	for (i = 0; i < cur_game->num_baddies; i += 1) {
		if (cur_game->baddies[i].visible == SDL_FALSE) continue;
		if (cur_game->baddies[i].coords.x >= cur_game->player.x - 5 &&
		    cur_game->baddies[i].coords.x <= cur_game->player.x + 5 &&
		    cur_game->baddies[i].coords.y >= cur_game->player.y - 5 &&
		    cur_game->baddies[i].coords.y <= cur_game->player.y + 5) {
		    	cur_game->player.hp -= 1;
		    	cur_game->baddies[i].visible = SDL_FALSE;
		    	cur_game->game.bonus -= 100;
		    	if (cur_game->game.bonus < 0) cur_game->game.bonus = 0;
		    	play_sound(SDL_TRUE);
		}
	}
	if (cur_game->player.hp <= 0) {
		cur_game->player.hp = 0;
		cur_game->game.state = GAMEOVER;
	}
}

void
init_stars(struct game *cur_game, int num)
{
	int i;
	
	cur_game->num_stars = num;
	cur_game->stars = malloc(sizeof(*cur_game->stars) * num);
	for (i = 0; i < num; i++) {
		cur_game->stars[i].x = rand_num(0, 1280/3);
		cur_game->stars[i].y = rand_num(0, 720/3);
		new_color(cur_game->stars[i].col);
		/* There should be more stars in the distance */
		if (rand_num(0, 99) > 50) {
			cur_game->stars[i].delta = 1;
		} else if (rand_num(0, 99) > 50) {
			cur_game->stars[i].delta = 2;
		} else {
			cur_game->stars[i].delta = 3;
		}
	}
}

void
kill_stars(struct game *cur_game)
{
	free(cur_game->stars);
}

void
init_goodies(struct game *cur_game, int num)
{
	int i;
	
	cur_game->num_goodies = num;
	cur_game->goodies = malloc(sizeof(*cur_game->goodies) * num);
	for (i = 0; i < num; i++) {
		switch (rand_num(0, 3)) {
		case 0:
			cur_game->goodies[i].coords.x = rand_num(0, 1280/3);
			cur_game->goodies[i].coords.y = rand_num(0, 720/12);
			break;
		case 1:
			cur_game->goodies[i].coords.x = rand_num(1280/3 - 1280/12, 1280/3);
			cur_game->goodies[i].coords.y = rand_num(0, 720/3);
			break;
		case 2:
			cur_game->goodies[i].coords.x = rand_num(0, 1280/3);
			cur_game->goodies[i].coords.y = rand_num(720/3 - 720/12, 720/3);
			break;
		case 3:
			cur_game->goodies[i].coords.x = rand_num(0, 1280/12);
			cur_game->goodies[i].coords.y = rand_num(0, 720/3);
			break;
		}
		/* Generate velocity - make sure they're not both 0 */
		do {
			cur_game->goodies[i].coords.w = rand_num(-3, 3);
			cur_game->goodies[i].coords.h = rand_num(-3, 3);
		} while(cur_game->goodies[i].coords.w == 0 &&
			cur_game->goodies[i].coords.h == 0);
		/* Random color, sprite and counter */
		new_color(cur_game->goodies[i].col);
		cur_game->goodies[i].sprite = rand_num(4, 5);
		cur_game->goodies[i].sprite_counter = 30;
		cur_game->goodies[i].visible = SDL_TRUE;
	}
}

void
kill_goodies(struct game *cur_game)
{
	free(cur_game->goodies);
}

void
init_baddies(struct game *cur_game, int num)
{
	int i;
	
	cur_game->num_baddies = num;
	cur_game->baddies = malloc(sizeof(*cur_game->baddies) * num);
	for (i = 0; i < num; i++) {
		switch (rand_num(0, 3)) {
		case 0:
			cur_game->baddies[i].coords.x = rand_num(0, 1280/3);
			cur_game->baddies[i].coords.y = rand_num(0, 720/12);
			break;
		case 1:
			cur_game->baddies[i].coords.x = rand_num(1280/3 - 1280/12, 1280/3);
			cur_game->baddies[i].coords.y = rand_num(0, 720/3);
			break;
		case 2:
			cur_game->baddies[i].coords.x = rand_num(0, 1280/3);
			cur_game->baddies[i].coords.y = rand_num(720/3 - 720/12, 720/3);
			break;
		case 3:
			cur_game->baddies[i].coords.x = rand_num(0, 1280/12);
			cur_game->baddies[i].coords.y = rand_num(0, 720/3);
			break;
		}
		/* Generate velocity - make sure they're not both 0 */
		do {
			cur_game->baddies[i].coords.w = rand_num(-3, 3);
			cur_game->baddies[i].coords.h = rand_num(-3, 3);
		} while(cur_game->baddies[i].coords.w == 0 &&
			cur_game->baddies[i].coords.h == 0);
		/* Random color, sprite and counter */
		new_color(cur_game->baddies[i].col);
		cur_game->baddies[i].sprite = rand_num(2, 3);
		cur_game->baddies[i].sprite_counter = 15;
		cur_game->baddies[i].visible = SDL_TRUE;
	}
}

void
kill_baddies(struct game *cur_game)
{
	free(cur_game->baddies);
}

void
move_player(struct game *cur_game)
{
	/* Decrement sprite counter and change sprite if necessary */
	cur_game->player.sprite_counter -= 1;
	if (cur_game->player.sprite_counter <= 0) {
		cur_game->player.sprite_counter = 30;
		if (cur_game->player.sprite == 1) {
			cur_game->player.sprite = 0;
		} else {
			cur_game->player.sprite = 1;
		}
	}
	/* Move player if keys are down */
	if (cur_game->keys.w == SDL_TRUE) cur_game->player.y -= 5;
	if (cur_game->keys.a == SDL_TRUE) cur_game->player.x -= 5;
	if (cur_game->keys.s == SDL_TRUE) cur_game->player.y += 5;
	if (cur_game->keys.d == SDL_TRUE) cur_game->player.x += 5;
	/* Make sure player is within the world */
	if (cur_game->player.x < 1) cur_game->player.x = 1;
	if (cur_game->player.y < 1) cur_game->player.y = 1;
	if (cur_game->player.x > 1280/3 - 16 - 1) cur_game->player.x = 1280/3 - 16 - 1;
	if (cur_game->player.y > 720/3 - 16 - 1) cur_game->player.y = 720/3 - 16 - 1;
}


void
move_goodies(struct game *cur_game)
{
	move_entity(cur_game->goodies, cur_game->num_goodies, 4, 5);
}


void
move_baddies(struct game *cur_game)
{
	int i;
	
	move_entity(cur_game->baddies, cur_game->num_baddies, 2, 3);
	for (i = 0; i < cur_game->num_baddies; i += 1) {
		/* Have a chance to change velocities */
		if (rand_num(1, 99) >= 99) {
			do {
				cur_game->baddies[i].coords.w = rand_num(-3, 3);
			} while (cur_game->baddies[i].coords.w == 0 && cur_game->baddies[i].coords.h == 0);
		}
		if (rand_num(1, 99) >= 99) {
			do {
				cur_game->baddies[i].coords.h = rand_num(-3, 3);
			} while (cur_game->baddies[i].coords.w == 0 && cur_game->baddies[i].coords.h == 0);
		}
	}
}

static void
move_entity(struct entity *entities, int num_entities, int sprite1, int sprite2)
{
	int i;
	for (i = 0; i < num_entities; i += 1) {
		/* Decrement sprite counter and change sprite if necessary */
		entities[i].sprite_counter -= 1;
		if (entities[i].sprite_counter <= 0) {
			entities[i].sprite_counter = 30;
			if (entities[i].sprite == sprite1) {
				entities[i].sprite = sprite2;
			} else {
				entities[i].sprite = sprite1;
			}
		}
		/* Move entity */
		entities[i].coords.x += entities[i].coords.w;
		entities[i].coords.y += entities[i].coords.h;
		/* Make sure goodie is within the world */
		if (entities[i].coords.x < 1) {
			entities[i].coords.x = 1;
			entities[i].coords.w *= -1;
		} else if (entities[i].coords.x > 1280/3 - 16 - 1) {
			entities[i].coords.x = 1280/3 - 16 - 1;
			entities[i].coords.w *= -1;
		}
		if (entities[i].coords.y < 1) {
			entities[i].coords.y = 1;
			entities[i].coords.h *= -1;
		} else if (entities[i].coords.y > 720/3 - 16 - 1) {
			entities[i].coords.y = 720/3 - 16 - 1;
			entities[i].coords.h *= -1;
		}
	}
}
