////////////////////////////////////////////////////
// Batch utility functions for map editing
////////////////////////////////////////////////////

#ifndef BATCH_UTIL_H
#define BATCH_UTIL_H

// Batch replace music file extensions in all maps within a folder
// Recursively searches for .map files and updates EVENT_PLAY_MUSIC events
// Returns 1 on success, 0 on failure
int batch_replace_music_extension(const char *folder, const char *old_ext, const char *new_ext);

#endif
