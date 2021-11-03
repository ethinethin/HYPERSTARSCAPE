#ifndef DRAWING_H
#define DRAWING_H

#include "main.h"

void	draw_world(struct game *cur_game);
void	animate_world(struct game *cur_game);
void	draw_text(struct game *cur_game, char *text, int x, int y);

#endif
