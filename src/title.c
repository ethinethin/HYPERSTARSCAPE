#include <stdio.h>
#include <stdlib.h>
#include "audio.h"
#include "drawing.h"
#include "main.h"
#include "title.h"

static void	save_highscores(void);

struct title_features {
	SDL_Rect coords;
	char *text;
	int state;
	int new_state;
} features[] = {
	/* Do not move highscores values down */
	{ { 5, 5, 55, 5 }, "HIGH SCORES", HIGHSCORE, HIGHSCORE },
	{ { 10, 26, 100, 5 }, NULL, HIGHSCORE, HIGHSCORE },
	{ { 10, 33, 100, 5 }, NULL, HIGHSCORE, HIGHSCORE },
	{ { 10, 40, 100, 5 }, NULL, HIGHSCORE, HIGHSCORE },
	{ { 10, 47, 100, 5 }, NULL, HIGHSCORE, HIGHSCORE },
	{ { 10, 54, 100, 5 }, NULL, HIGHSCORE, HIGHSCORE },
	{ { 10, 19, 100, 5 }, "       SCORE     LEVEL", HIGHSCORE, HIGHSCORE },
	{ { 5, 230, 65, 5 }, "BACK TO TITLE", HIGHSCORE, TITLE },
	/* Do not move options values down */
	{ { 5, 5, 35, 5 }, "OPTIONS", OPTIONS, OPTIONS },
	{ { 10, 19, 35, 5 }, "DISPLAY MODE", OPTIONS, OPTIONS },
	{ { 20, 26, 40, 5 }, NULL, OPTIONS, DISP_WINDOW },
	{ { 20, 33, 100, 5 }, NULL, OPTIONS, DISP_FULLSCREEN },
	{ { 20, 40, 95, 5 }, NULL, OPTIONS, DISP_FULLSCREEN_RES },
	{ { 10, 54, 35, 5 }, "VSYNC", OPTIONS, OPTIONS },
	{ { 20, 61, 20, 5 }, NULL, OPTIONS, DISP_VSYNC_ON },
	{ { 20, 68, 25, 5 }, NULL, OPTIONS, DISP_VSYNC_OFF },
	{ { 10, 82, 35, 5 }, "RESOLUTION", OPTIONS, OPTIONS },
	{ { 20, 89, 50, 5 },  NULL, OPTIONS, DISP_1280X720 },
	{ { 85, 89, 50, 5 },  NULL, OPTIONS, DISP_1360X768 },
	{ { 20, 96, 50, 5 },  NULL, OPTIONS, DISP_1366X768 },
	{ { 85, 96, 50, 5 },  NULL, OPTIONS, DISP_1440X900 },
	{ { 20, 103, 50, 5 }, NULL, OPTIONS, DISP_1600X900 },
	{ { 85, 103, 55, 5 }, NULL, OPTIONS, DISP_1600X1080 },
	{ { 20, 110, 55, 5 }, NULL, OPTIONS, DISP_1680X1050 },
	{ { 85, 110, 55, 5 }, NULL, OPTIONS, DISP_1920X1080 },
	{ { 20, 117, 55, 5 }, NULL, OPTIONS, DISP_1920X1200 },
	{ { 85, 117, 55, 5 }, NULL, OPTIONS, DISP_2560X1440 },
	{ { 20, 124, 55, 5 }, NULL, OPTIONS, DISP_2560X1600 },
	{ { 85, 124, 55, 5 }, NULL, OPTIONS, DISP_3840X2160 },
	{ { 10, 138, 35, 5 }, "MUSIC", OPTIONS, OPTIONS },
	{ { 20, 145, 20, 5 }, NULL, OPTIONS, MUSIC_ON },
	{ { 20, 152, 25, 5 }, NULL, OPTIONS, MUSIC_OFF },
	{ { 10, 166, 35, 5 }, "SOUND", OPTIONS, OPTIONS },
	{ { 20, 173, 20, 5 }, NULL, OPTIONS, SOUND_ON },
	{ { 20, 180, 25, 5 }, NULL, OPTIONS, SOUND_OFF },
	{ { 5, 230, 65, 5 }, "BACK TO TITLE", OPTIONS, TITLE },
	/* Anything below here can move however you need it */
	{ { 5, 5, 75, 5 }, "HYPER STARSCAPE", TITLE, TITLE },
	{ { 10, 19, 40, 5 }, "NEW GAME", TITLE, NEWGAME },
	{ { 10, 26, 40, 5 }, "CONTINUE", TITLE, CONTINUE },
	{ { 10, 33, 35, 5 }, "OPTIONS", TITLE, OPTIONS },
	{ { 10, 40, 55, 5 }, "HIGH SCORES", TITLE, HIGHSCORE },
	{ { 10, 47, 55, 5 }, "HOW TO PLAY", TITLE, HOWTOPLAY },
	{ { 10, 54, 35, 5 }, "CREDITS", TITLE, CREDITS },
	{ { 5, 230, 20, 5 }, "EXIT", TITLE, EXITGAME },
	{ { 5, 5, 55, 5 }, "HOW TO PLAY", HOWTOPLAY, HOWTOPLAY },
	{ { 10, 19, 1, 1 }, "USE WASD OR ARROW KEYS TO MOVE AROUND", HOWTOPLAY, HOWTOPLAY },
	{ { 10, 33, 1, 1 }, "SURVIVE EACH ROUND BY AVOIDING THE { ENEMIES", HOWTOPLAY, HOWTOPLAY },
	{ { 10, 47, 1, 1 }, "COLLECT * HEARTS TO EARN POINTS AND RESTORE HEALTH", HOWTOPLAY, HOWTOPLAY },
	{ { 10, 61, 1, 1 }, "BONUS POINTS ARE GIVEN AT THE END OF EACH ROUND", HOWTOPLAY, HOWTOPLAY },
	{ { 10, 75, 1, 1 }, "BONUS POINTS AMOUNT IS REDUCED BY { ENEMY HITS", HOWTOPLAY, HOWTOPLAY },
	{ { 5, 230, 65, 5 }, "BACK TO TITLE", HOWTOPLAY, TITLE },
	{ { 5, 5, 75, 5 }, "CREDITS", CREDITS, CREDITS },
	{ { 10, 19, 1, 1 }, "THIS GAME WAS CREATED BY ETHINETHIN", CREDITS, CREDITS },
	{ { 10, 26, 1, 1 }, "FIND ME ON TWITCH FOR GAME DEV STREAMS *", CREDITS, CREDITS },
	{ { 5, 230, 65, 5 }, "BACK TO TITLE", CREDITS, TITLE },
	{ { -1, -1, -1, -1 }, NULL, NOSTATE, NOSTATE }
};

