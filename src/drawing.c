#include <SDL2/SDL.h>
#include "drawing.h"
#include "entity.h"
#include "main.h"
#include "rand.h"
#include "title.h"

/* Function prototypes */
static void	draw_point(struct game *cur_game, int x, int y, unsigned char col[3]);
static void	draw_player(struct game *cur_game);
static void	draw_goodies(struct game *cur_game);
static void	draw_baddies(struct game *cur_game);
static void	draw_score(struct game *cur_game);
static void	draw_hp(struct game *cur_game);
static void	draw_timer(struct game *cur_game);
static void	draw_bonus(struct game *cur_game);
static void	draw_level(struct game *cur_game);
static void	draw_transition(struct game *cur_game);

/* Sprites */
const char *sprites[] = {
	"0000010000100000000010000001000000001000000100000000010000100000011000111100011010010001100010010000100110010000000011111111000000001111111100000000100110010000100100011000100101100011110001100000010000100000000010000001000000001000000100000000010000100000",
	"0000000110000000000000100100000000000100001000000000010000100000000000111100000000110001100011000100100110010010100011111111000110001111111100010100100110010010001100011000110000000011110000000000010000100000000001000010000000000010010000000000000110000000",
	"0000000000000000000000000000000000000000000000000000000000000000000000111100000000000110011000000000111001110000000010011001000000001001100100000000111001110000000001100110000000000011110000000000000000000000000000000000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000000000000000000000000000000000111100000000000101101000000000100110010000000011111111000000001111111100000000100110010000000001011010000000000011110000000000000000000000000000000000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000000000000000000000000000000001100110000000001111111100000001111111111000000111111111100000011111111110000000111111110000000001111110000000000011110000000000000110000000000000000000000000000000000000000000000000000000",
	"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000110011000000000111111110000000011111111000000001111111100000000011111100000000000111100000000000001100000000000000000000000000000000000000000000000000000000000000000000000"
};
const char *numbers[] = {
	"0111001010010100101001110","0010000100001000010000100","0111000010011100100001110",
	"0111000010011100001001110","0101001010011100001000010","0111001000011100001001110",
	"0111001000011100101001110","0111000010000100001000010","0111001010011100101001110",
	"0111001010011100001000010"
};
const char *letters[] = {
	"0111001010011100101001010","0111001010011000101001110","0111001000010000100001110",
	"0110001010010100101001100","0111001000011100100001110","0111001000011100100001000",
	"0111001000010100101001110","0101001010011100101001010","0111000100001000010001110",
	"0001000010000100101001110","0101001010011000101001010","0100001000010000100001110",
	"1000111011101011000110001","0100101101011110101101001","0111001010010100101001110",
	"0111001010011100100001000","0111001010010100101001111","0111001010011000101001010",
	"0111001000011100001001110","0111000100001000010000100","0101001010010100101001110",
	"0101001010010100101000100","1000110001101011010101010","0101001010001000101001010",
	"0101001010001000010000100","0111000010001000100001110","0000000000000000000000000"
};
const char *symbols[] = {
	"0101011111111110111000100","0101010101100010101000100","0111010101111111010101110"
};

static void
draw_point(struct game *cur_game, int x, int y, unsigned char col[3])
{
	SDL_SetRenderDrawColor(cur_game->display.renderer, col[0], 0, 0, 255);
	SDL_RenderDrawLine(cur_game->display.renderer, x * 3 + 0, y * 3, x * 3 + 0, y * 3 + 2);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 0, col[1], 0, 255);
	SDL_RenderDrawLine(cur_game->display.renderer, x * 3 + 1, y * 3, x * 3 + 1, y * 3 + 2);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 0, 0, col[2], 255);
	SDL_RenderDrawLine(cur_game->display.renderer, x * 3 + 2, y * 3, x * 3 + 2, y * 3 + 2);
	/*SDL_SetRenderDrawColor(cur_game->display.renderer, col[0], col[1], col[2], 255);
	SDL_RenderDrawPoint(cur_game->display.renderer, x, y);*/
}

