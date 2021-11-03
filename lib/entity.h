#ifndef ENTITY_H
#define ENTITY_H

#include "main.h"

void	new_color(unsigned char col[3]);
void	check_collisions(struct game *cur_game);
void	init_stars(struct game *cur_game, int num);
void	kill_stars(struct game *cur_game);
void	init_goodies(struct game *cur_game, int num);
void	kill_goodies(struct game *cur_game);
void	init_baddies(struct game *cur_game, int num);
void	kill_baddies(struct game *cur_game);
void	move_player(struct game *cur_game);
void	move_goodies(struct game *cur_game);
void	move_baddies(struct game *cur_game);

#endif
