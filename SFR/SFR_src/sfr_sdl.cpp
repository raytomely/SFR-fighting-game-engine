#include <SDL/SDL.h>
#include <SDL_mixer.h>
#include "keys.h"
#include "stub.h"

void convert_surface(SDL_Surface **surface)
{
    SDL_Surface *temp_surf = &(**surface);
    *surface = SDL_DisplayFormat (temp_surf);
    SDL_FreeSurface(temp_surf);
}

void convert_surface_alpha(SDL_Surface **surface)
{
    SDL_Surface *temp_surf = &(**surface);
    *surface = SDL_DisplayFormatAlpha(temp_surf);
    SDL_FreeSurface(temp_surf);
}

SDL_Surface *create_surface(int width, int height)
{
    SDL_Surface *surface;
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    convert_surface(&surface);
    return surface;
}

SDL_Surface *copy_surface(SDL_Surface *surface)
{
    return SDL_ConvertSurface(surface, surface->format, SDL_SWSURFACE);
}

int get_file_size(FILE* file)
{
    fseek (file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek (file, 0, SEEK_SET);
    return file_size;
}

int main_loop = 1;
SDL_Event event;
SDL_Surface *screen = NULL;
SDL_Color SFRPalette[256];
SDL_Color SFRFadePalette[256];
Uint8 alpha_color = 0;
Mix_Music *game_music;
char data_folder[512] = "SFRanma 2";
char file_path[1024];
int full_screen = 0;
int fps = 16;

unsigned char palette[768];

void setSFRPalette(void)
{
   int i, j;
   for (i=0, j=0 ; i < 256 ; i++, j+=3)
   {
       SFRPalette[i].r = palette[j];
       SFRPalette[i].g = palette[j+1];
       SFRPalette[i].b = palette[j+2];
   }
   SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, SFRPalette, 0, 256);
}

void setBrightness(int factor)
{
   int i ;
   for ( i=0 ; i<256 ; i++ )
   {
     SFRFadePalette[i].r = (SFRPalette[i].r * factor) / 100 ;
     SFRFadePalette[i].g = (SFRPalette[i].g * factor) / 100 ;
     SFRFadePalette[i].b = (SFRPalette[i].b * factor) / 100 ;
   }
}

void blit_raw(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys)
{
    int x, y;
    // compute starting address in dst surface
    Uint8  *dst_buffer = (Uint8*)dst->pixels + y_pos * dst->pitch + x_pos;
    Uint8  *src_bitmap = p;

    for(y = 0; y < ys; y++)
    {
        for(x = 0; x < xs; x++)
        {
            //if(src_bitmap[x] != alpha_color)
                dst_buffer[x] = src_bitmap[x];
        }
        dst_buffer += dst->w;
        src_bitmap += xcs;
    }
}

#define X_FLIP 1
#define Y_FLIP 2

void blit_raw_flipped(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys, int flip)
{
    int x, y, x2;
    int xstart = 0, xinc = 1;
    // compute starting address in dst surface
    Uint8  *dst_buffer = (Uint8*)dst->pixels + y_pos * dst->pitch + x_pos;
    Uint8  *src_bitmap = p;
    if(flip & X_FLIP)
    {
        xstart  = xs-1; xinc = -1;
    }
    if(flip & Y_FLIP)
    {
        src_bitmap = p+xcs*(ys-1);
        xcs = -xcs;
    }

    for(y = 0; y < ys; y++)
    {
        //src_bitmap = p+xcs*(ys-y-1);
        for(x = 0, x2 = xstart; x < xs; x++, x2+=xinc)
        {
                if(src_bitmap[x2] != alpha_color)
                    dst_buffer[x]=src_bitmap[x2];
        }
        dst_buffer += dst->w;
        src_bitmap += xcs;
    }
}

void blit_scaled_X2(SDL_Surface *dst, int x_pos, int y_pos, unsigned char *p, int xcs, int xs, int ys)
{
    int x, y, x2;
    unsigned char pixel;
    // compute starting address in dst surface
    Uint8  *dst_buffer = (Uint8*)dst->pixels + y_pos * dst->pitch + x_pos;
    Uint8  *src_bitmap = (Uint8*) p;

    for(y = 0; y < ys; y++)
    {
        for(x = 0, x2= 0; x < xs; x++, x2 += 2)
        {
            pixel = src_bitmap[x];
            dst_buffer[x2] = pixel; dst_buffer[x2+1] = pixel;
            (dst_buffer + dst->w)[x2] = pixel; (dst_buffer + dst->w)[x2+1] = pixel;
        }
        dst_buffer += dst->w << 1;
        src_bitmap += xcs;
    }
}