void
draw_world(struct game *cur_game)
{
	SDL_Rect src = { 0, 0, 1280, 720 };
	SDL_Rect dest = { 0, 0, cur_game->display.w, cur_game->display.h };
	int i;
	
	if (cur_game->display.renderer == NULL) printf("uh oh\n");
	/* Render to output texture */
	SDL_SetRenderTarget(cur_game->display.renderer, cur_game->display.output);
	SDL_SetRenderDrawColor(cur_game->display.renderer, 10, 10, 10, 255);
	SDL_RenderClear(cur_game->display.renderer);
	/* Draw stars */
	for (i = 0; i < cur_game->num_stars; i += 1) {
		new_color(cur_game->stars[i].col);
		draw_point(cur_game, cur_game->stars[i].x, cur_game->stars[i].y, cur_game->stars[i].col);
	}
	/* If you're in the right context, draw player, goodies, baddies, and score */
	if (cur_game->game.state == ACTIVE ||
	    cur_game->game.state == GAMEOVER ||
	    cur_game->game.state == TRANSITION) {
		draw_player(cur_game);
		draw_goodies(cur_game);
		draw_baddies(cur_game);
		draw_score(cur_game);
		draw_hp(cur_game);
		draw_timer(cur_game);
		draw_bonus(cur_game);
		draw_level(cur_game);
		draw_transition(cur_game);
	} else if (cur_game->game.state == TITLE || cur_game->game.state == OPTIONS ||
		   cur_game->game.state == HOWTOPLAY || cur_game->game.state == HIGHSCORE ||
		   cur_game->game.state == CREDITS) {
		draw_title(cur_game);
	}
	/* Game over? */
	if (cur_game->game.state == GAMEOVER) {
		draw_text(cur_game, "         GAME OVER", 145, 63);
		draw_text(cur_game, "PRESS SPACEBAR FOR NEW GAME", 145, 70);
		draw_text(cur_game, "    OR ESCAPE FOR TITLE", 145, 77);
		draw_highscores(cur_game);
	}
	/* Draw scanlines */
	//SDL_SetRenderDrawColor(cur_game->display.renderer, 0, 0, 0, 128);
	//for (i = 2; i < 720; i += 3) {
	//	SDL_RenderDrawLine(cur_game->display.renderer, 0, i, 1280, i);
	//}
	/* Reset rendering target */
	SDL_SetRenderTarget(cur_game->display.renderer, NULL);
	/* Display output texture */
	SDL_SetRenderDrawColor(cur_game->display.renderer, 0, 0, 0, 255);
	SDL_RenderClear(cur_game->display.renderer);
	SDL_RenderCopy(cur_game->display.renderer, cur_game->display.output, &src, &dest);
	SDL_RenderPresent(cur_game->display.renderer);
}

static void
draw_player(struct game *cur_game)
{
	int x, y;
	
	/* Change color */
	new_color(cur_game->player.col);
	/* Draw player */
	for (x = 0; x < 16; x += 1) {
		for (y = 0; y < 16; y += 1) {
			if (sprites[cur_game->player.sprite][y * 16 + x] == '1') {
				draw_point(cur_game, cur_game->player.x + x, cur_game->player.y + y, cur_game->player.col);
			}
		}
	}
}

void
animate_world(struct game *cur_game)
{
	int i;
	
	for (i = 0; i < cur_game->num_stars; i++) {
		cur_game->stars[i].x -= cur_game->stars[i].delta;
		if (cur_game->stars[i].x < 0) {
			cur_game->stars[i].x = 1280/3;
			cur_game->stars[i].y = rand_num(0, 720/3);
			cur_game->stars[i].col[0] = rand_num(128, 255);
			cur_game->stars[i].col[1] = rand_num(128, 255);
			cur_game->stars[i].col[2] = rand_num(128, 255);
			if (rand_num(0, 99) > 50) {
				cur_game->stars[i].delta = 1;
			} else if (rand_num(0, 99) > 50) {
				cur_game->stars[i].delta = 2;
			} else {
				cur_game->stars[i].delta = 3;
			}
		}
	}
	if (cur_game->game.state == ACTIVE) {
		move_player(cur_game);
		move_goodies(cur_game);
		move_baddies(cur_game);
		check_collisions(cur_game);
	}
}

static void
draw_goodies(struct game *cur_game)
{
	int i;
	int x, y;
	
	for (i = 0; i < cur_game->num_goodies; i += 1) {
		/* Draw current goodie if visible */
		if (cur_game->goodies[i].visible == SDL_FALSE) continue;
		/* change color */
		new_color(cur_game->goodies[i].col);
		for (x = 0; x < 16; x += 1) {
			for (y = 0; y < 16; y += 1) {
				if (sprites[cur_game->goodies[i].sprite][y * 16 + x] == '1') {
					draw_point(cur_game, cur_game->goodies[i].coords.x + x, cur_game->goodies[i].coords.y + y, cur_game->goodies[i].col);
				}
			}
		}
	}
}

