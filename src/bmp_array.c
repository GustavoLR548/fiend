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

/* Helper function to load first matching file using al_findfirst/al_findnext
 * Returns the full path to the first file found, or NULL if none found */
static char* find_first_file(const char *pattern)
{
	struct al_ffblk file_info;
	static char result_path[512];
	char dir_part[512];
	char search_pattern[512];
	char *last_sep;
	int find_result;
	
	/* Extract directory part from pattern */
	strcpy(dir_part, pattern);
	last_sep = strrchr(dir_part, '\\');
	if(!last_sep) last_sep = strrchr(dir_part, '/');
	
	if(last_sep) {
		last_sep[1] = '\0'; /* Keep the separator */
	} else {
		dir_part[0] = '\0'; /* No directory part */
	}
	
	log_info("Searching for files matching: %s", pattern);
	log_info("Directory part: '%s'", dir_part);
	
	/* On Windows, al_findfirst with *.bmp wildcards is unreliable
	 * Use *.* and filter for .bmp files manually 
	 * Also, on Windows, remove trailing slash/backslash before searching */
	strcpy(search_pattern, dir_part);
	
	/* Remove trailing slash/backslash if present - Windows al_findfirst doesn't like it */
	int len = strlen(search_pattern);
	if(len > 0 && (search_pattern[len-1] == '\\' || search_pattern[len-1] == '/')) {
		search_pattern[len-1] = '\0';
		log_info("Removed trailing slash, directory: '%s'", search_pattern);
	}
	
	/* Try with forward slashes - sometimes Windows al_findfirst prefers them */
	for(int i = 0; search_pattern[i]; i++) {
		if(search_pattern[i] == '\\') search_pattern[i] = '/';
	}
	
	strcat(search_pattern, "/*.*");
	
	log_info("Using search pattern (converted to forward slashes): %s", search_pattern);
	find_result = al_findfirst(search_pattern, &file_info, FA_ALL);
	
	if(find_result != 0) {
		log_error("No files found in directory: %s (error code: %d)", dir_part, find_result);
		return NULL;
	}
	
	/* Iterate through files looking for first .bmp file */
	do {
		char *ext = get_extension(file_info.name);
		log_info("Checking file: %s (extension: %s)", file_info.name, ext ? ext : "none");
		
		if(ext && (strcmp(ext, "bmp") == 0 || strcmp(ext, "BMP") == 0)) {
			/* Found a BMP file! */
			sprintf(result_path, "%s%s", dir_part, file_info.name);
			log_info("Found matching BMP file: %s", result_path);
			al_findclose(&file_info);
			return result_path;
		}
	} while(al_findnext(&file_info) == 0);
	
	/* No BMP files found */
	log_error("No .bmp files found in directory: %s", dir_part);
	al_findclose(&file_info);
	return NULL;
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
