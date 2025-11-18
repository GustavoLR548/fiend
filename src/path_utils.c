////////////////////////////////////////////////////
// Path utility functions for cross-platform compatibility
//
// Ensures consistent path handling across Windows and Unix-like systems
///////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include "path_utils.h"

/* ensure_trailing_slash:
 * Ensures the path ends with a forward slash.
 * Forward slashes work on all platforms including Windows with Allegro.
 */
void ensure_trailing_slash(char *path)
{
	size_t len = strlen(path);
	if (len > 0 && path[len - 1] != '/' && path[len - 1] != '\\') {
		/* Always use forward slash for compatibility */
		strcat(path, "/");
	}
	/* If path ends with backslash, replace it with forward slash */
	else if (len > 0 && path[len - 1] == '\\') {
		path[len - 1] = '/';
	}
}

/* normalize_path:
 * Converts all path separators to forward slashes.
 * Forward slashes work on all platforms including Windows with Allegro.
 */
void normalize_path(char *path)
{
	char *p = path;
	while (*p) {
		/* Always convert to forward slashes - Allegro handles them on all platforms */
		if (*p == '\\') {
			*p = '/';
		}
		p++;
	}
}

/* build_path:
 * Safely builds a file path with proper separator and bounds checking.
 */
int build_path(char *dest, size_t dest_size, const char *dir, const char *filename)
{
	size_t dir_len = strlen(dir);
	size_t file_len = strlen(filename);
	size_t total_len = dir_len + file_len + 2; /* +2 for separator and null terminator */
	
	/* Check if result would overflow */
	if (total_len > dest_size) {
		return -1;
	}
	
	/* Copy directory */
	strncpy(dest, dir, dest_size - 1);
	dest[dest_size - 1] = '\0';
	
	/* Normalize the directory path separators */
	normalize_path(dest);
	
	/* Ensure trailing separator */
	ensure_trailing_slash(dest);
	
	/* Append filename using strncat for safety */
	strncat(dest, filename, dest_size - strlen(dest) - 1);
	
	/* Normalize the final path */
	normalize_path(dest);
	
	return 0;
}
