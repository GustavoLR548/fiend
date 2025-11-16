////////////////////////////////////////////////////
// Path utility functions for cross-platform compatibility
//
// Ensures consistent path handling across Windows and Unix-like systems
///////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include "path_utils.h"

/* ensure_trailing_slash:
 * Ensures the path ends with a platform-appropriate path separator.
 */
void ensure_trailing_slash(char *path)
{
	size_t len = strlen(path);
	if (len > 0 && path[len - 1] != PATH_SEP && path[len - 1] != '/' && path[len - 1] != '\\') {
		/* Add platform-appropriate separator */
		strcat(path, PATH_SEP_STR);
	}
	/* If path ends with wrong separator, replace it with correct one */
	else if (len > 0 && path[len - 1] != PATH_SEP) {
		path[len - 1] = PATH_SEP;
	}
}

/* normalize_path:
 * Converts path separators to the platform-appropriate format.
 * Windows requires backslashes, Unix systems use forward slashes.
 */
void normalize_path(char *path)
{
	char *p = path;
	while (*p) {
#ifdef _WIN32
		/* On Windows, convert forward slashes to backslashes */
		if (*p == '/') {
			*p = '\\';
		}
#else
		/* On Unix, convert backslashes to forward slashes */
		if (*p == '\\') {
			*p = '/';
		}
#endif
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