static void
draw_baddies(struct game *cur_game)
{
	int i;
	int x, y;
	
	for (i = 0; i < cur_game->num_baddies; i += 1) {
		/* Draw current baddie if visible */
		if (cur_game->baddies[i].visible == SDL_FALSE) continue;
		/* change color */
		new_color(cur_game->baddies[i].col);
		for (x = 0; x < 16; x += 1) {
			for (y = 0; y < 16; y += 1) {
				if (sprites[cur_game->baddies[i].sprite][y * 16 + x] == '1') {
					draw_point(cur_game, cur_game->baddies[i].coords.x + x, cur_game->baddies[i].coords.y + y, cur_game->baddies[i].col);
				}
			}
		}
	}
}

static void
draw_score(struct game *cur_game)
{
	char score[16];
	
	/* Format score with leading 0s */
	sprintf(score, "SCORE[%09d", cur_game->player.score);
	/* Print out in upper corner */
	draw_text(cur_game, score, 346, 5);
}

static void
draw_hp(struct game *cur_game)
{
	int i, j, k;
	int sprite;
	unsigned char col[3] = { 255, 255, 255 };
	
	for (i = 0; i < 10; i += 1) {
		if (cur_game->player.hp > i) {
			sprite = 0;
		} else {
			sprite = 1;
		}
		for (j = 0; j < 5; j += 1) {
			for (k = 0; k < 5; k += 1) {
				if (symbols[sprite][k * 5 + j] == '1') {
					draw_point(cur_game, 5 + i * 7 + j, 5 + k, col);
				}
			}
		}
	}
}

static void
draw_timer(struct game *cur_game)
{
	int i, j;
	char timer[3];
	unsigned char col[3] = { 255, 255, 255 };
	
	/* Reduce timer and format */
	if (cur_game->game.state == ACTIVE) cur_game->game.round_timer -= 1;
	sprintf(timer, "%02d", cur_game->game.round_timer/40);
	for (i = 0; i < 5; i += 1) {
		for (j = 0; j < 5; j += 1) {
			if (numbers[timer[0] - 48][j * 5 + i] == '1') {
				draw_point(cur_game, 208 + i, 5 + j, col);
			}
			if (numbers[timer[1] - 48][j * 5 + i] == '1') {
				draw_point(cur_game, 213 + i, 5 + j, col);
			}
		}
	}
}

static void
draw_bonus(struct game *cur_game)
{
	char bonus[12];
	
	/* Format score with leading 0s */
	sprintf(bonus, "BONUS[%05d", cur_game->game.bonus);
	/* Print out in bottom corner */
	draw_text(cur_game, bonus, 366, 230);
}

static void
draw_level(struct game *cur_game)
{
	char level[10];
	
	sprintf(level, "LEVEL[%d", cur_game->game.level + 1);
	draw_text(cur_game, level, 5, 230);
}

void
draw_text(struct game *cur_game, char *text, int x, int y)
{
	int i, j, k;
	unsigned char col[3] = { 255, 255, 255 };
	const char *cur_letter;
	
	for (i = 0; text[i] != '\0'; i += 1) {
		if (text[i] == ' ') {
			continue;
		} else if (text[i] == '*') {
			cur_letter = symbols[0];
		} else if (text[i] == '{') {
			cur_letter = symbols[2];
		} else if (text[i] >= '0' && text[i] <= '9') {
			cur_letter = numbers[text[i] - 48];
		} else {
			cur_letter = letters[text[i] - 65];
		}
		for (j = 0; j < 5; j += 1) {
			for (k = 0; k < 5; k += 1) {
				if (cur_letter[k * 5 + j] == '1') {
					draw_point(cur_game, x + i * 5 + j, y + k, col);
				}
			}
		}
	}
}

static void
draw_transition(struct game *cur_game)
{
	SDL_Rect coords;
	char level[10];
	
	/* If in a transition state, draw a shrinking rectangle over everything */
	if (cur_game->game.state != TRANSITION) return;
	coords.w = cur_game->game.percent * 1280;
	coords.h = cur_game->game.percent * 720;
	coords.x = 1280 / 2 - coords.w / 2;
	coords.y = 720 / 2 - coords.h / 2;
	SDL_SetRenderDrawColor(cur_game->display.renderer, 40, 40, 40, 255);
	SDL_RenderFillRect(cur_game->display.renderer, &coords);
	if (cur_game->game.percent >= 0.09) {
		sprintf(level, "LEVEL[%d", cur_game->game.level + 1);
		draw_text(cur_game, level, 195, 117);
	}
	cur_game->game.percent -= 0.01;
	if (cur_game->game.percent <= 0) {
		cur_game->game.percent = 1;
		cur_game->game.state = ACTIVE;
	}	
}
