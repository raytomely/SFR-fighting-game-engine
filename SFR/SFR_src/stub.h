#ifndef STUB_H
#define STUB_H

#include <SDL_mixer.h>

#define TRUE 1
#define FALSE 0

union REGS;
void stub1(int a, REGS *b, REGS *c);

#define gotoxy(a,b)
#define int86(a, b, c) stub1(a,b,c)
#define inp(x) x
#define outportb(a,b)
#define _farpokeb(a, b, c)
#define inportb(x) x
#define _go32_my_cs() 1
#define _go32_dpmi_get_protected_mode_interrupt_vector(a, b)
#define _go32_dpmi_allocate_iret_wrapper(x)
#define _go32_dpmi_set_protected_mode_interrupt_vector(a, b);

typedef struct
{
    int pm_offset;
    int pm_selector;
}_go32_dpmi_seginfo;


union REGS   // placeholder REGS union
{
    struct
    {
        int ax;
    }x;
};

typedef struct
{
   unsigned char *ptr ;
   unsigned char message ;
   unsigned char status  ;
   int size ;
   int timeStamp ;
   Mix_Music *mix_music;
   char name[20];
} MIDI;
extern MIDI* midi_ptr;

void stub2(MIDI *midi);

//#define load_midi(fn) midi_ptr
//#define play_midi(a, b) stub2(a)
//#define destroy_midi(bgmusic)

typedef struct {
    unsigned short    Period;
    unsigned short    Volume;
    unsigned int      Length;
    unsigned char     *Data;
    Mix_Chunk *mix_chunk;
    char name[20];
} SAMPLE;
extern SAMPLE* sample_ptr;

//#define load_sample(fn) sample_ptr
//#define play_sample(a, b, c, d, e)

#define set_volume(sfxvolume, midvolume)

#define allegro_init()
#define install_timer()
#define install_sound(a, b, c) 0


#endif // STUB_H
