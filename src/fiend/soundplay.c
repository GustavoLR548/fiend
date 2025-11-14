////////////////////////////////////////////////////
// This file contains sound playing and updating
//
// all code by Thomas Grip 
///////////////////////////////////////////////////



#include <allegro.h>


#include <stdio.h>
#include <string.h>

#include "../audio.h"
#include "../fiend.h"
#include "../grafik4.h"




extern SOUND_INFO *sound_info;




//calculate pan and volume
void calc_sound_prop(int x,int y,int*vol, int*pan, int s_vol)
{
 float dist, x_dist;
 	
 dist = distance(x,y,player.x,player.y);
 
 if(dist>HEAR_RANGE)
 {
	 *vol=0;
	 *pan=128;
	  return;
 }

 *vol = s_vol - s_vol * (dist/HEAR_RANGE);

 if(*vol<0)*vol=0;

 x_dist=x-(int)player.x;
 *pan = 128 +(127*(x_dist/HEAR_RANGE));
}





//////////////////////////////////////////////////
//////////// PLAY A SOUND ////////////////////////
//////////////////////////////////////////////////

int play_fiend_sound(char *name, int x, int y,int lower_at_dist,int loop,int priority)
{
	int i,num;
	int found_sound=0;
	int found_data=0;
	int vol, pan;
	int temp;


	printf("[GAME AUDIO] play_fiend_sound called: name='%s', x=%d, y=%d, loop=%d\n", name, x, y, loop);

	if(!sound_is_on)return -1;
	
	///////// Find The sound number ////////////
	
	for(i=0;i<num_of_sounds;i++)
		if(strcmp(sound_info[i].name,name)==0)
		{
			found_sound=1;
			num=i;
			break;
		}
	if(!found_sound){
		printf("[GAME AUDIO] Sound '%s' not found in sound_info array!\n", name);
		return -1;
	}

	//if the sound has more then one alternative (num>1) randomize between em
	if(sound_info[num].num>1)
	{
		temp = RANDOM(0,sound_info[num].num-1);
		num+=temp;
	}
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
		if(sound_data[i].used==0)
		{
			found_data=1;
			break;
		}

	if(!found_data)return -1;
	
	///////// Set the sound data ////////////
	
	sound_data[i].used=1;
	sound_data[i].sound_num=num;
	sound_data[i].priority=priority;
	sound_data[i].x=x;
	sound_data[i].y=y;
	sound_data[i].loop=loop;
	sound_data[i].lower_at_dist=lower_at_dist;
	sound_data[i].playing=0;
	sound_data[i].vol=sound_info[sound_data[i].sound_num].volume;
	
	
	///////// Play the sound and set propreties //
	
	if(!sound_data[i].lower_at_dist)
	{
		vol = sound_data[i].vol;
		pan = 128;
	}
	else
		calc_sound_prop(sound_data[i].x,sound_data[i].y,&vol, &pan, sound_info[sound_data[i].sound_num].volume);
	
	// Apply master volume
	vol = (vol * fiend_sound_volume) / 255;
	if(vol > 255) vol = 255;
	if(vol < 0) vol = 0;
				
	sound_data[i].voice_num = audio_play_sound(sound_info[num].sound, sound_data[i].loop, vol, pan);
	
	if(sound_data[i].voice_num==NULL)return -1;
	
	sound_data[i].playing=1;
	
	return 	i;	
}




//////////////////////////////////////////////////
////////////// UPDATE THE SOUNDS /////////////////
///////////////////////////////////////////////////
 
void update_sound(void)
{
	int i;
	int vol, pan;
	static int debug_counter = 0;
	
	if(!sound_is_on)return;
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
	{
		if(sound_data[i].used)
		{
			if(!audio_is_playing(sound_data[i].voice_num))
			{
				audio_stop_channel(sound_data[i].voice_num);
				sound_data[i].used=0;
			}
			else
			{
				if(!sound_data[i].lower_at_dist)
				{
					vol = sound_info[sound_data[i].sound_num].volume;
					pan = 128;
				}
				else
					calc_sound_prop(sound_data[i].x,sound_data[i].y,&vol, &pan, sound_info[sound_data[i].sound_num].volume);
				
				// Apply master volume
				int vol_before = vol;
				vol = (vol * fiend_sound_volume) / 255;
				if(vol > 255) vol = 255;
				if(vol < 0) vol = 0;
				
				// Debug: print every 60 frames for looping sounds
				if(sound_data[i].loop && debug_counter % 60 == 0) {
					printf("[UPDATE_SOUND] slot=%d, sound_num=%d, vol_before=%d, fiend_sound_volume=%d, vol_after=%d, channel=%p\n",
						i, sound_data[i].sound_num, vol_before, fiend_sound_volume, vol, sound_data[i].voice_num);
				}
				
				audio_set_volume(sound_data[i].voice_num, vol);
				audio_set_pan(sound_data[i].voice_num, pan);
			}

		}
	}
	debug_counter++;
}



