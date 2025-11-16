////////////////////////////////////////////////////
// Batch utility functions for map editing
// 
// Provides functionality to batch update music file extensions
// across multiple map files
////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../fiend.h"
#include "../trigger.h"

// Helper function to check if a path is a directory
static int is_directory(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

// Helper function to replace file extension in a string
// Returns 1 if extension was found and replaced, 0 otherwise
static int replace_music_extension(char *filename, const char *old_ext, const char *new_ext)
{
	int len = strlen(filename);
	int old_ext_len = strlen(old_ext);
	int new_ext_len = strlen(new_ext);
	
	// Check if the filename ends with old_ext (case insensitive)
	if (len < old_ext_len)
		return 0;
	
	char *ext_pos = filename + len - old_ext_len;
	
	// Case insensitive comparison
	int match = 1;
	for (int i = 0; i < old_ext_len; i++) {
		char c1 = ext_pos[i];
		char c2 = old_ext[i];
		// Convert to lowercase for comparison
		if (c1 >= 'A' && c1 <= 'Z') c1 = c1 - 'A' + 'a';
		if (c2 >= 'A' && c2 <= 'Z') c2 = c2 - 'A' + 'a';
		if (c1 != c2) {
			match = 0;
			break;
		}
	}
	
	if (!match)
		return 0;
	
	// Replace the extension
	// Make sure we don't overflow the buffer (assuming 40 char max)
	if (len - old_ext_len + new_ext_len >= 40) {
		fprintf(stderr, "Error: New filename would be too long: %s\n", filename);
		return 0;
	}
	
	strcpy(ext_pos, new_ext);
	// Ensure null termination
	filename[len - old_ext_len + new_ext_len] = '\0';
	
	return 1;
}

// Process a single map file
static int process_map_file(const char *filepath, const char *old_ext, const char *new_ext, int *total_changes)
{
	int changes_in_file = 0;
	MAP_DATA temp_map;
	
	printf("Processing: %s\n", filepath);
	
	// Load the map
	if (!load_edit_map(&temp_map, (char *)filepath)) {
		fprintf(stderr, "  Error: Failed to load map file\n");
		return 0;
	}
	
	// Iterate through all triggers
	for (int t = 0; t < temp_map.num_of_triggers; t++) {
		TRIGGER_DATA *trigger = &temp_map.trigger[t];
		
		// Check all events in this trigger
		for (int e = 0; e < MAX_EVENT_NUM; e++) {
			EVENT_DATA *event = &trigger->event[e];
			
			// Check if this is a PLAY_MUSIC event (type 68)
			if (event->used && event->type == EVENT_PLAY_MUSIC) {
				// Try to replace the extension in string1 (the music filename)
				if (replace_music_extension(event->string1, old_ext, new_ext)) {
					printf("  Trigger '%s', Event %d: %s\n", 
					       trigger->name, e, event->string1);
					changes_in_file++;
				}
			}
		}
	}
	
	// If we made changes, save the map
	if (changes_in_file > 0) {
		if (!save_map(&temp_map, (char *)filepath)) {
			fprintf(stderr, "  Error: Failed to save map file\n");
			return 0;
		}
		printf("  Saved %d changes\n", changes_in_file);
		*total_changes += changes_in_file;
	} else {
		printf("  No changes needed\n");
	}
	
	return 1;
}

// Recursively process all .map files in a directory
static int process_directory(const char *dirpath, const char *old_ext, const char *new_ext, int *total_changes)
{
	DIR *dir;
	struct dirent *entry;
	char filepath[512];
	
	dir = opendir(dirpath);
	if (!dir) {
		fprintf(stderr, "Error: Cannot open directory: %s\n", dirpath);
		return 0;
	}
	
	while ((entry = readdir(dir)) != NULL) {
		// Skip . and ..
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		
		// Build full path
		snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);
		
		if (is_directory(filepath)) {
			// Recursively process subdirectory
			process_directory(filepath, old_ext, new_ext, total_changes);
		} else {
			// Check if it's a .map file
			int len = strlen(entry->d_name);
			if (len > 4 && strcmp(entry->d_name + len - 4, ".map") == 0) {
				process_map_file(filepath, old_ext, new_ext, total_changes);
			}
		}
	}
	
	closedir(dir);
	return 1;
}

// Main batch replacement function
// Searches folder recursively for .map files and replaces old_ext with new_ext
// in all EVENT_PLAY_MUSIC events
int batch_replace_music_extension(const char *folder, const char *old_ext, const char *new_ext)
{
	int total_changes = 0;
	
	printf("\n=== Batch Music Extension Replacement ===\n");
	printf("Folder: %s\n", folder);
	printf("Replacing: '%s' -> '%s'\n\n", old_ext, new_ext);
	
	if (!is_directory(folder)) {
		fprintf(stderr, "Error: Not a valid directory: %s\n", folder);
		return 0;
	}
	
	// Process all files in the directory tree
	if (!process_directory(folder, old_ext, new_ext, &total_changes)) {
		fprintf(stderr, "Error occurred during processing\n");
		return 0;
	}
	
	printf("\n=== Complete ===\n");
	printf("Total changes made: %d\n", total_changes);
	
	return 1;
}
