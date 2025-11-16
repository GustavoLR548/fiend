////////////////////////////////////////////////////
// This file contains map loading and saving 
//
// all code by Thomas Grip 
///////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "fiend.h"
#include "lightmap.h"
#include "logger.h"


extern int map_x;
extern int map_y;


//Save map *map to *file
int save_map(MAP_DATA *map, char* file)
{
	FILE *f;
	int dummy_ptr = 0;  // 32-bit dummy pointer for compatibility
	
	f = fopen(file, "wb");
	if(f==NULL)return 0;

	// IMPORTANT: Write fields individually to match 32-bit format
	// This ensures compatibility with the 32-bit map files
	
	// Write fixed-size fields at the beginning
	fwrite(map->name, sizeof(char), 40, f);  // name[40]
	fwrite(&map->w, sizeof(int), 1, f);      // w
	fwrite(&map->h, sizeof(int), 1, f);      // h
	fwrite(&map->player_x, sizeof(int), 1, f);     // player_x
	fwrite(&map->player_y, sizeof(int), 1, f);     // player_y
	fwrite(&map->player_angle, sizeof(int), 1, f); // player_angle
	fwrite(&map->outside, sizeof(int), 1, f);      // outside
	fwrite(&map->light_level, sizeof(int), 1, f);  // light_level
	
	fwrite(&map->num_of_lights, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (light)
	
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (layer1)
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (layer2)
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (layer3)
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (shadow)
	
	fwrite(&map->num_of_objects, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (object)
	
	fwrite(&map->num_of_areas, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (area)
	
	fwrite(&map->num_of_look_at_areas, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (look_at_area)
	
	fwrite(&map->num_of_links, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (link)
	
	fwrite(&map->num_of_soundemitors, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (soundemitor)
	
	fwrite(&map->num_of_triggers, sizeof(int), 1, f);
	fwrite(&dummy_ptr, 4, 1, f); // Write 32-bit dummy pointer (trigger)
	
	fwrite(map->var, sizeof(VARIABLE_DATA), LOCAL_VAR_NUM, f);  // var array
	
	fwrite(&map->num_of_path_nodes, sizeof(int), 1, f);
	fwrite(map->path_node, sizeof(PATH_NODE), MAX_PATHNODE_NUM, f);  // path_node array
	
	// Now write the actual data arrays
	fwrite(map->light, sizeof(LIGHT_DATA), map->num_of_lights, f);		
	
    fwrite(map->layer1, sizeof(TILE_DATA), map->w*map->h,f);
	fwrite(map->layer2, sizeof(TILE_DATA), map->w*map->h,f);
	fwrite(map->layer3, sizeof(TILE_DATA), map->w*map->h,f);

	fwrite(map->shadow, sizeof(char), map->w*map->h,f);

	fwrite(map->object, sizeof(OBJECT_DATA),map->num_of_objects, f);
	fwrite(map->area, sizeof(AREA_DATA),map->num_of_areas, f);
	fwrite(map->look_at_area, sizeof(LOOK_AT_AREA_DATA),map->num_of_look_at_areas, f);
	fwrite(map->link, sizeof(LINK_DATA),map->num_of_links, f);
	fwrite(map->soundemitor, sizeof(SOUNDEMITOR_DATA),map->num_of_soundemitors, f);

	fwrite(map->trigger, sizeof(TRIGGER_DATA),map->num_of_triggers, f);
	

	fclose(f);
	return 1;
}


//Loads *file to *map. Memory must be alloctaed for the map struct
//like MAP_DATA map...
int load_map(MAP_DATA* map, char *file)
{

	int i;
	FILE *f;
	
	for(i=0;i<map->num_of_lights;i++)
		destroy_bitmap(lightmap_data[i]);

	
	
	f = fopen(file, "rb");
	if(f==NULL){ sprintf(fiend_errorcode,"couldn't load %s",map);return 0;}

	// IMPORTANT: The map files were created on a 32-bit system where pointers are 4 bytes.
	// On 64-bit systems, pointers are 8 bytes, causing struct layout mismatch.
	// We manually read each field and skip over the 32-bit pointers (4 bytes each)
	// instead of using fread() on the entire MAP_DATA struct.
	
	// Read the fixed-size fields at the beginning
	fread(map->name, sizeof(char), 40, f);  // name[40]
	map->name[39] = '\0';  // Ensure null termination
	fread(&map->w, sizeof(int), 1, f);      // w
	fread(&map->h, sizeof(int), 1, f);      // h
	fread(&map->player_x, sizeof(int), 1, f);     // player_x
	fread(&map->player_y, sizeof(int), 1, f);     // player_y
	fread(&map->player_angle, sizeof(int), 1, f); // player_angle
	fread(&map->outside, sizeof(int), 1, f);      // outside
	fread(&map->light_level, sizeof(int), 1, f);  // light_level
	
	fread(&map->num_of_lights, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (light)
	
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer1)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer2)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer3)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (shadow)
	
	fread(&map->num_of_objects, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (object)
	
	fread(&map->num_of_areas, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (area)
	
	fread(&map->num_of_look_at_areas, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (look_at_area)
	
	fread(&map->num_of_links, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (link)
	
	fread(&map->num_of_soundemitors, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (soundemitor)
	
	fread(&map->num_of_triggers, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (trigger)
	
	fread(map->var, sizeof(VARIABLE_DATA), LOCAL_VAR_NUM, f);  // var array
	
	fread(&map->num_of_path_nodes, sizeof(int), 1, f);
	fread(map->path_node, sizeof(PATH_NODE), MAX_PATHNODE_NUM, f);  // path_node array

	map->light = calloc(sizeof(LIGHT_DATA), map->num_of_lights); 
	fread(map->light, sizeof(LIGHT_DATA), map->num_of_lights, f);		
	
	map->layer1 = calloc(sizeof(TILE_DATA), map->w*map->h); 
	fread(map->layer1, sizeof(TILE_DATA), map->w*map->h,f);
		
	map->layer2 = calloc(sizeof(TILE_DATA), map->w*map->h); 
	fread(map->layer2, sizeof(TILE_DATA), map->w*map->h,f);
		
	map->layer3 = calloc(sizeof(TILE_DATA), map->w*map->h); 
	fread(map->layer3, sizeof(TILE_DATA), map->w*map->h,f);

	map->shadow = calloc(sizeof(char), map->w*map->h); 
	fread(map->shadow, sizeof(char), map->w*map->h,f);

		
	map->object = calloc(sizeof(OBJECT_DATA), map->num_of_objects); 
	fread(map->object, sizeof(OBJECT_DATA),map->num_of_objects, f);
		
	map->area = calloc(sizeof(AREA_DATA), map->num_of_areas); 
	fread(map->area, sizeof(AREA_DATA),map->num_of_areas, f);
	
	map->look_at_area = calloc(sizeof(LOOK_AT_AREA_DATA), map->num_of_look_at_areas); 
	fread(map->look_at_area, sizeof(LOOK_AT_AREA_DATA),map->num_of_look_at_areas, f);
	
	map->link = calloc(sizeof(LINK_DATA), map->num_of_links); 
	fread(map->link, sizeof(LINK_DATA),map->num_of_links, f);

	map->soundemitor = calloc(sizeof(SOUNDEMITOR_DATA), map->num_of_soundemitors); 
	fread(map->soundemitor, sizeof(SOUNDEMITOR_DATA),map->num_of_soundemitors, f);
	
	// DEBUG: Check file position before reading triggers
	log_debug("========== BEFORE READING TRIGGERS ==========");
	log_debug("File position: 0x%lX (%ld)", ftell(f), ftell(f));
	log_debug("About to read %d triggers of size %zu bytes each", 
		map->num_of_triggers, sizeof(TRIGGER_DATA));
	log_debug("Expected to read %zu bytes total", 
		map->num_of_triggers * sizeof(TRIGGER_DATA));
	log_debug("=============================================");
	
	map->trigger = calloc(sizeof(TRIGGER_DATA), map->num_of_triggers);
	fread(map->trigger, sizeof(TRIGGER_DATA),map->num_of_triggers, f);
	
	// DEBUG: Print what triggers we loaded
	log_debug("========== LOADED TRIGGERS ==========");
	log_debug("File position after reading triggers: %ld", ftell(f));
	log_debug("Number of triggers: %d", map->num_of_triggers);
	for(i=0; i<map->num_of_triggers && i<5; i++) {
		log_debug("Trigger[%d]: name='%s' active=%d type=%d", 
			i, map->trigger[i].name, map->trigger[i].active, map->trigger[i].type);
	}
	log_debug("=====================================");
		
	
	
	for(i=0;i<map->num_of_lights;i++)
		create_light_map2(&lightmap_data[i], &map->light[i]);
	
	

	fclose(f);
	return 1;



}


//loads a map reday for editing...this map takes up lots of memory...
//new_map must have been called
int load_edit_map(MAP_DATA* map, char *file)
{

	int i;
	MAP_DATA *temp_map;
	FILE *f;
	
	// Free old map memory if it exists (for map transitions)
	// The first time this is called, new_map() will have already allocated memory
	// On subsequent calls (map transitions), we need to free the old memory first
	if(map->light != NULL) {
		log_debug("Freeing old map->object at %p (%d objects)", (void*)map->object, map->num_of_objects);
		for(i=0;i<map->num_of_lights;i++) {
			destroy_bitmap(lightmap_data[i]);
			lightmap_data[i] = NULL;  // Prevent double-free on exit
		}
		
		free(map->light);
		free(map->layer1);
		free(map->layer2);
		free(map->layer3);
		free(map->shadow);
		free(map->object);
		free(map->area);
		free(map->look_at_area);
		free(map->link);
		free(map->soundemitor);
		free(map->trigger);
	}

	

	// Convert Windows path separators to Unix (from backup maps)
	char fixed_path[256];
	strncpy(fixed_path, file, sizeof(fixed_path) - 1);
	fixed_path[sizeof(fixed_path) - 1] = '\0';
	for(char *p = fixed_path; *p; p++) {
		if(*p == '\\') *p = '/';
	}
	
	f = fopen(fixed_path, "rb");
	if(f==NULL){
		log_error("Could not open map file '%s' (original: '%s')", fixed_path, file);
		sprintf(fiend_errorcode,"couldn't load %s",file);
		return 0;
	}

	// IMPORTANT: The map files were created on a 32-bit system where pointers are 4 bytes.
	// On 64-bit systems, pointers are 8 bytes, causing struct layout mismatch.
	// We manually read each field and skip over the 32-bit pointers (4 bytes each)
	// instead of using fread() on the entire MAP_DATA struct.
	// This ensures compatibility between 32-bit map files and 64-bit builds.
	
	temp_map = calloc(sizeof(MAP_DATA),1);
	
	// Read the fixed-size fields at the beginning
	fread(temp_map->name, sizeof(char), 40, f);  // name[40]
	temp_map->name[39] = '\0';  // Ensure null termination
	fread(&temp_map->w, sizeof(int), 1, f);      // w
	fread(&temp_map->h, sizeof(int), 1, f);      // h
	fread(&temp_map->player_x, sizeof(int), 1, f);     // player_x
	fread(&temp_map->player_y, sizeof(int), 1, f);     // player_y
	fread(&temp_map->player_angle, sizeof(int), 1, f); // player_angle
	fread(&temp_map->outside, sizeof(int), 1, f);      // outside
	fread(&temp_map->light_level, sizeof(int), 1, f);  // light_level
	
	fread(&temp_map->num_of_lights, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (light)
	
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer1)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer2)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (layer3)
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (shadow)
	
	fread(&temp_map->num_of_objects, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (object)
	
	fread(&temp_map->num_of_areas, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (area)
	
	fread(&temp_map->num_of_look_at_areas, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (look_at_area)
	
	fread(&temp_map->num_of_links, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (link)
	
	fread(&temp_map->num_of_soundemitors, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (soundemitor)
	
	fread(&temp_map->num_of_triggers, sizeof(int), 1, f);
	fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer (trigger)
	
	fread(temp_map->var, sizeof(VARIABLE_DATA), LOCAL_VAR_NUM, f);  // var array
	
	// DEBUG: Show local variables loaded from map file
	log_debug("========== LOADED LOCAL VARS FROM MAP FILE ==========");
	for(i=0; i<LOCAL_VAR_NUM; i++)
	{
		if(strcmp(temp_map->var[i].name, "null") != 0)
		{
			log_debug("Var[%d]: name='%s' value=%d", 
				i, temp_map->var[i].name, temp_map->var[i].value);
		}
	}
	log_debug("=====================================================");
	
	fread(&temp_map->num_of_path_nodes, sizeof(int), 1, f);
	fread(temp_map->path_node, sizeof(PATH_NODE), MAX_PATHNODE_NUM, f);  // path_node array
	
	map->w = temp_map->w;
	map->h = temp_map->h;
	
	log_debug("load_edit_map: Loaded map, w=%d h=%d (temp: w=%d h=%d)", 
		map->w, map->h, temp_map->w, temp_map->h);
	
	map->num_of_lights = temp_map->num_of_lights;
	map->num_of_areas = temp_map->num_of_areas;
	map->num_of_look_at_areas = temp_map->num_of_look_at_areas;
	map->num_of_links = temp_map->num_of_links;
	map->num_of_objects = temp_map->num_of_objects;
	map->num_of_soundemitors = temp_map->num_of_soundemitors;
	map->num_of_triggers = temp_map->num_of_triggers;
	map->num_of_path_nodes = temp_map->num_of_path_nodes;
	
	map->player_x = temp_map->player_x;
	map->player_y = temp_map->player_y;
	map->player_angle = temp_map->player_angle;
	
	map->light_level = temp_map->light_level;
	map->outside = temp_map->outside;
	
	strncpy(map->name, temp_map->name, 39);
	map->name[39] = '\0';

	memcpy(map->var, temp_map->var, sizeof(VARIABLE_DATA)*LOCAL_VAR_NUM);
	memcpy(map->path_node, temp_map->path_node, sizeof(PATH_NODE)*MAX_PATHNODE_NUM);
	
	free(temp_map);
	
	//stop doing that funny stuff----
	
	// Validate map dimensions and counts to prevent heap corruption
	if(map->w < 1 || map->w > 1000 || map->h < 1 || map->h > 1000) {
		log_error("Invalid map dimensions: w=%d h=%d", map->w, map->h);
		fclose(f);
		return 0;
	}
	if(map->num_of_lights < 0 || map->num_of_lights > 10000 ||
	   map->num_of_objects < 0 || map->num_of_objects > 10000 ||
	   map->num_of_areas < 0 || map->num_of_areas > 10000 ||
	   map->num_of_look_at_areas < 0 || map->num_of_look_at_areas > 10000 ||
	   map->num_of_links < 0 || map->num_of_links > 10000 ||
	   map->num_of_soundemitors < 0 || map->num_of_soundemitors > 10000 ||
	   map->num_of_triggers < 0 || map->num_of_triggers > 10000) {
		log_error("Invalid map counts: lights=%d objects=%d areas=%d look_at=%d links=%d sounds=%d triggers=%d",
			map->num_of_lights, map->num_of_objects, map->num_of_areas, map->num_of_look_at_areas,
			map->num_of_links, map->num_of_soundemitors, map->num_of_triggers);
		fclose(f);
		return 0;
	}
	
	// Allocate memory for the new map data
	log_debug("Allocating new map arrays for %d objects", map->num_of_objects);
	map->light = calloc(sizeof(LIGHT_DATA), map->num_of_lights);
	map->layer1 = calloc(sizeof(TILE_DATA), map->w*map->h);
	map->layer2 = calloc(sizeof(TILE_DATA), map->w*map->h);
	map->layer3 = calloc(sizeof(TILE_DATA), map->w*map->h);
	map->shadow = calloc(sizeof(char), map->w*map->h);
	map->object = calloc(sizeof(OBJECT_DATA), map->num_of_objects);
	log_debug("New map->object allocated at %p", (void*)map->object);
	map->area = calloc(sizeof(AREA_DATA), map->num_of_areas);
	map->look_at_area = calloc(sizeof(LOOK_AT_AREA_DATA), map->num_of_look_at_areas);
	map->link = calloc(sizeof(LINK_DATA), map->num_of_links);
	map->soundemitor = calloc(sizeof(SOUNDEMITOR_DATA), map->num_of_soundemitors);
	map->trigger = calloc(sizeof(TRIGGER_DATA), map->num_of_triggers);
	
	fread(map->light, sizeof(LIGHT_DATA), map->num_of_lights, f);
	
	size_t tiles_read1 = fread(map->layer1, sizeof(TILE_DATA), map->w*map->h,f);
	size_t tiles_read2 = fread(map->layer2, sizeof(TILE_DATA), map->w*map->h,f);
	size_t tiles_read3 = fread(map->layer3, sizeof(TILE_DATA), map->w*map->h,f);
	
	if(tiles_read1 != (size_t)(map->w*map->h) || tiles_read2 != (size_t)(map->w*map->h) || tiles_read3 != (size_t)(map->w*map->h)) {
		log_error("Incomplete tile data read: expected %d tiles, got layer1=%zu layer2=%zu layer3=%zu",
			map->w*map->h, tiles_read1, tiles_read2, tiles_read3);
		fclose(f);
		return 0;
	}
	
	// DEBUG: Sample some tile data to see if it looks valid
	log_debug("========== TILE DATA SAMPLE (load_edit_map) ==========");
	log_debug("sizeof(TILE_DATA) = %zu bytes", sizeof(TILE_DATA));
	log_debug("Map dimensions: %d x %d = %d tiles", map->w, map->h, map->w * map->h);
	log_debug("Sample tiles from layer1 at positions:");
	for(int sample_i = 0; sample_i < 5 && sample_i < map->w * map->h; sample_i++) {
		log_debug("  [%d]: tile_set=%d, tile_num=%d", 
			sample_i, map->layer1[sample_i].tile_set, map->layer1[sample_i].tile_num);
	}
	log_debug("======================================================");
	
	fread(map->shadow, sizeof(char), map->w*map->h,f);
	
	fread(map->object, sizeof(OBJECT_DATA),map->num_of_objects, f);
	
	fread(map->area, sizeof(AREA_DATA),map->num_of_areas, f);
	
	fread(map->look_at_area, sizeof(LOOK_AT_AREA_DATA),map->num_of_look_at_areas, f);
	
	fread(map->link, sizeof(LINK_DATA),map->num_of_links, f);

	fread(map->soundemitor, sizeof(SOUNDEMITOR_DATA),map->num_of_soundemitors, f);
	
	// DEBUG: Check file position before reading triggers
	log_debug("========== BEFORE READING TRIGGERS (load_edit_map) ==========");
	log_debug("File position: 0x%lX (%ld)", ftell(f), ftell(f));
	log_debug("About to read %d triggers of size %zu bytes each", 
		map->num_of_triggers, sizeof(TRIGGER_DATA));
	log_debug("Expected to read %zu bytes total", 
		map->num_of_triggers * sizeof(TRIGGER_DATA));
	log_debug("============================================================");
	
	fread(map->trigger, sizeof(TRIGGER_DATA),map->num_of_triggers, f);
	
	// DEBUG: Print what triggers we loaded
	log_debug("========== LOADED TRIGGERS (load_edit_map) ==========");
	log_debug("File position after reading triggers: %ld", ftell(f));
	log_debug("Number of triggers: %d", map->num_of_triggers);
	for(i=0; i<map->num_of_triggers && i<5; i++) {
		log_debug("Trigger[%d]: name='%s' active=%d type=%d", 
			i, map->trigger[i].name, map->trigger[i].active, map->trigger[i].type);
	}
	log_debug("====================================================");
				
	
	map_x=0;
    map_y=0;
    
	if(in_the_game)
	{
		for(i=0;i<map->num_of_lights;i++)
			create_light_map2(&lightmap_data[i], &map->light[i]);

		sprintf(map_file,"%s",file);
	}
	else
	{
		for(i=0;i<map->num_of_lights;i++)
			create_light_map(&lightmap_data[i], &map->light[i]);
	}

		
	
	fclose(f);

	update_tile_object_height();

	return 1;

}





//this is being lazy....I use fixed sizes on all of the data...
//these fixed sizes are the max size.. so the map takes up alot of 
//memory, but it is saved in as compact size as possible.....
void new_map(MAP_DATA* map)
{
 int i;

 strcpy(map->name, "noname");

 map->light = calloc(sizeof(LIGHT_DATA), MAX_LIGHT_NUM); 
 
 map->num_of_lights = 0;
 
 map->light_level=31;

 map->h = 20;
 map->w = 20;

 map->player_x = 80;
 map->player_y = 80;
 
 map->player_angle = 0;

 map->outside = 0;


 
 map->layer1 = calloc(sizeof(TILE_DATA), MAX_LAYER_H*MAX_LAYER_W); 
 map->layer2 = calloc(sizeof(TILE_DATA), MAX_LAYER_H*MAX_LAYER_W); 
 map->layer3 = calloc(sizeof(TILE_DATA), MAX_LAYER_H*MAX_LAYER_W); 

 map->shadow = calloc(sizeof(char), MAX_LAYER_H*MAX_LAYER_W); 
				
 map->object = calloc(sizeof(OBJECT_DATA), MAX_OBJECT_NUM); 
 map->area = calloc(sizeof(AREA_DATA), MAX_AREA_NUM); 
 map->look_at_area = calloc(sizeof(LOOK_AT_AREA_DATA), MAX_LOOK_AT_AREA_NUM); 
 map->link = calloc(sizeof(LINK_DATA), MAX_LINK_NUM); 
 map->soundemitor = calloc(sizeof(SOUNDEMITOR_DATA), MAX_SOUNDEMITOR_NUM); 
 map->trigger = calloc(sizeof(TRIGGER_DATA), MAX_TRIGGER_NUM); 

 
 
  map->num_of_objects = 0;
  map->num_of_areas = 0;
  map->num_of_look_at_areas = 0;
  map->num_of_links =0;
  map->num_of_soundemitors =0;
  map->num_of_triggers =0;
  map->num_of_path_nodes =0;
	
 for(i=0;i<LOCAL_VAR_NUM;i++)
 {
	map->var[i].value=0;
	sprintf(map->var[i].name,"null",i);
 }

 for(i=0;i<MAX_PATHNODE_NUM;i++)
 {
	map->path_node[i].used=0;
	map->path_node[i].x=0;
	map->path_node[i].y=0;

 }
 
  
 for(i=0;i<MAX_LAYER_H*MAX_LAYER_W;i++)
  {
    map->layer1[i].tile_set=0;
	map->layer1[i].tile_num=2;
	

	map->layer2[i].tile_set=0;
	map->layer2[i].tile_num=0;
	
	map->layer3[i].tile_set=0;
	map->layer3[i].tile_num=0;

	map->shadow[i]=-1;

	
  }

	/*acquire_screen();
	for(i=0;i<40;i++)
	textprintf(screen,font,10+i*27,200,makecol(255,40,0),"%d-%d,",map->layer1[i].tile_set, map->layer1[i].tile_num);
	release_screen();
	readkey();*/
}



//reset or clear some varialbles
void clear_map(MAP_DATA *map)
{
 int i;

 for(i=0;i<map->num_of_lights;i++)
	destroy_bitmap(lightmap_data[i]);
	
 strcpy(map->name,"noname");
 
 map->light_level=31;
 map->outside = 0;
 
 map->h=20;
 map->w=20;

 map->player_x = 80;
 map->player_y = 80;

 map->player_angle = 0;


 map->num_of_lights = 0;
 
 map->num_of_objects = 0;
 map->num_of_areas = 0;
 map->num_of_look_at_areas = 0;
 map->num_of_links =0;
 map->num_of_soundemitors =0;
 map->num_of_triggers =0;
 map->num_of_path_nodes =0;

 
 for(i=0;i<LOCAL_VAR_NUM;i++)
 {
	map->var[i].value=0;
	sprintf(map->var[i].name,"null");
 }

 for(i=0;i<MAX_PATHNODE_NUM;i++)
 {
	map->path_node[i].used=0;
	map->path_node[i].x=0;
	map->path_node[i].y=0;
 }
  
 for(i=0;i<MAX_LAYER_H*MAX_LAYER_W;i++)
  {
    map->layer1[i].tile_set=0;
	map->layer1[i].tile_num=2;
	

	map->layer2[i].tile_set=0;
	map->layer2[i].tile_num=0;
	
	map->layer3[i].tile_set=0;
	map->layer3[i].tile_num=0;
	
	map->shadow[i]=-1;
	
  }


 map_x=0;
 map_y=0;


}



//frees the memory that map has taken up
//NOT the MAP_DATA struct though
void release_map(MAP_DATA *map)
{
  int i;
 	
  for(i=0;i<map->num_of_lights;i++) {
	if(lightmap_data[i]) {  // Check for NULL to prevent double-free
		destroy_bitmap(lightmap_data[i]);
		lightmap_data[i] = NULL;
	}
  }

 
	
  free(map->light);
  map->light = NULL;
  
  free(map->layer1);
  map->layer1 = NULL;
  free(map->layer2);
  map->layer2 = NULL;
  free(map->layer3);
  map->layer3 = NULL;

  free(map->shadow);
  map->shadow = NULL;

  free(map->object);
  map->object = NULL;
  free(map->area);
  map->area = NULL;
  free(map->look_at_area);
  map->look_at_area = NULL;
  free(map->link);
  map->link = NULL;
  free(map->soundemitor);
  map->soundemitor = NULL;
  free(map->trigger);
  map->trigger = NULL;
  
  
}




