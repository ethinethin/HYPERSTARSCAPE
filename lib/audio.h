#ifndef AUDIO_H
#define AUDIO_H

#include "main.h"

void	init_audio(struct game *cur_game);
void	kill_audio(void);
void	swap_songs(int song_num);
void	play_sound(SDL_bool isenemy);
void	sound_settings(struct game *cur_game);

#endif
