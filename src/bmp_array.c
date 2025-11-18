////////////////////////////////////////////////////
// This file contains bitmap array loading funcs
//
// all code by Thomas Grip 
///////////////////////////////////////////////////

#include <allegro.h>
#include <stdio.h>
#include <string.h>


#include "draw.h"
#include "fiend.h"
#include "grafik4.h"
#include "logger.h"
#include "path_utils.h"


#define MAX_BMP_ARRAY_DATA 100
#define MAX_RLE_ARRAY_DATA 100

BMP_ARRAY *temp_bmp_data;

RLE_ARRAY *temp_rle_data;


///////////////////////////////////////////////////////
////// THE BITMAP ARRAY FUNCTIONS /////////////////////
//////////////////////////////////////////////////////
 
static /*int*/ void find_one_file(const char *file,int attr,/*void **/int param)
{
	log_info("find_one_file callback: Loading file '%s'", file);
	temp_bmp_data[0].dat = load_bmp(file,NULL);
	if(temp_bmp_data[0].dat == NULL) {
		log_error("Failed to load bitmap: %s", file);
	} else {
		log_info("Successfully loaded bitmap: %s", file);
	}
    //return 0;
}


BMP_ARRAY* load_bmp_array(char *dir_tmp,int item_num)
{
	char file_path[90];
	char file_name[20];
	int i;
	
	char dir[256];
	sprintf(dir, "%s", dir_tmp);
	
	/* Normalize path separators for the platform */
	normalize_path(dir);
	ensure_trailing_slash(dir);
	
	log_info("load_bmp_array: Original dir='%s', Normalized dir='%s', item_num=%d", dir_tmp, dir, item_num);
	
	if(item_num==-1)
		temp_bmp_data = calloc(sizeof(BMP_ARRAY), MAX_BMP_ARRAY_DATA);
	else
		temp_bmp_data = calloc(sizeof(BMP_ARRAY), item_num);
	
	
	if(item_num==1)
	{
		sprintf(file_path,"%s*.bmp",dir);
		log_info("Loading bitmap array from: %s", file_path);
		log_info("About to call for_each_file...");
		i= for_each_file/*_ex*/(file_path,FA_ALL/*,0*/,find_one_file,0);
		log_info("for_each_file returned: %d (expected 1)", i);

		if(i!=1){allegro_message("couldn't load %s",file_path);exit(-1);}
		
		temp_bmp_data[0].num = item_num;
	}
	else if(item_num==-1)
	{
		i=0;
		while(1)
		{
			if(i<10)
				sprintf(file_name,"00%d.bmp",i);
			else if(i<100)
				sprintf(file_name,"0%d.bmp",i);
			else
				sprintf(file_name,"%d.bmp",i);
		
			if(build_path(file_path, sizeof(file_path), dir, file_name) != 0) {
				allegro_message("path too long: %s%s", dir, file_name);
				exit(-1);
			}

			log_info("Attempting to load: %s", file_path);
			
			temp_bmp_data[i].dat = load_bmp(file_path,NULL);
			
			if(temp_bmp_data[i].dat==NULL)
			{
				if(i==0){
					log_error("Failed to load first bitmap: %s", file_path);
					allegro_message("couldn't load %s",file_path);
					exit(-1);
				}
				else{
					log_info("No more bitmaps found, loaded %d files", i);
					break;
				}
			}
			
			log_info("Successfully loaded: %s", file_path);
			i++;
		}
		temp_bmp_data[0].num = i;
	}
	else
	{
		for(i=0;i<item_num;i++)
		{
			if(i<10)
				sprintf(file_name,"00%d.bmp",i);
			else if(i<100)
				sprintf(file_name,"0%d.bmp",i);
			else
				sprintf(file_name,"%d.bmp",i);
		
			if(build_path(file_path, sizeof(file_path), dir, file_name) != 0) {
				allegro_message("path too long: %s%s", dir, file_name);
				exit(-1);
			}

			log_info("Loading bitmap %d/%d: %s", i+1, item_num, file_path);
			temp_bmp_data[i].dat = load_bmp(file_path,NULL);
			
			if(temp_bmp_data[i].dat==NULL){
				log_error("Failed to load bitmap: %s", file_path);
				allegro_message("couldn't load %s",file_path);
				exit(-1);
			}

			temp_bmp_data[i].num = item_num;
		}
	}
	
	return temp_bmp_data;
}


