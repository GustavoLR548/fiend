////////////////////////////////////////////////////
// Path utility functions for cross-platform compatibility
//
// Ensures consistent path handling across Windows and Unix-like systems
///////////////////////////////////////////////////

#ifndef PATH_UTILS_H
#define PATH_UTILS_H

/* ensure_trailing_slash:
 * Ensures the path ends with a forward slash.
 * Forward slashes work on all platforms including Windows with Allegro.
 * 
 * Parameters:
 *   path - The path string to modify (must have space for one more character)
 */
void ensure_trailing_slash(char *path);

#endif /* PATH_UTILS_H */
