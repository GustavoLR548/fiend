////////////////////////////////////////////////////
// Path utility functions for cross-platform compatibility
//
// Ensures consistent path handling across Windows and Unix-like systems
///////////////////////////////////////////////////

#include <string.h>
#include "path_utils.h"

/* ensure_trailing_slash:
 * Ensures the path ends with a forward slash.
 * Forward slashes work on all platforms including Windows with Allegro.
 */
void ensure_trailing_slash(char *path)
{
	size_t len = strlen(path);
	if (len > 0 && path[len - 1] != '/' && path[len - 1] != '\\') {
		strcat(path, "/");
	}
}
