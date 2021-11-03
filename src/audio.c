#include <stdlib.h>
#include <SDL2/SDL.h>
#include "audio.h"
#include "main.h"
#include "rand.h"

struct song {
	SDL_AudioSpec spec;
	Uint8 *buf;
	Uint32 len;
	SDL_AudioDeviceID devid;
	Uint8 *cur_buf;
	Uint32 cur_len;
} *songs;

struct song *enemy_sounds;
struct song *heart_sounds;
int music;
int sound;

/* Function prototypes */
static void	load_music(void);
static void	unload_music(void);
static void	load_sounds(void);
static void	unload_sounds(void);
static void	callback_music(void *userdata, Uint8 *stream, int len);
static void	callback_sound(void *userdata, Uint8 *stream, int len);

int cur_song_num;
void
init_audio(struct game *cur_game)
{
	/* Load music and sounds */
	load_music();
	load_sounds();
	/* Update sound settings */
	sound_settings(cur_game);
	/* Play first song */
	cur_song_num = 1;
	SDL_PauseAudioDevice(songs[cur_song_num].devid, 0);
}

void
kill_audio(void)
{
	unload_music();
	unload_sounds();
}

static void
load_music(void)
{
	int i;
	char *names[] = { "./data/song1.wav", "./data/song2.wav" };
	
	songs = malloc(sizeof(*songs) * 2);
	for (i = 0; i < 2; i += 1) {
		SDL_LoadWAV(names[i], &songs[i].spec, &songs[i].buf, &songs[i].len);
		songs[i].spec.callback = callback_music;
		songs[i].spec.userdata = &songs[i];
		songs[i].devid = SDL_OpenAudioDevice(NULL, 0, &songs[i].spec, NULL, 0);
		songs[i].cur_buf = songs[i].buf;
		songs[i].cur_len = songs[i].len;
	}
}

static void
unload_music(void)
{
	SDL_CloseAudioDevice(songs[0].devid);
	SDL_FreeWAV(songs[0].buf);
	SDL_CloseAudioDevice(songs[1].devid);
	SDL_FreeWAV(songs[1].buf);
	free(songs);
}

static void
load_sounds(void)
{
	int i;
	char *names[] = { "./data/enemies01.wav", "./data/enemies02.wav", "./data/enemies03.wav", 
			  "./data/enemies04.wav", "./data/enemies05.wav" };
	char *names_hearts[] = { "./data/hearts01.wav", "./data/hearts02.wav", "./data/hearts03.wav",
				 "./data/hearts04.wav", "./data/hearts05.wav" };
				 
	/* Allocate memory */
	enemy_sounds = malloc(sizeof(*enemy_sounds) * 5);
	heart_sounds = malloc(sizeof(*heart_sounds) * 5);
	for (i = 0; i < 5; i += 1) {
		/* Load enemy sounds */
		SDL_LoadWAV(names[i], &enemy_sounds[i].spec, &enemy_sounds[i].buf, &enemy_sounds[i].len);
		enemy_sounds[i].spec.callback = callback_sound;
		enemy_sounds[i].spec.userdata = &enemy_sounds[i];
		enemy_sounds[i].devid = SDL_OpenAudioDevice(NULL, 0, &enemy_sounds[i].spec, NULL, 0);
		enemy_sounds[i].cur_buf = enemy_sounds[i].buf;
		enemy_sounds[i].cur_len = enemy_sounds[i].len;
		/* Load heart sounds */
		SDL_LoadWAV(names_hearts[i], &heart_sounds[i].spec, &heart_sounds[i].buf, &heart_sounds[i].len);
		heart_sounds[i].spec.callback = callback_sound;
		heart_sounds[i].spec.userdata = &heart_sounds[i];
		heart_sounds[i].devid = SDL_OpenAudioDevice(NULL, 0, &heart_sounds[i].spec, NULL, 0);
		heart_sounds[i].cur_buf = heart_sounds[i].buf;
		heart_sounds[i].cur_len = heart_sounds[i].len;
	}
}