struct score {
	int num;
	unsigned int score;
	int level;
} highscores[5];

void
mouse_click(struct game *cur_game, int x, int y)
{
	int i;
	
	for (i = 0; features[i].coords.x != -1; i += 1) {
		if (cur_game->game.state == features[i].state &&
		    x >= features[i].coords.x &&
		    x <= features[i].coords.x + features[i].coords.w &&
		    y >= features[i].coords.y &&
		    y <= features[i].coords.y + features[i].coords.h) {
			cur_game->game.state = features[i].new_state;
			return;
		}
	}
}

void
load_highscores(void)
{
	char buffer[200];
	char decrypt[10];
	int i;
	int j;
	FILE *fp;
	
	/* Load encrypted data from file */
	fp = fopen("highscores.dat", "r");
	if (fp != NULL) {
		/* Decrypt data as it comes in */
		for (i = 0; (buffer[i] = fgetc(fp)) != EOF; i += 1) {
			buffer[i] += 47;
		}
		buffer[i] = '\0';
		fclose(fp);
		/* Put these values into the appropriate variables */
		for (i = 0; i < 5; i += 1) {
			decrypt[0] = buffer[i * 13 + 0]; decrypt[1] = '\0';
			highscores[i].num = atoi(decrypt);
			for (j = 0; j < 9; j += 1) decrypt[j] = buffer[i * 13 + j + 1]; decrypt[j] = '\0';
			highscores[i].score = atoi(decrypt);
			for (j = 0; j < 3; j += 1) decrypt[j] = buffer[i * 13 + j + 10]; decrypt[j] = '\0';
			highscores[i].level = atoi(decrypt);
		}
	} else {
		/* Save data does not exist, create it and save */
		for (i = 0; i < 5; i += 1) {
			highscores[i].num = i + 1;
			highscores[i].score = 0;
			highscores[i].level = 1;
		}
		save_highscores();
	}
}

void
point_to_highscores(void)
{
	int i;
	/* We'll use this feature to reformat the highscores strings */
	if (features[1].text != NULL) kill_highscores();
	for (i = 1; i < 6; i += 1) {
		features[i].text = malloc(sizeof(*features[i].text) * 25);
		sprintf(features[i].text, "%d    %9d    %3d", i, highscores[i - 1].score, highscores[i - 1].level);
	}
	/* Use this opportunity to save highscores */
	save_highscores();
}