char *getFilePath(char *filename)
{
    int len = strlen(data_folder);
    memset(file_path, 0, sizeof(file_path));
    strcpy(file_path, data_folder);
    file_path[len] = '/'; len++;
    strcpy(&file_path[len], filename);
    return file_path;
}

void get_str_value(FILE *fp, char* str_var, int str_var_size, const char* name)
{
    char c, s[20];
    fscanf(fp,"%s %c", s, &c);
    if(strcmp(s, name))
    {
        //printf("nono strn= %s   strv= %s \n",name,str_var);
        c=getc(fp);
        while(c != '\n' && c != EOF){c=getc(fp);} ungetc(c, fp);
        return;
    }
    memset(str_var, 0, str_var_size);
    c=getc(fp);
    while(isspace(c) && c != EOF){c=getc(fp);} ungetc(c, fp);
    fgets(str_var, 512, fp);
    int i = strlen(str_var)-1;
    while(isspace(str_var[i]) && i > 0){str_var[i] = 0; i--;}
    //printf("strn= %s   strv= %s \n",name,str_var);
}

void get_int_value(FILE *fp, int *int_var, const char* name)
{
    char c, s[20];
    fscanf(fp,"%s %c", s, &c);
    if(strcmp(s, name)) {/*printf("nono intn= %s   intv= %d\n",name,*int_var);*/return;}
    fscanf(fp,"%d", int_var);
    //printf("intn= %s   intv= %d\n",name,*int_var);
}

extern int hit_freeze_time, ground_slam, air_recovering, auto_hitspark, shrink_hitbox, shrink_factor, debug;

void load_config(void)
{
    FILE *fp ;
    fp=fopen("sfrconfig.cfg","rb") ;
    if(fp==NULL)
    {
        fprintf(stderr, "Couldn't open sfrconfig.cfg file \n");
        return;
        //exit(1);
    }
    get_str_value(fp, data_folder, sizeof(data_folder), "data_folder");
    get_int_value(fp, &hit_freeze_time, "hit_freeze_time");
    get_int_value(fp, &ground_slam, "ground_slam");
    get_int_value(fp, &air_recovering, "air_recovering");
    get_int_value(fp, &auto_hitspark, "auto_hitspark");
    get_int_value(fp, &shrink_hitbox, "shrink_hitbox");
    get_int_value(fp, &shrink_factor, "shrink_factor");
    if(shrink_factor <= 0) shrink_factor = 1;
    get_int_value(fp, &debug, "debug");
    get_int_value(fp, &full_screen, "full_screen");
    fclose(fp) ;
}

int sfr_keys[SDLK_LAST];

