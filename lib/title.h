#ifndef TITLE_H
#define TITLE_H

#include "main.h"

void	mouse_click(struct game *cur_game, int x, int y);
void	load_highscores(void);
void	point_to_highscores(void);
void	kill_highscores(void);
void	check_if_highscore(struct game *cur_game);
void	draw_highscores(struct game *cur_game);
void	change_display(struct game *cur_game);
void	save_display(struct game *cur_game);
void	load_display(struct game *cur_game);
void	point_to_display(struct game *cur_game);
void	draw_title(struct game *cur_game);

#endif