void
kill_highscores(void)
{
	int i;
	
	for (i = 1; i < 6; i += 1) {
		if (features[i].text != NULL) {
			free(features[i].text);
		}
	}
}

static void
save_highscores(void)
{
	char buffer[200];
	int i;
	FILE *fp;

	/* Write to a buffer */
	for (i = 0, buffer[0] = '\0'; i < 5; i += 1) {	
		sprintf(buffer, "%s%d%09d%03d", buffer, highscores[i].num, highscores[i].score, highscores[i].level);
	}
	/* Encrypt by subtracting 47 from the ascii value */
	for (i = 0; buffer[i] != '\0'; i += 1) {
		buffer[i] -= 47;
	}
	/* Write out to a file */
	fp = fopen("highscores.dat", "w");
	if (fp == NULL) {
		printf("Cannot create highscores.dat!\n");
		exit(1);
	}
	fprintf(fp, "%s", buffer);
	fclose(fp);
}

void
check_if_highscore(struct game *cur_game)
{
	int i, j;
	
	for (i = 0; i < 5; i += 1) {
		if (cur_game->player.score > highscores[i].score ||
		    (cur_game->player.score == highscores[i].score &&
		    (cur_game->game.level + 1) > highscores[i].level)) {
		    	for (j = 4; j > i; j -= 1) {
		    		highscores[j].score = highscores[j - 1].score;
		    		highscores[j].level = highscores[j - 1].level;
		    	}
		    	highscores[i].score = cur_game->player.score;
		    	highscores[i].level = cur_game->game.level + 1;
		    	break;
		}
	}
	point_to_highscores();
}

void
draw_highscores(struct game *cur_game)
{
	int i;
	
	draw_text(cur_game, features[0].text, features[0].coords.x + 180, features[0].coords.y + 91);
	for (i = 1; i < 7; i += 1) {
		draw_text(cur_game, features[i].text, features[i].coords.x + 145, features[i].coords.y + 91);
	}
}

void
change_display(struct game *cur_game)
{
	SDL_bool changed = SDL_FALSE;
	int w = cur_game->display.w;
	int h = cur_game->display.h;
	
	switch (cur_game->game.state) {
	case DISP_WINDOW:
	case DISP_FULLSCREEN:
	case DISP_FULLSCREEN_RES:
		/* Is user changing display mode? */
		if (cur_game->display.mode != cur_game->game.state) {
			cur_game->display.mode = cur_game->game.state; 
			changed = SDL_TRUE;
		}
		break;
	case DISP_VSYNC_ON:
	case DISP_VSYNC_OFF:
		/* Is user changing vsync? */
		if (cur_game->display.vsync != cur_game->game.state) {
			cur_game->display.vsync = cur_game->game.state;
			changed = SDL_TRUE;
		}
		break;
	case MUSIC_ON:
	case MUSIC_OFF:
		if (cur_game->game.music != cur_game->game.state) {
			cur_game->game.music = cur_game->game.state;
			point_to_display(cur_game);
			sound_settings(cur_game);
			save_display(cur_game);
		}
		break;
	case SOUND_ON:
	case SOUND_OFF:
		if (cur_game->game.sound != cur_game->game.state) {
			cur_game->game.sound = cur_game->game.state;
			point_to_display(cur_game);
			sound_settings(cur_game);
			save_display(cur_game);
		}
		break;
	case DISP_1280X720:
		w = 1280; h = 720;
		break;
	case DISP_1360X768:
		w = 1360; h = 768;
		break;
	case DISP_1366X768:
		w = 1366; h = 768;
		break;
	case DISP_1440X900:
		w = 1440; h = 900;
		break;
	case DISP_1600X900:
		w = 1600; h = 900;
		break;
	case DISP_1600X1080:
		w = 1600; h = 1080;
		break;
	case DISP_1680X1050:
		w = 1680; h = 1050;
		break;
	case DISP_1920X1080:
		w = 1920; h = 1080;
		break;
	case DISP_1920X1200:
		w = 1920; h = 1200;
		break;
	case DISP_2560X1440:
		w = 2560; h = 1440;
		break;
	case DISP_2560X1600:
		w = 2560; h = 1600;
		break;
	case DISP_3840X2160:
		w = 3840; h = 2160;
		break;
	}
	/* Does the user want to change resolution? */
	if (w != cur_game->display.w || h != cur_game->display.h) {
		cur_game->display.w = w;
		cur_game->display.h = h;
		cur_game->display.res_code = cur_game->game.state;
		/* If they're in fullscreen and changing res, you need ot go to FULLSCREEN CUSTOM */
		if (cur_game->display.mode == DISP_FULLSCREEN) cur_game->display.mode = DISP_FULLSCREEN_RES;
		changed = SDL_TRUE;
	}
	/* Have display settings changed? */
	if (changed == SDL_TRUE) {
		cur_game->game.state = REFRESH_DISPLAY;
	} else {
		cur_game->game.state = OPTIONS;
	}
}