static void
unload_sounds(void)
{
	int i;
	
	for (i = 0; i < 5; i += 1) {
		SDL_CloseAudioDevice(enemy_sounds[i].devid);
		SDL_FreeWAV(enemy_sounds[i].buf);
		SDL_CloseAudioDevice(heart_sounds[i].devid);
		SDL_FreeWAV(heart_sounds[i].buf);
	}
	free(enemy_sounds);
	free(heart_sounds);
}

static void
callback_music(void *userdata, Uint8 *stream, int len)
{
	struct song *cur_song;
	
	cur_song = (struct song *) userdata;
	/* Loop if you have to */
	if (cur_song->cur_len == 0) {
		cur_song->cur_len = cur_song->len;
		cur_song->cur_buf = cur_song->buf;
	}
	/* Check len, make sure it's okay */
	if (len > (int) cur_song->cur_len) {
		len = (int) cur_song->cur_len;
	}
	/* Play silence */
	SDL_memset(stream, 0, len);
	/* Play current song position if music is on */
	if (music == MUSIC_OFF) return;
	SDL_MixAudioFormat(stream, cur_song->cur_buf, cur_song->spec.format, len, SDL_MIX_MAXVOLUME);
	/* Move the buffer forward */
	cur_song->cur_buf += len;
	cur_song->cur_len -= len;
}

static void
callback_sound(void *userdata, Uint8 *stream, int len)
{
	struct song *cur_sound;
	
	cur_sound = (struct song *) userdata;
	/* Reset sound if you have to */
	if (cur_sound->cur_len == 0) {
		cur_sound->cur_len = cur_sound->len;
		cur_sound->cur_buf = cur_sound->buf;
		SDL_PauseAudioDevice(cur_sound->devid, 1);
		return;
	}
	/* Check len, make sure it's okay */
	if (len > (int) cur_sound->cur_len) {
		len = (int) cur_sound->cur_len;
	}
	/* Play silence */
	SDL_memset(stream, 0, len);
	/* Play current sound position */
	SDL_MixAudioFormat(stream, cur_sound->cur_buf, cur_sound->spec.format, len, SDL_MIX_MAXVOLUME);
	/* Move the buffer forward */
	cur_sound->cur_buf += len;
	cur_sound->cur_len -= len;
}

void
swap_songs(int song_num)
{
	int next;

	/* Do you need to change songs? */
	if (song_num == cur_song_num) return;	
	/* Pause current song */
	SDL_PauseAudioDevice(songs[cur_song_num].devid, 1);
	next = cur_song_num == 0 ? 1 : 0;
	/* Update buffer */
	songs[next].cur_len = songs[cur_song_num].cur_len;
	/* In case songs are of different length, account for it */
	if (songs[next].cur_len > songs[next].len) songs[next].cur_len = songs[next].len;
	songs[next].cur_buf = songs[next].buf + (int) (songs[next].len - songs[next].cur_len);
	cur_song_num = next;
	SDL_PauseAudioDevice(songs[cur_song_num].devid, 0);
}

void
play_sound(SDL_bool isenemy)
{
	int rando;
	struct song *cur_sound;
	
	/* Don't try to play the sound if the sound is turned off */
	if (sound == SOUND_OFF) return;
	/* Playing enemy or heart sound? */
	cur_sound = isenemy ? enemy_sounds : heart_sounds;
	/* Generate a random number to play one of the 5 sounds */
	while (SDL_TRUE) {
		rando = rand_num(0, 4);
		/* Check if current sound is being played, break if not */
		if (cur_sound[rando].len == cur_sound[rando].cur_len) break;
	}
	/* Play it */
	SDL_PauseAudioDevice(cur_sound[rando].devid, 0);
}

void
sound_settings(struct game *cur_game)
{
	music = cur_game->game.music;
	sound = cur_game->game.sound;
}
