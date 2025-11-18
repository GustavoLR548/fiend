////////////////////////////////////////////////////
// This file Contains Character loading and drawing funcs 
//
// all code by Thomas Grip 
///////////////////////////////////////////////////


#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <locale.h>

#include "grafik4.h"

#include "fiend.h"
#include "character.h"
#include "rotate_sprite.h"
#include "path_utils.h"




//Info
CHARACTER_INFO *char_info;

//variables
int num_of_characters=0;




//Loads all the character graphics... 
int load_characters(void)
{
	FILE *f;

	
	BMP_ARRAY *temp_data;
	
	char buffer[50];
	char sprite_file_name[100][15];
    char info_file_name[100][15];
	int num_of_frames=1;
	char file_path[256] = "graphic/characters/";
	char final_path[256];
	int i,j,k;
	
	// Set C locale for parsing numbers with periods as decimal separators
	char *old_locale = setlocale(LC_NUMERIC, "C");
	
	/* Normalize path separators for the platform */
	normalize_path(file_path);
	
	char characters_txt[256];
	if(build_path(characters_txt, sizeof(characters_txt), file_path, "characters.txt") != 0) {
		sprintf(fiend_errorcode,"path too long for characters.txt");
		return 1;
	}
	
	char_info = calloc(sizeof(CHARACTER_INFO), MAX_CHARACTERS);

	f = fopen(characters_txt, "r");      //Load the Info file
	
	if(f==NULL)
	{sprintf(fiend_errorcode,"couldn't find characters.txt");return 1;} //error testing...
	
	
	while(fscanf(f, "%s %s\n", sprite_file_name[num_of_characters], info_file_name[num_of_characters])!=EOF)
	{
		num_of_characters++;	
	}

	fclose(f);
    

	for(i=0;i<num_of_characters;i++)
	{
		
		//----------BEGIN GET INFO-----------------------
		if(build_path(final_path, sizeof(final_path), file_path, info_file_name[i]) != 0) {
			sprintf(fiend_errorcode,"path too long: %s%s", file_path, info_file_name[i]);
			return 1;
		}

		f = fopen(final_path, "r");
		if(f==NULL){sprintf(fiend_errorcode,"couldn't load %s",final_path);return 1;}//error testing...

		fscanf(f,"%s %d\n",buffer,&num_of_frames);//get all the info.....
		char_info[i].num_of_frames = num_of_frames;//number of frames
		
		fscanf(f,"%s %s\n",buffer, char_info[i].name);//get the name

		//get witdh, height, number of angles and the speed
		fscanf(f,"%s %d %s %d %s %d %s %d %s %d %s %f %s %d %s %d\n",buffer, &char_info[i].w, buffer, &char_info[i].h,buffer, &char_info[i].hit_w, buffer, &char_info[i].hit_h, buffer, &char_info[i].angles, buffer, &char_info[i].speed, buffer, &char_info[i].energy, buffer, &char_info[i].weight);

		//Get the sounds
		fscanf(f,"%s %s %s %s %s %s\n",buffer, char_info[i].sound_death, buffer, char_info[i].sound_hurt, buffer, char_info[i].sound_ambient);
		
		//blood stain...
		int blood_scan = fscanf(f,"%s %d %s %d %s %f",buffer, &char_info[i].blood_x, buffer, &char_info[i].blood_y,buffer,&char_info[i].run_add);
		
		//Get step info
		int step_scan = fscanf(f,"%s %d %s %d %s %d %s %d",buffer, &char_info[i].walk_step1, buffer, &char_info[i].walk_step2, buffer, &char_info[i].run_step1, buffer, &char_info[i].run_step2);
		if(step_scan < 8 || blood_scan < 6) {
			sprintf(fiend_errorcode,"Error parsing character file %s",final_path);
			return 1;
		}

		//get the number of animations
		int scan_result = fscanf(f,"%s %d",buffer, &char_info[i].num_of_animations);
		
		//read the animation in fo first you get the name of the animation
		//then you read the frames til a '-1' is encounterd
		for(j=0;j<char_info[i].num_of_animations;j++)
		{
			fscanf(f,"%s",char_info[i].animation[j].name);
			k=-1;
			do
			{
				k++;
				if(k >= 60) {
					sprintf(fiend_errorcode,"Too many animation frames in character %d animation %d",i,j);
					return 1;
				}
				if(fscanf(f,"%d",&char_info[i].animation[j].frame[k]) != 1) {
					sprintf(fiend_errorcode,"Error reading animation frame in character %d animation %d frame %d",i,j,k);
					return 1;
				}
			}while(char_info[i].animation[j].frame[k]!=-1);
			
		}

		fclose(f);
		//-------END GET INFO---------------

		
		
		//--------BEGIN GET DATA------------------
		if(build_path(final_path, sizeof(final_path), file_path, sprite_file_name[i]) != 0) {
			sprintf(fiend_errorcode,"path too long: %s%s", file_path, sprite_file_name[i]);
			return 1;
		}

		
	    temp_data = load_bmp_array(final_path,char_info[i].num_of_frames);//load the graphic
		if(temp_data==NULL){sprintf(fiend_errorcode,"couldn't load %s",final_path);return 1;}//error testing...

		
		for(j=0;j<char_info[i].num_of_frames;j++)//For each of the diffrent pics....
		{
			crop_picdata(temp_data[j].dat, &char_info[i].pic[j]);
		}
		
		unload_bmp_array(temp_data);
		//----------END GET DATA-------------------

	
	}




	// Restore original locale
	setlocale(LC_NUMERIC, old_locale);

	return 0;
}



//release all the characters from memory
void release_characters(void)
{
	int i,j;

	for(i=0;i<num_of_characters;i++)
		for(j=0;j<char_info[i].num_of_frames;j++)
		{
			destroy_bitmap(char_info[i].pic[j].data);			
		}
	
			
	free(char_info);
}







//Draw a character to dest....
void draw_fiend_char(BITMAP *dest,CHARACTER_INFO *temp,int x, int y,int action,int frame, float angle)
{
	int num = temp->animation[action].frame[frame];
	
	pivot_sprite(dest, temp->pic[num].data,x,y,temp->pic[num].center_x,temp->pic[num].center_y,degree_to_fixed(angle));
	//pivot_fiend_sprite(dest, temp->pic[num].data,x,y,temp->pic[num].center_x,temp->pic[num].center_y,angle,FIEND_DRAW_MODE_TRANS);
	//rotate_fiend_sprite(dest, temp->pic[num].data,x,y,angle,FIEND_DRAW_MODE_TRANS);
}



//what number had the action name? 
int char_action(CHARACTER_INFO *temp, char *name)
{
	int i;

	for(i=0;i<temp->num_of_animations;i++)
	{
		if(strcmp(name,temp->animation[i].name)==0)
			return i;

	}

	return 0;
}



