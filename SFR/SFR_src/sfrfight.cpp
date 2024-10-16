#include "sfrabat.h"
#include "sfrwai.h"
#include "sfrtag.h"

int main(int argc, char *argv[])
{
  reds[0] = 228;
  reds[1] = 196;
  reds[2] = 224;
  reds[3] = 192;

  FILE *fp;
  //char bgname[25];
  //int bgoff;
  int dctr=0;
  int temp=0;
  int tournament=0;

  //char choice;

  char title[25];
  portrait titlepic;
  char titlesong[25];
  char palname[25];
  char smfont[25];

  char bgfont[25];
  char config[25];

  char font_name[25];

  speed = 0;
  midvolume = 255;
  sfxvolume = 255;
  juggling = 0;
  drama = 0;
  centext = 0;
  noleg = 0;
  shadcnt = 10;
  maxrounds = 2;
  shadowdelay = 1;
  //SAMPLE *test;

  if((fp = fopen("options.opt","rb")) == NULL) {
    printf("how slow?(0 = fastest, 10 = slowest)\n");
    scanf("%d",&speed);
    }
  else fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d",&speed,&sfxvolume,&midvolume,&juggling,&air_blocking,&drama,&centext,&noleg,&shadcnt,&maxrounds,&shadowdelay,&shadow_color);
  fclose(fp);

  srand(time(NULL));

  init_clock(speed);
  load_hud((char*)"bars.cfg");

  SFR_SDL_init();
  if(!noleg) {
    allegro_init();
    install_timer();

    if(install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL) != 0) {
      printf("Error initialising sound system\n");
      exit(-1);
      }

    set_volume(sfxvolume,midvolume);
    }

  fp = fopen("sfr.cfg","rb");

  fscanf(fp,"%s",title);
  get_portrait((port_ptr)&titlepic,title);
  fscanf(fp,"%s",titlesong);
  fscanf(fp,"%s",palname);
  fscanf(fp,"%s",smfont);
  get_font((font_ptr)&fnt,smfont);
  fscanf(fp,"%s",bgfont);
  get_font((font_ptr)&bgfnt,bgfont);

  fscanf(fp,"%s",font_name);
  get_font((font_ptr)&spfnt,font_name);

  fscanf(fp,"%s",font_name);
  fscanf(fp,"%s",font_name);

  fscanf(fp,"%s",font_name);
  get_font((font_ptr)&shdfnt,font_name);

  fscanf(fp,"%s",font_name);
  fscanf(fp,"%s",font_name);
  fscanf(fp,"%s",font_name);
  fscanf(fp,"%s",font_name);

  fscanf(fp,"%s",config);
  get_font((font_ptr)&hugefnt,"hugefont.fnt");
  fclose(fp);

  if(!get_cs((cs_ptr)&cs,config))
  {
    printf("Get Character Set Failed!\n");
    return 0;
  }

  char getex;
  char ext[4];
  int i;

  i=0;
  getex = 'n';
  while(getex != '.'){ getex=config[i++]; }
  ext[0]=config[i++];
  ext[1]=config[i++];
  ext[2]=config[i++];
  ext[3]=0;//NULL;

  if(strcmp(ext,"cfg")) tournament = 1;
  else tournament = 0;

  MIDI *titletune = load_midi(titlesong);
  if(!get_systemsounds((char*)"syssound.cfg"))
  {
    printf("Failed to get system sounds. Exiting.\n");
    return 0;
  }


  set_video_mode(VGA);
//  SetVesaMode(SVGA640x480x256);
  palette sfrpal = pal_import(palname,0);
  set_pal(sfrpal);
  init_double_buffer();

  char strings[6][25];
  int val=0;
  int ctr=0;
  int rad = 0;

  if(tournament) {
    strcpy(strings[0],"Story Battle");
    strcpy(strings[1],"Exhibition Battle");
    strcpy(strings[2],"Tag Battle");
    strcpy(strings[3],"Survival Battle");
    strcpy(strings[4],"Options Mode");
    strcpy(strings[5],"Exit Game");
    }
  else {
    strcpy(strings[0],"Tournament Mode");
    strcpy(strings[1],"Exhibition Mode");
    strcpy(strings[2],"KOF Mode"); //"Tag Mode");
    strcpy(strings[3],"Watch Mode");
    strcpy(strings[4],"Options Mode");
    strcpy(strings[5],"Exit Game");
    }  //SDL_FillRect(screen,NULL,30);