void intit_sfr_keys(void)
{
    memset(sfr_keys, 0, sizeof(sfr_keys));

    sfr_keys[SDLK_1] = MAKE_1                ;
    sfr_keys[SDLK_2] = MAKE_2                ;
    sfr_keys[SDLK_3] = MAKE_3                ;
    sfr_keys[SDLK_4] = MAKE_4                ;
    sfr_keys[SDLK_5] = MAKE_5                ;
    sfr_keys[SDLK_6] = MAKE_6                ;
    sfr_keys[SDLK_7] = MAKE_7                ;
    sfr_keys[SDLK_8] = MAKE_8                ;
    sfr_keys[SDLK_9] = MAKE_9                ;
    sfr_keys[SDLK_0] = MAKE_0                ;
    sfr_keys[SDLK_MINUS] = MAKE_MINUS        ;
    sfr_keys[SDLK_EQUALS] = MAKE_EQUALS      ;
    sfr_keys[SDLK_BACKSPACE] = MAKE_BKSP     ;
    sfr_keys[SDLK_q] = MAKE_Q                ;
    sfr_keys[SDLK_w] = MAKE_W                ;
    sfr_keys[SDLK_e] = MAKE_E                ;
    sfr_keys[SDLK_r] = MAKE_R                ;
    sfr_keys[SDLK_t] = MAKE_T                ;
    sfr_keys[SDLK_y] = MAKE_Y                ;
    sfr_keys[SDLK_u] = MAKE_U                ;
    sfr_keys[SDLK_i] = MAKE_I                ;
    sfr_keys[SDLK_o] = MAKE_O                ;
    sfr_keys[SDLK_p] = MAKE_P                ;
    sfr_keys[SDLK_LEFTBRACKET] = MAKE_LFT_BRACKET      ;
    sfr_keys[SDLK_RIGHTBRACKET] = MAKE_RGT_BRACKET      ;
    sfr_keys[SDLK_a] = MAKE_A                ;
    sfr_keys[SDLK_s] = MAKE_S                ;
    sfr_keys[SDLK_d] = MAKE_D                ;
    sfr_keys[SDLK_f] = MAKE_F                ;
    sfr_keys[SDLK_g] = MAKE_G                ;
    sfr_keys[SDLK_h] = MAKE_H                ;
    sfr_keys[SDLK_j] = MAKE_J                ;
    sfr_keys[SDLK_k] = MAKE_K                ;
    sfr_keys[SDLK_l] = MAKE_L                ;
    sfr_keys[SDLK_SEMICOLON] = MAKE_SEMI     ;

    //sfr_keys[SDLK_SPACE] = MAKE_APOS             ;
    //sfr_keys[SDLK_SPACE] = MAKE_TILDE            ;

    sfr_keys[SDLK_LSHIFT] = MAKE_LEFT_SHIFT       ;
    sfr_keys[SDLK_BACKSLASH] = MAKE_BACK_SLASH       ;
    sfr_keys[SDLK_z] = MAKE_Z                ;
    sfr_keys[SDLK_x] = MAKE_X                ;
    sfr_keys[SDLK_c] = MAKE_C                ;
    sfr_keys[SDLK_v] = MAKE_V                ;
    sfr_keys[SDLK_b] = MAKE_B                ;
    sfr_keys[SDLK_n] = MAKE_N                ;
    sfr_keys[SDLK_m] = MAKE_M                ;
    sfr_keys[SDLK_COMMA] = MAKE_COMMA            ;
    sfr_keys[SDLK_PERIOD] = MAKE_PERIOD           ;
    sfr_keys[SDLK_SLASH] = MAKE_FOWARD_SLASH     ;
    sfr_keys[SDLK_RSHIFT] = MAKE_RIGHT_SHIFT      ;

    //sfr_keys[SDLK_SPACE] = MAKE_PRT_SCRN         ;

    sfr_keys[SDLK_CAPSLOCK] = MAKE_CAPS_LOCK        ;
    sfr_keys[SDLK_F1] = MAKE_F1               ;
    sfr_keys[SDLK_F2] = MAKE_F2               ;
    sfr_keys[SDLK_F3] = MAKE_F3               ;
    sfr_keys[SDLK_F4] = MAKE_F4               ;
    sfr_keys[SDLK_F5] = MAKE_F5               ;
    sfr_keys[SDLK_F6] = MAKE_F6               ;
    sfr_keys[SDLK_F7] = MAKE_F7               ;
    sfr_keys[SDLK_F8] = MAKE_F8               ;
    sfr_keys[SDLK_F9] = MAKE_F9               ;
    sfr_keys[SDLK_F10] = MAKE_F10              ;
    sfr_keys[SDLK_F11] = MAKE_F11              ;
    sfr_keys[SDLK_F12] = MAKE_F12              ;
    sfr_keys[SDLK_NUMLOCK] = MAKE_NUM_LOCK         ;
    sfr_keys[SDLK_SCROLLOCK] = MAKE_SCROLL_LOCK      ;
    sfr_keys[SDLK_HOME] = MAKE_HOME             ;
    sfr_keys[SDLK_PAGEUP] = MAKE_PGUP             ;
    sfr_keys[SDLK_MINUS] = MAKE_KEYPAD_MINUS     ;

    //sfr_keys[SDLK_SPACE] = MAKE_CENTER           ;

    sfr_keys[SDLK_PLUS] = MAKE_KEYPAD_PLUS      ;
    sfr_keys[SDLK_END] = MAKE_END              ;
    sfr_keys[SDLK_PAGEDOWN] = MAKE_PGDWN            ;
    sfr_keys[SDLK_INSERT] = MAKE_INS              ;
    sfr_keys[SDLK_DELETE] = MAKE_DEL              ;
    sfr_keys[SDLK_RIGHT] = MAKE_RIGHT      ;
    sfr_keys[SDLK_LEFT] = MAKE_LEFT       ;
    sfr_keys[SDLK_UP] = MAKE_UP         ;
    sfr_keys[SDLK_DOWN] = MAKE_DOWN       ;
    sfr_keys[SDLK_RETURN] = MAKE_ENTER      ;
    sfr_keys[SDLK_TAB] = MAKE_TAB        ;
    sfr_keys[SDLK_SPACE] = MAKE_SPACE      ;
    sfr_keys[SDLK_RCTRL] = MAKE_CTRL       ;
    sfr_keys[SDLK_LCTRL] = MAKE_CTRL       ;
    sfr_keys[SDLK_RALT] = MAKE_ALT        ;
    sfr_keys[SDLK_LALT] = MAKE_ALT        ;
    sfr_keys[SDLK_ESCAPE] = MAKE_ESC        ;
}

