#ifndef SFR_SDL_H
#define SFR_SDL_H

#include <SDL/SDL.h>
#include <SDL_mixer.h>

#include "stub.h"

void key_handler(void);
#define processKeyboardInput() key_handler()

#define delay(x) SDL_Delay(x)
//#define play_sound(chunk) Mix_PlayChannel(-1, chunk, 0);

extern SDL_Surface *screen;
extern SDL_Color SFRPalette[256];
extern SDL_Color SFRFadePalette[256];

void setSFRPalette(void);
void setBrightness(int factor);
SDL_Surface *create_surface(int width, int height);
void blit_raw(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys);
void blit_raw_flipped(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys, int flip);
void blit_scaled_X2(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys);
void intit_sfr_keys(void);
int read_keyboard(void);
void free_music(MIDI* midi_music);
MIDI* load_music(const char *filename);
void play_music(MIDI* midi_music);
SAMPLE* load_sound(const char *filename);
void play_sound(SAMPLE* sample);
void free_sound(SAMPLE* sample);
char *getFilePath(char *filename);
void load_config(void);
void SFR_SDL_init(void);
void SFR_SDL_quit(void);
void processInput(void);
void sleep(void);
void init_clock(int speed);
void main_event_loop(void);

#define load_midi(fn) load_music(fn)
#define play_midi(music, a) play_music(music)
#define destroy_midi(music) free_music(music); music = NULL
#define stop_midi() Mix_HaltMusic();

#define load_sample(fn) load_sound(fn)
#define play_sample(sample, a, b, c, d) play_sound(sample)
#define free_sample(sample) free_sound(sample); sample = NULL
#define stop_sample() Mix_HaltChannel(-1)

#define fopen(fn,mode) fopen(getFilePath((char*)fn),mode)

#endif // SFR_SDL_H