/////////////////////////////////////////////////////
//////////// STOP ALL SOUNDS FUNCTIONS ////////////////
/////////////////////////////////////////////////////


void stop_sound_num(int num)
{
	if(!sound_is_on)return;
	audio_stop_channel(sound_data[num].voice_num);
	sound_data[num].used=0;
}


void stop_sound_name(char *name)
{
	int found_sound=0;
	int num,i;
	
	if(!sound_is_on)return;
	
	for(i=0;i<num_of_sounds;i++)
		if(strcmp(sound_info[i].name,name)==0)
		{
			found_sound=1;
			num=i;
			break;
		}
	if(!found_sound)return;
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
		if(sound_data[i].used)
		{
			if(sound_data[i].sound_num==num)
				stop_sound_num(i);
		}

}




void stop_all_sounds(void)
{
	int i;

	if(!sound_is_on)return;
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
	{
		if(sound_data[i].used)
		{
			audio_stop_channel(sound_data[i].voice_num);
			sound_data[i].used=0;
		}
	}

}


void pause_all_sounds(void)
{
	int i;

	if(!sound_is_on)return;
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
	{
		if(sound_data[i].used)
		{
			audio_pause_channel(sound_data[i].voice_num);
			sound_data[i].used=0;
		}
	}

}

void resume_all_sounds(void)
{
	int i;

	if(!sound_is_on)return;
	
	for(i=0;i<MAX_SOUNDS_PLAYING;i++)
	{
		if(sound_data[i].used)
		{
			audio_resume_channel(sound_data[i].voice_num);
			sound_data[i].used=0;
		}
	}

}


/////////////////////////////////////////////////////////////
/////////////////// MUSIC FUNCTIONS ////////////////////////
/////////////////////////////////////////////////////////////


char current_music[50] ="none";
int music_is_looping=0;
static void *audio_music_channel;
static void *music_sound;

int play_fiend_music(char* file, int loop)
{
	char path[100];
	char error_string[50];
	
	if(!sound_is_on)return -1;
	

	if(strcmp(file,current_music)==0)return 1;
	
	if(strcmp(current_music,"none")!=0)
	{
		audio_stop_music(audio_music_channel);
		audio_free_music(music_sound);
	}
	
	sprintf(path,"music/%s",file);
	
	if(audio_load_music(path, &music_sound) != 0)
	{
		sprintf(error_string,"Error loading stream \"%s\" ",path);
		make_engine_error(error_string);
		return 0;
	}
	
	music_is_looping = loop;
	strcpy(current_music,file);
	
	speed_counter=0;
	audio_music_channel = audio_play_music(music_sound, loop, fiend_music_volume);
	
	return 1;
}


void set_fiend_music_volume(int vol)
{
	printf("[MUSIC] set_fiend_music_volume called with vol=%d, channel=%p\n", vol, audio_music_channel);
	if(audio_music_channel)
		audio_set_volume(audio_music_channel, vol);
}


void stop_fiend_music(void)
{
	if(!sound_is_on)return;
	if(strcmp(current_music,"none")==0)return;
	
	if(strcmp(current_music,"none")!=0)
	{
		audio_stop_music(audio_music_channel);
		audio_free_music(music_sound);
		audio_music_channel = NULL;
		music_sound = NULL;
		strcpy(current_music,"none");
	}
}

void pause_fiend_music(void)
{
	if(!sound_is_on)return;
	if(strcmp(current_music,"none")==0)return;
	
	if(audio_music_channel && !audio_is_music_paused(audio_music_channel))
	{
		audio_pause_music(audio_music_channel);
	}
}

void resume_fiend_music(void)
{
	if(!sound_is_on)return;
	if(strcmp(current_music,"none")==0)return;
	
	if(audio_music_channel && audio_is_music_paused(audio_music_channel))
	{
		audio_resume_music(audio_music_channel);
	}
}


////////////////////////////////////////////////////////////
////////// UPDATE SOUND EMITORS ///////////////////////////
///////////////////////////////////////////////////////////

void update_soundemitors(void)
{
	int i;

	for(i=0;i<map->num_of_soundemitors;i++)
	{
		if(map->soundemitor[i].active && map->soundemitor[i].voice_num<0)
		{
			map->soundemitor[i].voice_num = play_fiend_sound(map->soundemitor[i].sound_name, map->soundemitor[i].x, map->soundemitor[i].y, map->soundemitor[i].emitor_type, map->soundemitor[i].loop, 200);

		}

	}

}