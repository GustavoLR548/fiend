# Fiend - Porting from Windows 32-bit (2001) to Linux 64-bit (2025)

This document comprehensively describes all changes made to successfully port the 2001 game "Fiend" from 32-bit Windows to modern 64-bit Linux systems.

## Table of Contents
- [Overview](#overview)
- [System Requirements](#system-requirements)
- [Build Instructions](#build-instructions)
- [Issues Fixed](#issues-fixed)
  - [1. Missing FMOD Audio Library](#1-missing-fmod-audio-library)
  - [2. Deprecated Allegro Functions](#2-deprecated-allegro-functions)
  - [3. Multiple Definition Linker Errors](#3-multiple-definition-linker-errors)
  - [4. fscanf Format String Bugs](#4-fscanf-format-string-bugs)
  - [5. Locale-Dependent Float Parsing](#5-locale-dependent-float-parsing-critical)
  - [6. 32-bit/64-bit Map File Compatibility](#6-32-bit64-bit-map-file-compatibility-critical)
  - [7. Black Screen Rendering Issue](#7-black-screen-rendering-issue)
  - [8. Trigger System Corruption](#8-trigger-system-corruption-critical)
  - [9. Save Game Directory Creation](#9-save-game-directory-creation)
  - [10. Memory Management and Double-Free Crashes](#10-memory-management-and-double-free-crashes)
- [Technical Deep Dives](#technical-deep-dives)
- [Testing Results](#testing-results)
- [Known Limitations](#known-limitations)
- [Future Improvements](#future-improvements)

---

## Overview

**Status: ✅ FULLY WORKING AND PLAYABLE!**

Fiend is a survival horror game originally developed in 2001 for 32-bit Windows. This port makes it run on modern 64-bit Linux systems while maintaining compatibility with the original game data files.

### What Works
- ✅ Complete compilation on modern GCC/Linux
- ✅ Loading original 32-bit Windows map files on 64-bit Linux
- ✅ All graphics rendering correctly
- ✅ Player movement and controls
- ✅ Complete trigger system (doors, NPCs, events, area transitions)
- ✅ Save/load game functionality
- ✅ Map editor tool
- ✅ Full gameplay from start to finish

### What Doesn't Work
- ⚠️ Audio (FMOD library unavailable, game runs silently)

---

## System Requirements

- **OS**: Linux (tested on Ubuntu 24.04 LTS)
- **Compiler**: GCC 13.3.0 or later
- **Architecture**: x86_64 (64-bit)
- **Libraries**: 
  - Allegro 4.4.3+ (`liballegro4-dev`)
  - CMake 3.5+
  - Standard build tools

---

## Build Instructions

### Install Dependencies
```bash
sudo apt-get install build-essential cmake liballegro4-dev
```

### Build the Game
```bash
cd /path/to/fiend
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### Run the Game
```bash
cd ../release
./fiend
```

### Convenient Makefile Targets
A Makefile is provided with helpful shortcuts:
```bash
make build      # Full build
make run        # Build and run
make run-log    # Build and run with logging
make quick      # Quick rebuild (skip cmake)
make gdb        # Run with GDB debugger
make editor     # Build and run map editor
make help       # Show all targets
```

---

## Issues Fixed

### 1. Missing FMOD Audio Library

**Problem**: The game used FMOD Ex audio library which is no longer available in modern Linux package repositories.

**Solution**: Made FMOD optional via conditional compilation using `USE_FMOD` preprocessor directive.

**Files Modified**:
- `CMakeLists.txt` - Changed FMOD from REQUIRED to optional
- `src/fiend.h` - Wrapped FMOD includes and declarations
- `src/sound.h` - Wrapped FMOD type definitions
- `src/fiend.c` - Wrapped initialization/cleanup
- `src/fiend/console_funcs.c` - Wrapped sound control
- `src/fiend/soundplay.c` - Wrapped sound playback
- `src/fiend/savegame.c` - Wrapped sound state saving

**Impact**: Game compiles and runs without audio. Future versions could integrate modern audio libraries.

---

### 2. Deprecated Allegro Functions

**Problem**: Allegro 4.4 deprecated fixed-point math functions like `fatan()` and `fsin()`.

**Solution**: Replaced with standard C math library equivalents.

**Changes**:
- `src/grafik4.c`: `fatan(...)` → `atan2(...)`
- `src/fiend/menu.c`: `fsin(degree_to_fixed(...))` → `sin(...*M_PI/180.0)` (4 locations)
- Added `-lm` (math library) to linker flags

---

### 3. Multiple Definition Linker Errors

**Problem**: Global variables were defined in header files, causing multiple definition errors during linking.

**Solution**: Changed definitions to `extern` declarations in headers, kept actual definitions in .c files.

**Files Modified**:
- `src/fiend.h` - Changed to extern declarations
- `src/fiend.c` - Added actual definitions (gun_fired, npc_damaged, outside_lightlevel, etc.)

---

### 4. fscanf Format String Bugs

**Problem**: Several files had fscanf calls with mismatched format strings and arguments, particularly an extra `buffer` argument with no corresponding format specifier.

**Example**: 
```c
// Before (wrong)
fscanf(f, "%d ", &array[k], buffer);

// After (correct)
fscanf(f, "%d", &array[k]);
```

**Files Fixed**:
- `src/character.c` - Animation frame loading
- `src/enemy.c` - Animation frame loading
- `src/object.c` - Object animation loading
- `src/fiend/particle.c` - Particle animation loading

---

### 5. Locale-Dependent Float Parsing (CRITICAL)

**Problem**: When system locale uses comma as decimal separator (e.g., pt_BR, de_DE, fr_FR), `fscanf` with `%f` fails to parse numbers with periods (e.g., "1.5"). This caused:
- fscanf to read "1" and leave ".5" in the stream
- Subsequent parsing to read ".5" as a string token
- Complete corruption of file parsing sequence
- Segmentation faults during character/enemy loading

**Root Cause**: The fscanf function respects the LC_NUMERIC locale category:
- In "C" locale: Period (.) is decimal separator
- In pt_BR/de_DE locale: Comma (,) is decimal separator

**Solution**: Set locale to "C" before parsing, restore original locale after.

**Files Modified**:
- `src/character.c`:
  ```c
  #include <locale.h>
  
  void load_characters(void) {
      char *old_locale = setlocale(LC_NUMERIC, NULL);
      setlocale(LC_NUMERIC, "C");
      
      // ... parsing code ...
      
      setlocale(LC_NUMERIC, old_locale);
  }
  ```
- `src/enemy.c` - Same pattern

**Impact**: Fixed critical runtime crash on non-C locales. Essential for international users.

---

### 6. 32-bit/64-bit Map File Compatibility (CRITICAL)

**Problem**: Original game saved map files using direct struct serialization on 32-bit Windows. The `MAP_DATA` structure contains 11 pointer fields:

```c
typedef struct {
    char name[40];
    int w, h;
    int player_x, player_y, player_angle;
    int outside, light_level;
    
    int num_of_lights;
    LIGHT_DATA *light;           // 4 bytes on 32-bit, 8 bytes on 64-bit
    
    TILE_DATA *layer1;           // Pointer
    TILE_DATA *layer2;           // Pointer
    TILE_DATA *layer3;           // Pointer
    char *shadow;                // Pointer
    
    int num_of_objects;
    OBJECT_DATA *object;         // Pointer
    
    int num_of_areas;
    AREA_DATA *area;             // Pointer
    
    int num_of_look_at_areas;
    LOOK_AT_AREA_DATA *look_at_area;  // Pointer
    
    int num_of_links;
    LINK_DATA *link;             // Pointer
    
    int num_of_soundemitors;
    SOUNDEMITOR_DATA *soundemitor;    // Pointer
    
    int num_of_triggers;
    TRIGGER_DATA* trigger;       // Pointer
    
    // ... more fields ...
} MAP_DATA;
```

**Size Mismatch**:
- 32-bit: `sizeof(MAP_DATA)` ≈ 7024 bytes (pointers = 4 bytes each)
- 64-bit: `sizeof(MAP_DATA)` = 7068 bytes (pointers = 8 bytes each)

When loading a 32-bit map file on 64-bit, `fread()` expects 7068 bytes but the file contains 7024 bytes with different field offsets. All fields after the first pointer are read from wrong positions, resulting in garbage data.

**Symptoms Before Fix**:
- Crash after intro when loading maps
- Garbage values like `num_of_objects = 55883500` (should be ~40)
- Invalid object types like `1634627438` (ASCII "atnn" from misaligned data)
- Segmentation faults accessing arrays with garbage indices

**Solution**: Manual field-by-field reading in `load_edit_map()` (`src/mapio.c`):

```c
// Read individual scalar fields
fread(temp_map->name, sizeof(char), 40, f);
fread(&temp_map->w, sizeof(int), 1, f);
fread(&temp_map->h, sizeof(int), 1, f);
fread(&temp_map->player_x, sizeof(int), 1, f);
fread(&temp_map->player_y, sizeof(int), 1, f);
fread(&temp_map->player_angle, sizeof(int), 1, f);
fread(&temp_map->outside, sizeof(int), 1, f);
fread(&temp_map->light_level, sizeof(int), 1, f);

// Read counts, then skip the 32-bit pointers (4 bytes each)
fread(&temp_map->num_of_lights, sizeof(int), 1, f);
fseek(f, 4, SEEK_CUR); // Skip 32-bit pointer

fseek(f, 4, SEEK_CUR); // Skip layer1 pointer
fseek(f, 4, SEEK_CUR); // Skip layer2 pointer
fseek(f, 4, SEEK_CUR); // Skip layer3 pointer
fseek(f, 4, SEEK_CUR); // Skip shadow pointer

fread(&temp_map->num_of_objects, sizeof(int), 1, f);
fseek(f, 4, SEEK_CUR); // Skip object pointer

// ... repeat for all 11 pointers ...

// Copy values to actual map structure
map->w = temp_map->w;
map->h = temp_map->h;
map->num_of_lights = temp_map->num_of_lights;
// ... etc ...

// Allocate new memory for the correct sizes
map->light = calloc(sizeof(LIGHT_DATA), map->num_of_lights);
map->layer1 = calloc(sizeof(TILE_DATA), map->w * map->h);
// ... etc ...

// Now read the actual data arrays
fread(map->light, sizeof(LIGHT_DATA), map->num_of_lights, f);
fread(map->layer1, sizeof(TILE_DATA), map->w * map->h, f);
fread(map->object, sizeof(OBJECT_DATA), map->num_of_objects, f);
// ... etc ...
```

**Important Notes**:
1. **Map Files**: Now compatible! Can load 32-bit Windows maps on 64-bit Linux
2. **Savegames**: NOT portable between architectures (still uses struct serialization with pointers)
3. **Map Editor**: Uses same function, automatically benefits from fix

**Results After Fix**:
- ✅ Maps load correctly (w=64 h=50, num_objects=39, etc.)
- ✅ All object data valid
- ✅ No crashes during map loading
- ✅ Map editor can load and edit maps

---

### 7. Black Screen Rendering Issue

**Problem**: After fixing map loading, the game loaded all data correctly and executed the rendering pipeline, but the screen remained completely black.

**Investigation**: 
- Map data was correct (verified via debug output)
- Tileset loading worked (33 tilesets)
- Virtual screen buffer created (480x480)
- All rendering functions executing
- **BUT**: `screen_is_black` flag was set to 1

**Root Cause**: The fade-out effect from menu/intro sets `screen_is_black = 1` to gradually darken the screen. This flag was never reset during game initialization.

In `src/fiend/draw_level.c`:
```c
if(screen_is_black) { 
    clear(screen);  // Just clears to black - nothing renders!
}
else {
    blit(virt, screen, ...);  // Actually displays the game
}
```

The `fiend_fadein()` function would normally reset it, but that's only called by trigger events, not during initial setup.

**Solution**: Added one line to `src/fiend/main.c` after map loading:
```c
load_edit_map(map, map_name);
screen_is_black = 0;  // Reset fade flag so rendering works
fiend_fadein();
```

**Results**:
- ✅ Game renders correctly
- ✅ All graphics visible
- ✅ Player can see and play the game

**Lesson**: Sometimes the bug isn't in complex systems, but in a simple flag. The map editor working was the key hint that all data/rendering code was fine.

---

### 8. Trigger System Corruption (CRITICAL)

**Problem**: After fixing rendering, discovered:
- Trigger names corrupted ("ing" instead of "Nothing")
- Trigger types had impossible values
- "ACT I" title never appeared
- Doors didn't work
- Area transitions didn't work
- NPC interactions didn't work

**Investigation Journey**:

#### Phase 1: Structure Size Analysis
Initially suspected `TRIGGER_DATA` size was wrong. Tried various sizes (2788, 2780 bytes) with different padding combinations. Added extensive debug logging and hex dump analysis.

#### Phase 2: Binary File Analysis
Created test programs to search for known trigger names in map files. Found triggers at file position 104752, but code was reading from 104736 - a **16-byte gap** after soundemitor data.

#### Phase 3: The Gap Fix
Added `fseek(f, 16, SEEK_CUR)` after reading soundemitors:
- ✅ Trigger names loaded correctly!
- ✅ "ACT I" title appeared!
- ✅ Trigger data structure was correct
- ✗ **BUT doors and area triggers STILL didn't work!**

Deeper analysis revealed:
- File position was 0x19920 (104,736 bytes)
- But "forest1" trigger was at 0x18934 (100,660 bytes) in the file
- We were **4,076 bytes past** where triggers should start!
- Previous structures were being over-read by exactly 4,076 bytes

#### Phase 4: The Real Culprit - Structure Packing!

**Root Cause Found**: The `#pragma pack(1)` directive in `map.h` was forcing ALL structures to have NO padding:

Original 32-bit Windows MSVC compiler used **natural alignment** (4-byte boundaries):
- Natural alignment adds padding bytes to align int/float fields to 4-byte boundaries
- `#pragma pack(1)` removes ALL padding, making structures smaller than expected

Example:
```c
struct Example {
    char name[30];  // 30 bytes
    int x;          // 4 bytes
};

// Natural alignment (what Windows MSVC used):
// 30 bytes + 2 bytes padding + 4 bytes = 36 bytes

// With pragma pack(1) (what we incorrectly added):
// 30 bytes + 4 bytes = 34 bytes (WRONG!)
```

When reading 39 OBJECT_DATA structures, 10 LINK_DATA structures, etc., these small 2-4 byte differences per structure accumulated into a **4KB file position error** by the time we reached triggers!

**The Solution**: **REMOVED `#pragma pack(1)` from `map.h`**

Let the compiler use natural alignment:
- Natural alignment on 64-bit Linux GCC matches 32-bit Windows MSVC
- Structure sizes now match file format exactly
- Only MAP_DATA header needs special handling (for pointer size difference)
- Manual field-by-field reading handles that one specific case
- All other structures align perfectly without pragma tricks!

**Files Modified**:
- `src/map.h` - **REMOVED** `#pragma pack(1)`

**Results - COMPLETE SUCCESS**:
- ✅ All trigger data loads from correct file positions
- ✅ Trigger names, types, conditions, and events all correct
- ✅ Door triggers work (can enter/exit buildings!)
- ✅ Area transition triggers work (map changes!)
- ✅ NPC dialogue triggers work
- ✅ Event triggers work (cutscenes, variable changes, etc.)
- ✅ ALL game mechanics now functional!

**The Critical Lesson**: **Never assume you need `#pragma pack` without evidence!** The original Windows code didn't use it, so we shouldn't either. Natural struct alignment is usually correct for cross-platform compatibility. The only exception is when dealing with pointer fields in serialized structures (like MAP_DATA header), which should be handled explicitly with manual field reading.

---

### 9. Save Game Directory Creation

**Problem**: Save game functionality appeared to succeed but no files were created. The `save/` directory didn't exist in the release folder.

**Solution**: Added automatic directory creation in `save_game()` function in `src/fiend/savegame.c`:

```c
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int save_game(char *file) {
    // Create save directory if it doesn't exist
    mkdir("save", 0755);
    
    sprintf(save_file, "save/%s", file);
    
    f = fopen(save_file, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Failed to create save file '%s': %s\n", 
                save_file, strerror(errno));
        return 0;
    }
    
    printf("Saving game to '%s'...\n", save_file);
    // ... save code ...
    printf("Game saved successfully to '%s'\n", save_file);
}
```

**Added to .gitignore**:
```
save/
```

**Results**:
- ✅ Save directory automatically created
- ✅ Save files created successfully
- ✅ Load game functionality works
- ✅ Proper error reporting with errno/strerror

---

### 10. Memory Management and Double-Free Crashes

**Problem**: After implementing save game functionality, the game crashed with "double free or corruption (!prev)" error when transitioning between maps (specifically entering george1.map from lauder.map).

**Investigation with GDB**:
```
#10 destroy_bitmap() from /lib/x86_64-linux-gnu/liballegro.so.4.4
#11 exit_fiend() at /home/guara/code/fiend/src/fiend.c:313
#12 main() at /home/guara/code/fiend/src/fiend/main.c:247
```

The crash was in `exit_fiend()` when calling `destroy_bitmap()`, not during map transition!

**Root Cause**: 
1. When transitioning maps, `load_edit_map()` destroys old lightmap bitmaps:
   ```c
   for(i=0; i<map->num_of_lights; i++)
       destroy_bitmap(lightmap_data[i]);
   ```

2. But it doesn't NULL the `lightmap_data[]` pointers after destroying them

3. `map->num_of_lights` gets updated to the new map's light count

4. When the game exits, `exit_fiend()` tries to destroy lightmaps again:
   ```c
   for(i=0; i<map->num_of_lights; i++)
       destroy_bitmap(lightmap_data[i]);  // Double free!
   ```

5. The `lightmap_data[i]` pointers are stale/invalid → double-free crash

**Example Scenario**:
- Transition from lauder.map (0 lights) → george1.map (1 light)
- `load_edit_map()` tries to destroy 0 lightmaps (using old count)
- Creates 1 new lightmap for george1
- `exit_fiend()` tries to destroy 1 lightmap
- BUT `lightmap_data[0]` points to freed memory → crash!

**Solution**: 

1. **In `load_edit_map()` (`src/mapio.c`)**: NULL pointers after destroying:
   ```c
   if(map->light != NULL) {
       for(i=0; i<map->num_of_lights; i++) {
           destroy_bitmap(lightmap_data[i]);
           lightmap_data[i] = NULL;  // Prevent double-free
       }
       // ... free other memory ...
   }
   
   // ... later, allocate new memory ...
   map->light = calloc(sizeof(LIGHT_DATA), map->num_of_lights);
   map->layer1 = calloc(sizeof(TILE_DATA), map->w * map->h);
   // ... etc ...
   ```

2. **In `exit_fiend()` (`src/fiend.c`)**: Check for NULL before destroying:
   ```c
   for(i=0; i<map->num_of_lights; i++)
       if(lightmap_data[i])  // Check for NULL
           destroy_bitmap(lightmap_data[i]);
   ```

3. **In `release_map()` (`src/mapio.c`)**: Same pattern:
   ```c
   for(i=0; i<map->num_of_lights; i++) {
       if(lightmap_data[i]) {
           destroy_bitmap(lightmap_data[i]);
           lightmap_data[i] = NULL;
       }
   }
   
   free(map->light);
   map->light = NULL;
   // ... free and NULL all pointers ...
   ```

**Results**:
- ✅ Map transitions work without crashes
- ✅ Can enter/exit buildings multiple times
- ✅ Game can be exited cleanly
- ✅ No memory leaks or double-free errors

**The Lesson**: Always NULL pointers after freeing them, especially in structures that persist across state changes. Check for NULL before freeing. This is defensive programming 101.

---

## Technical Deep Dives

### Structure Alignment and Binary Compatibility

**Key Insight**: Different compilers and architectures align structures differently. Original 32-bit Windows MSVC used 4-byte natural alignment. Modern 64-bit Linux GCC also uses natural alignment (matching MSVC behavior for most structures).

**What Works**:
- Let compiler use natural alignment (default behavior)
- Structures automatically align correctly between platforms
- No pragma directives needed for most structures

**What Doesn't Work**:
- Using `#pragma pack(1)` to force byte packing
- Makes structures smaller than file format expects
- Accumulates file position errors across multiple structures
- Breaks binary file compatibility

**Exception**: Pointer fields in serialized structures need special handling because pointer sizes differ (4 bytes vs 8 bytes). Solution: manual field-by-field reading for structures with pointers (like MAP_DATA header).

### Locale and Floating-Point Parsing

**The Problem**: 
```c
// In pt_BR locale
fscanf(f, "%f", &value);  // Expects comma: "1,5"
// But file contains: "1.5"
// Result: reads "1", leaves ".5" in stream
```

**The Fix**:
```c
char *old_locale = setlocale(LC_NUMERIC, NULL);
setlocale(LC_NUMERIC, "C");
// ... parse floats with fscanf ...
setlocale(LC_NUMERIC, old_locale);
```

**Why This Matters**: Games often contain numeric data files created by English-speaking developers. They use period as decimal separator. Without locale management, the game will crash on systems configured for other languages.

### Memory Management Best Practices

**Always**:
1. NULL pointers after freeing: `free(ptr); ptr = NULL;`
2. Check for NULL before freeing: `if(ptr) free(ptr);`
3. Free in reverse order of allocation
4. Track allocation counts and verify in cleanup

**Never**:
1. Use freed pointers
2. Assume pointer is NULL without explicit setting
3. Free the same pointer twice
4. Mix malloc/free with library-specific allocators

---

## Testing Results

### Compilation
- ✅ No errors
- ⚠️ Some warnings (format strings, unused variables) - non-critical
- ✅ Both executables build successfully
  - `fiend`: ~481KB
  - `mapeditor`: ~387KB

### Data Loading
- ✅ All character data (6 characters)
- ✅ All enemy data
- ✅ All object data
- ✅ All particle data
- ✅ All map files (32-bit Windows format on 64-bit Linux)
- ✅ Tilesets (33 loaded)

### Gameplay
- ✅ Intro cutscene with skip functionality
- ✅ Menu system
- ✅ Map rendering
- ✅ Player movement and controls
- ✅ Camera/look controls
- ✅ Collision detection
- ✅ Trigger system (doors, NPCs, events, area transitions)
- ✅ Save/load game
- ✅ Inventory system
- ✅ Combat system
- ✅ Map transitions
- ✅ Game exit without crashes

### Tools
- ✅ Map editor loads and displays maps
- ✅ All editing functions work

---

## Known Limitations

1. **No Audio**: FMOD library unavailable on modern Linux
   - Game runs silently
   - All sound/music functionality disabled
   - Could be replaced with OpenAL, SDL_mixer, etc. in future

2. **Savegame Portability**: NOT portable between 32-bit and 64-bit
   - Uses direct struct serialization with pointers
   - Savegames created on 64-bit Linux only work on 64-bit Linux
   - Map files ARE portable (fixed via manual reading)

3. **Debug Output**: Verbose stderr messages during gameplay
   - Helpful for troubleshooting
   - Could be removed or controlled via debug flag

4. **Fixed Resolution**: Original game resolution (640x480)
   - No widescreen support
   - No resolution options

---

## Future Improvements

### High Priority
1. **Audio System**: Replace FMOD with modern library
   - OpenAL for 3D audio
   - SDL_mixer for simple playback
   - libsndfile for file loading

2. **Portable Savegames**: Redesign save format
   - Use text-based or structured binary format
   - Avoid direct struct serialization
   - Support cross-platform save files

### Medium Priority
3. **Widescreen Support**: Add modern aspect ratios
   - 16:9, 16:10 support
   - Scalable UI
   - FOV adjustments

4. **Controller Support**: Gamepad/joystick input
   - SDL2 input system
   - Configurable button mapping

5. **Performance**: Profile and optimize
   - Modern OpenGL rendering
   - Multi-threading for loading
   - Improved draw call batching

### Low Priority
6. **Quality of Life**:
   - In-game graphics settings
   - Key rebinding
   - Mouse sensitivity options
   - Autosave functionality

7. **Code Cleanup**:
   - Remove debug output
   - Modernize C code style
   - Add comprehensive comments
   - Static analysis fixes

---

## Repository Information

- **Original Developer**: Magnus Norddahl (2001)
- **Current Port**: Gustavo (GustavoLR548) (2025)
- **Branch**: `feat/allegro4libsound`
- **Status**: ✅ **FULLY WORKING!**

---

## Appendix: File Modification Summary

### Core Game Files
- `src/fiend.h` - Extern declarations, FMOD wrapping
- `src/fiend.c` - Global definitions, FMOD wrapping, locale fixes
- `src/sound.h` - FMOD wrapping
- `src/map.h` - **REMOVED pragma pack(1)**
- `src/mapio.c` - Manual MAP_DATA reading, memory management fixes
- `src/character.c` - Locale fixes, fscanf fixes
- `src/enemy.c` - Locale fixes, fscanf fixes
- `src/object.c` - fscanf fixes
- `src/grafik4.c` - Deprecated function replacement
- `src/fiend/main.c` - Black screen fix, game loop
- `src/fiend/intro.c` - Skip functionality, bounds checking
- `src/fiend/draw_level.c` - Debug output
- `src/fiend/menu.c` - Deprecated function replacement
- `src/fiend/particle.c` - fscanf fixes
- `src/fiend/console_funcs.c` - FMOD wrapping
- `src/fiend/soundplay.c` - FMOD wrapping
- `src/fiend/savegame.c` - Directory creation, error handling, FMOD wrapping
- `src/fiend/ai.c` - Bounds checking
- `src/tile.c` - Debug output

### Build System
- `CMakeLists.txt` - FMOD optional, math library, debug flags
- `Makefile` - Convenient build targets, GDB support

### Data Files
- `release/graphic/characters/*.txt` - Fixed "run_step2" typo (6 files)

### Documentation
- `docs/PORTING_GUIDE.md` - This file (consolidated documentation)
- `README.md` - Updated with build instructions
- `.gitignore` - Added save/, build/, binaries

### Development Tools
- `tests/` - Various structure analysis and debug tools
- `.github/workflows/` - CI/CD for automated builds

---

## Contact and Contributions

For questions, issues, or contributions, please contact the repository owner or open an issue on GitHub.

**This port demonstrates that with careful analysis and systematic debugging, even complex 20+ year old games can be successfully brought to modern platforms while maintaining full compatibility with original game data.**
