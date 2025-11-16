////////////////////////////////////////////////////
// Path utility functions for cross-platform compatibility
//
// Ensures consistent path handling across Windows and Unix-like systems
///////////////////////////////////////////////////

#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <stddef.h>

/* Platform-specific path separator */
#ifdef _WIN32
    #define PATH_SEP '\\'
    #define PATH_SEP_STR "\\"
#else
    #define PATH_SEP '/'
    #define PATH_SEP_STR "/"
#endif

/* ensure_trailing_slash:
 * Ensures the path ends with the platform-appropriate path separator.
 * 
 * Parameters:
 *   path - The path string to modify (must have space for one more character)
 */
void ensure_trailing_slash(char *path);

/* normalize_path:
 * Converts all path separators in a string to the platform-appropriate separator.
 * On Windows: converts / to \
 * On Unix: converts \ to /
 * 
 * Parameters:
 *   path - The path string to normalize (modified in place)
 */
void normalize_path(char *path);

/* build_path:
 * Safely builds a file path by combining directory and filename with proper separator.
 * Handles trailing slashes automatically and uses platform-appropriate separators.
 * 
 * Parameters:
 *   dest - Destination buffer
 *   dest_size - Size of destination buffer
 *   dir - Directory path
 *   filename - Filename to append
 * 
 * Returns:
 *   0 on success, -1 if the result would overflow the buffer
 */
int build_path(char *dest, size_t dest_size, const char *dir, const char *filename);

#endif /* PATH_UTILS_H */