void
save_display(struct game *cur_game)
{
	FILE *fp;
	
	fp = fopen("options.ini", "w");
	if (fp == NULL) {
		printf("Can't create options.ini!\n");
		exit(1);
	} else {
		fprintf(fp, "w=%d\n", cur_game->display.w);
		fprintf(fp, "h=%d\n", cur_game->display.h);
		fprintf(fp, "mode=%d\n", cur_game->display.mode);
		fprintf(fp, "vsync=%d\n", cur_game->display.vsync);
		fprintf(fp, "code=%d\n", cur_game->display.res_code);
		fprintf(fp, "music=%d\n", cur_game->game.music);
		fprintf(fp, "sound=%d\n", cur_game->game.sound);
	}
	fclose(fp);
}

void
load_display(struct game *cur_game)
{
	FILE *fp;
	fp = fopen("options.ini", "r");
	if (fp == NULL) {
		cur_game->display.w = 1280;
		cur_game->display.h = 720;
		cur_game->display.mode = DISP_WINDOW;
		cur_game->display.vsync = DISP_VSYNC_OFF;
		cur_game->display.res_code = DISP_1280X720;
	} else {
		fscanf(fp, "w=%d\n", &cur_game->display.w);
		fscanf(fp, "h=%d\n", &cur_game->display.h);
		fscanf(fp, "mode=%d\n", &cur_game->display.mode);
		fscanf(fp, "vsync=%d\n", &cur_game->display.vsync);
		fscanf(fp, "code=%d\n", &cur_game->display.res_code);
		fscanf(fp, "music=%d\n", &cur_game->game.music);
		fscanf(fp, "sound=%d\n", &cur_game->game.sound);
	}
}

void
point_to_display(struct game *cur_game)
{
	int i;
	int feature_num[] = { 10, 11, 12, 14, 15, 30, 31, 33, 34, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, -1 };
	static char *feat_unselected[] = { "  WINDOW","  FULLSCREEN DEFAULT","  FULLSCREEN CUSTOM",
					   "  ON","  OFF", "  ON", "  OFF", "  ON", "  OFF",
					   "  1280X720","  1360X768","  1366X768","  1440X900","  1600X900","  1600X1080",
					   "  1680X1050","  1920X1080","  1920X1200","  2560X1440","  2560X1600","  3840X2160"};
	static char *feat_selected[] = { "* WINDOW","* FULLSCREEN DEFAULT","* FULLSCREEN CUSTOM",
					 "* ON","* OFF", "* ON", "* OFF", "* ON", "* OFF",
					 "* 1280X720","* 1360X768","* 1366X768","* 1440X900","* 1600X900","  1600X1080",
					 "* 1680X1050","* 1920X1080","* 1920X1200","* 2560X1440","* 2560X1600","* 3840X2160"};

	for (i = 0; feature_num[i] != -1; i += 1) {
		if (i < 3) {
			if (cur_game->display.mode == features[feature_num[i]].new_state) {
				features[feature_num[i]].text = feat_selected[i];
			} else {
				features[feature_num[i]].text = feat_unselected[i];
			}
		} else if (i < 5) {
			if (cur_game->display.vsync == features[feature_num[i]].new_state) {
				features[feature_num[i]].text = feat_selected[i];
			} else {
				features[feature_num[i]].text = feat_unselected[i];
			}
		} else if (i < 7) {
			if (cur_game->game.music == features[feature_num[i]].new_state) {
				features[feature_num[i]].text = feat_selected[i];
			} else {
				features[feature_num[i]].text = feat_unselected[i];
			}
		} else if (i < 9) {
			if (cur_game->game.sound == features[feature_num[i]].new_state) {
				features[feature_num[i]].text = feat_selected[i];
			} else {
				features[feature_num[i]].text = feat_unselected[i];
			}
		} else {
			if (cur_game->display.res_code == features[feature_num[i]].new_state) {
				features[feature_num[i]].text = feat_selected[i];
			} else {
				features[feature_num[i]].text = feat_unselected[i];
			}
		}
	}
}

void
draw_title(struct game *cur_game)
{
	int i;
	
	for (i = 0; features[i].coords.x != -1; i += 1) {
		if (cur_game->game.state == features[i].state) {
			draw_text(cur_game, features[i].text, features[i].coords.x, features[i].coords.y);
		}
	}
}