void unload_bmp_array(BMP_ARRAY *temp)
{
	int num = temp[0].num;
	int i;

	if(temp==NULL)return;

	for(i=0;i<num;i++)
	{
		destroy_bitmap(temp[i].dat);
	}

	free(temp);
}

///////////////////////////////////////////////////////
////// THE RLE ARRAY FUNCTIONS /////////////////////
//////////////////////////////////////////////////////


static /*int*/ void find_one_rle_file(const char *file,int attr,/*void **/int param)
{
	BITMAP *bmp;
	bmp = load_bmp(file,NULL);

	temp_rle_data[0].dat = get_rle_sprite(bmp);
	 
	destroy_bitmap(bmp);
    //return 0;
}


RLE_ARRAY* load_rle_array(char *dir_tmp,int item_num)
{
	char file_path[90];
	char file_name[20];
	BITMAP *bmp;
	int i;
	
	char dir[256];
	sprintf(dir, "%s", dir_tmp);
	
	/* Normalize path separators for the platform */
	normalize_path(dir);
	ensure_trailing_slash(dir);
	
	if(item_num==-1)
		temp_rle_data = calloc(sizeof(RLE_ARRAY), MAX_RLE_ARRAY_DATA);
	else
		temp_rle_data = calloc(sizeof(RLE_ARRAY), item_num);
	
	
	if(item_num==1)
	{
		sprintf(file_path,"%s*.bmp",dir);
		i= for_each_file/*_ex*/(file_path,FA_ARCH/*,0*/,find_one_rle_file,0);

		if(i!=1){allegro_message("couldn't load %s",file_path);exit(-1);}
		
		temp_rle_data[0].num = item_num;
	}
	else if(item_num==-1)
	{
		i=0;
		while(1)
		{
			if(i<10)
				sprintf(file_name,"00%d.bmp",i);
			else if(i<100)
				sprintf(file_name,"0%d.bmp",i);
			else
				sprintf(file_name,"%d.bmp",i);
		
			if(build_path(file_path, sizeof(file_path), dir, file_name) != 0) {
				allegro_message("path too long: %s%s", dir, file_name);
				exit(-1);
			}

			bmp = load_bmp(file_path,NULL);
			
			if(bmp == NULL)
			{
				if(i==0){
					allegro_message("couldn't load %s",file_path);
					exit(-1);
				}
				else{
					break;
				}
			}
			
			temp_rle_data[i].dat = get_rle_sprite(bmp);
			destroy_bitmap(bmp);

			if(temp_rle_data[i].dat==NULL){
				allegro_message("couldn't load %s",file_path);
				exit(-1);
			}

			i++;
		}
		temp_rle_data[0].num = i;
	}
	else
	{
		for(i=0;i<item_num;i++)
		{
			if(i<10)
				sprintf(file_name,"00%d.bmp",i);
			else if(i<100)
				sprintf(file_name,"0%d.bmp",i);
			else
				sprintf(file_name,"%d.bmp",i);
		
			if(build_path(file_path, sizeof(file_path), dir, file_name) != 0) {
				allegro_message("path too long: %s%s", dir, file_name);
				exit(-1);
			}

			bmp = load_bmp(file_path,NULL);
			
			if(bmp == NULL){
				allegro_message("couldn't load %s",file_path);
				exit(-1);
			}
			
			temp_rle_data[i].dat = get_rle_sprite(bmp);
			destroy_bitmap(bmp);

			if(temp_rle_data[i].dat==NULL){
				allegro_message("couldn't load %s",file_path);
				exit(-1);
			}

			temp_rle_data[i].num = item_num;
		}
	}
	
	return temp_rle_data;
}


void unload_rle_array(RLE_ARRAY *temp)
{
	int num = temp[0].num;
	int i;

	if(temp==NULL)return;

	for(i=0;i<num;i++)
	{
		destroy_rle_sprite(temp[i].dat);
	}

	free(temp);
}