//draw_portrait(0,0,(port_ptr)&titlepic,0);
//draw_string(100,100,(font_ptr)&bgfnt,"hello",'l',1);
//draw_character(100,100,(font_ptr)&hugefnt,1,1);
//SDL_Flip(screen);//main_event_loop();
//while(!keys_active){processKeyboardInput();sleep();}
  key_init();

  START:

  do {
    if(dctr==0) play_midi(titletune,1);
    draw_portrait(0,0,(port_ptr)&titlepic,0);

    for(ctr=0;ctr<6;ctr++) {
      if(val == ctr) draw_string(75,160+(ctr-val)*20,(font_ptr)&bgfnt,strings[ctr],'l',1);
      else if(abs(ctr-val) == 1) draw_string(75,160+(ctr-val)*20,(font_ptr)&shdfnt,strings[ctr],'l',1);
      }

    if(rad<5) rad++;
    else rad = 0;
    circle_b(50,165,255,rad);
    show_double_buffer(0,0,1);

    ++dctr;
    if(dctr > 750) {
      dctr=0;
      new_demo_mode(config);
      }
    KEYS_INPUT();
    }while(!keys_active);

  if(key_table[MAKE_UP]) {
    play_sample(systemsounds[3],sfxvolume,125,1000,0);
    /*do {
      }while(keys_active);*/
    FLUSH_KEY(MAKE_UP);
    if(val>0) val--;
    goto START;
    }
  else if(key_table[MAKE_DOWN]) {
    play_sample(systemsounds[3],sfxvolume,125,1000,0);
    /*do {
      }while(keys_active);*/
    FLUSH_KEY(MAKE_DOWN);
    if(val<5) val++;
    goto START;
    }
  else if(key_table[MAKE_ENTER]) {
    /*do {
      }while(keys_active);*/
    FLUSH_KEY(MAKE_ENTER);
    play_sample(systemsounds[0],sfxvolume,125,1000,0);
    switch(val) {
      case 0: dctr=0;
              tournament_mode(config);
              goto START;
              break;
      case 1: dctr=0;
              exhibition_mode(config);
              goto START;
              break;
      case 2: dctr=0;
              //tag_mode();
              kof_mode();
              goto START;
              break;
      case 3: new_demo_mode(config);
              dctr=0;
              goto START;
              break;
      case 4: temp = options_menu();
              if(!temp)
              {
                printf("Failed to save options.\n");
                delay(2000);
              }
              dctr=0;
              goto START;
              break;
      case 5: goto END;
              break;
      default : goto START;
      }

    }
  else if(key_table[MAKE_F1])
  {
      if(!debug)
      {
          debug = 1;
          draw_string(100,100,(font_ptr)&fnt,"debug enabled",'l',1);
      }
      else
      {
          debug = 0;
          draw_string(100,100,(font_ptr)&fnt,"debug disabled",'l',1);
      }
      show_double_buffer(0,0,1);
      delay(1000);
      goto START;
  }
  else if(key_table[MAKE_ESC]) goto END;
  else goto START;

  END:
//  destroy_midi(titletune);
  key_delete();
  set_video_mode(TEXT);
  kill_font((font_ptr)&fnt);
  kill_font((font_ptr)&bgfnt);
  kill_font((font_ptr)&shdfnt);
  kill_font((font_ptr)&spfnt);
  kill_font((font_ptr)&hugefnt);
  kill_portrait((port_ptr)&titlepic);
  kill_double_buffer();
  free_hud();
  destroy_midi(titletune);
  destroy_midi(cs.bgtune);
  free_systemsounds();
  SFR_SDL_quit();

  return 1;
}