extern unsigned char key_table[128];
extern int keys_active;

int read_keyboard(void)
{
    if(SDL_PollEvent(&event) == 1)
    {
        switch(event.type)
        {
            case SDL_QUIT:
                key_table[MAKE_ESC] = 1;
                keys_active++;
                break;
            case SDL_KEYDOWN://printf("key_m=%d\n",sfr_keys[event.key.keysym.sym]);
                return sfr_keys[event.key.keysym.sym];
            case SDL_KEYUP://printf("key_b=%d\n",sfr_keys[event.key.keysym.sym]+128$);
                return sfr_keys[event.key.keysym.sym]+128;
        }
    }
    return 0;
}

void free_music(MIDI* midi_music)
{
    if (midi_music != NULL)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(midi_music->mix_music);
        free(midi_music);
    }
}

extern int midvolume;
MIDI* load_music(const char *filename)
{
    MIDI* midi_music = (MIDI*)malloc(sizeof(MIDI));
    midi_music->mix_music = Mix_LoadMUS(getFilePath((char*)filename));
    strcpy(midi_music->name, filename);
    Mix_VolumeMusic(((midvolume+1)%257)/2);
    return midi_music;
}

void play_music(MIDI* midi_music)
{
    Mix_PlayMusic(midi_music->mix_music, -1);
}

extern int sfxvolume;
SAMPLE* load_sound(const char *filename)
{
    SAMPLE* sample = (SAMPLE*)malloc(sizeof(SAMPLE));
    sample->mix_chunk = Mix_LoadWAV(getFilePath((char*)filename));
    strcpy(sample->name, filename);
    Mix_VolumeChunk(sample->mix_chunk, ((sfxvolume+1)%257)/2);
    return sample;
}

void play_sound(SAMPLE* sample)
{
    Mix_PlayChannel(-1, sample->mix_chunk, 0);
}

void free_sound(SAMPLE* sample)
{
    if (sample != NULL)
    {
        Mix_FreeChunk(sample->mix_chunk);
        free(sample);
    }
}

void SFR_SDL_init(void)
{
   load_config();
   SDL_Init(SDL_INIT_VIDEO);
   SDL_WM_SetCaption("SFR",NULL);
   if(full_screen) full_screen = SDL_FULLSCREEN;
   screen = SDL_SetVideoMode(640, 400, 8, SDL_HWPALETTE|SDL_HWSURFACE|SDL_DOUBLEBUF|full_screen);
   setSFRPalette();
   intit_sfr_keys();
   Mix_Init(0);
   Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
}

void SFR_SDL_quit(void)
{
   Mix_HaltChannel(-1);
   Mix_HaltMusic();
   Mix_FreeMusic(game_music);
   Mix_CloseAudio();
   Mix_Quit();
   SDL_Quit();
   printf("Bye! from SFR SDL.\n");
   printf("Thank to Raytomely for this port.\n");
}

void processInput(void)
{
    if(SDL_PollEvent(&event) == 1)
    {
        switch(event.type)
        {
            case SDL_QUIT:
                main_loop = 0;
                break;
            /*case SDL_KEYDOWN:
                 switch(event.key.keysym.sym)
                {
                    case SDLK_1:
                        printf("1\n");
                        break;
                }*/
        }
    }
}

void sleep(void)
{
    static int old_time = 0,  actual_time = 0;
    actual_time = SDL_GetTicks();
    if (actual_time - old_time < fps) // if less than fps ms has passed
    {
        SDL_Delay(fps - (actual_time - old_time));
        old_time = SDL_GetTicks();
    }
    else
    {
        old_time = actual_time;
    }
}

void init_clock(int speed)
{
    if(speed > 0)
        fps = 16 + (speed * 10);
}

void main_event_loop(void)
{
    main_loop = 1;
    while(main_loop)
    {
        processInput();
        sleep();
    }
}

MIDI* midi_ptr;
SAMPLE* sample_ptr;
void stub1(int a, REGS *b, REGS *c){};
void stub2(MIDI *midi){};
