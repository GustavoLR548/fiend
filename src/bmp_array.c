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

/* Helper function to find first matching file by reading from preprocessed resource lists
 * This avoids Windows al_findfirst issues and is faster than directory scanning */
static char* find_first_file_from_list(const char *dir_pattern)
{
	static char result_path[512];
	static char resource_list_path[512];
	FILE *list_file;
	char line[512];
	char dir_part[512];
	char *last_sep;
	int dir_len;
	
	/* Extract directory part from pattern (remove *.bmp) */
	strcpy(dir_part, dir_pattern);
	last_sep = strrchr(dir_part, '\\');
	if(!last_sep) last_sep = strrchr(dir_part, '/');
	
	if(last_sep) {
		*last_sep = '\0'; /* Remove everything after last separator */
	}
	
	/* Convert to forward slashes for comparison */
	for(char *p = dir_part; *p; p++) {
		if(*p == '\\') *p = '/';
	}
	dir_len = strlen(dir_part);
	
	log_info("Looking for files in directory: %s", dir_part);
	
	/* Determine which resource list file to use based on path */
	if(strstr(dir_part, "graphic/characters")) {
		strcpy(resource_list_path, "data/resource_lists/characters.txt");
	} else if(strstr(dir_part, "graphic/enemies")) {
		strcpy(resource_list_path, "data/resource_lists/enemies.txt");
	} else if(strstr(dir_part, "graphic/objects")) {
		strcpy(resource_list_path, "data/resource_lists/objects.txt");
	} else if(strstr(dir_part, "graphic/faces")) {
		strcpy(resource_list_path, "data/resource_lists/faces.txt");
	} else if(strstr(dir_part, "graphic/tiles")) {
		strcpy(resource_list_path, "data/resource_lists/tiles.txt");
	} else if(strstr(dir_part, "graphic/particles")) {
		strcpy(resource_list_path, "data/resource_lists/particles.txt");
	} else {
		log_error("Unknown resource directory: %s", dir_part);
		return NULL;
	}
	
	log_info("Opening resource list: %s", resource_list_path);
	
	list_file = fopen(resource_list_path, "r");
	if(!list_file) {
		log_error("Failed to open resource list: %s", resource_list_path);
		return NULL;
	}
	
	/* Read through the list file looking for matching directory */
	while(fgets(line, sizeof(line), list_file)) {
		/* Remove newline */
		line[strcspn(line, "\r\n")] = 0;
		
		/* Check if this file is in the directory we're looking for */
		if(strncmp(line, dir_part, dir_len) == 0) {
			/* Found a matching file! */
			strcpy(result_path, line);
			
			/* Normalize path for current platform */
			normalize_path(result_path);
			
			log_info("Found file from resource list: %s", result_path);
			fclose(list_file);
			return result_path;
		}
	}
	
	fclose(list_file);
	log_error("No files found in resource list for directory: %s", dir_part);
	return NULL;
}

/* Use the resource list approach for finding files */
static char* find_first_file(const char *pattern)
{
	return find_first_file_from_list(pattern);
}
 
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
		char *found_file;
		sprintf(file_path,"%s*.bmp",dir);
		log_info("Loading bitmap array from: %s", file_path);
		
		/* Use al_findfirst directly instead of unreliable for_each_file */
		found_file = find_first_file(file_path);
		if(found_file) {
			temp_bmp_data[0].dat = load_bmp(found_file, NULL);
			if(temp_bmp_data[0].dat == NULL) {
				log_error("Failed to load bitmap: %s", found_file);
			} else {
				log_info("Successfully loaded bitmap: %s", found_file);
			}
		}

		// Check if bitmap was actually loaded
		if(temp_bmp_data[0].dat == NULL)
		{
			log_error("Failed to load bitmap from: %s", file_path);
			allegro_message("couldn't load %s",file_path);
			exit(-1);
		}
		
		log_info("Successfully loaded bitmap from pattern: %s", file_path);
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
		char *found_file;
		BITMAP *bmp;
		sprintf(file_path,"%s*.bmp",dir);
		log_info("Loading RLE sprite from: %s", file_path);
		
		/* Use al_findfirst directly instead of unreliable for_each_file */
		found_file = find_first_file(file_path);
		if(found_file) {
			bmp = load_bmp(found_file, NULL);
			if(bmp) {
				temp_rle_data[0].dat = get_rle_sprite(bmp);
				destroy_bitmap(bmp);
				log_info("Successfully loaded RLE sprite: %s", found_file);
			} else {
				log_error("Failed to load bitmap for RLE: %s", found_file);
			}
		}

		// Check if RLE sprite was actually loaded
		if(temp_rle_data[0].dat == NULL)
		{
			log_error("Failed to load RLE sprite from: %s", file_path);
			allegro_message("couldn't load %s",file_path);
			exit(-1);
		}
		
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
