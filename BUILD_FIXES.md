# Fiend - Build Fixes for Modern Linux (2024)

This document summarizes the changes made to build and run the 2001 game "Fiend" on a modern Linux system.

## System Information
- OS: Linux (tested on Ubuntu/Debian-based systems)
- Compiler: GCC 13.3.0
- Allegro Version: 4.4.3 (liballegro4-dev)
- Locale: pt_BR.UTF-8 (Brazilian Portuguese)

## Issues Fixed

### 1. Missing FMOD Audio Library
**Problem**: The game used FMOD Ex audio library which is no longer available in modern package repositories.

**Solution**: Made FMOD optional via conditional compilation using `USE_FMOD` preprocessor directive.

**Files Modified**:
- `CMakeLists.txt` - Changed FMOD from REQUIRED to optional
- `src/fiend.h` - Wrapped FMOD includes and extern declarations
- `src/sound.h` - Wrapped FMOD type definitions
- `src/fiend.c` - Wrapped FMOD initialization and cleanup code
- `src/fiend/console_funcs.c` - Wrapped FMOD sound control functions
- `src/fiend/soundplay.c` - Wrapped FMOD sound playback functions
- `src/fiend/savegame.c` - Wrapped FMOD sound state saving

### 2. Deprecated Allegro Functions
**Problem**: Allegro 4.4 deprecated fixed-point math functions like `fatan()` and `fsin()`.

**Solution**: Replaced with standard C math library equivalents.

**Changes**:
- `src/grafik4.c`: Replaced `fatan(...)` with `atan2(...)`
- `src/fiend/menu.c`: Replaced `fsin(degree_to_fixed(...))` with `sin(...*M_PI/180.0)` (4 occurrences)
- Added `-lm` (math library) to linker flags in CMakeLists.txt

### 3. Multiple Definition Linker Errors
**Problem**: Global variables were defined in header files, causing multiple definition errors during linking.

**Solution**: Changed definitions to `extern` declarations in headers, kept actual definitions in .c files.

**Files Modified**:
- `src/fiend.h` - Changed global variables from definitions to extern declarations
- `src/fiend.c` - Added actual definitions for global variables (gun_fired, npc_damaged, outside_lightlevel, etc.)

### 4. fscanf Format String Bugs
**Problem**: Several files had fscanf calls with mismatched format strings and arguments, particularly an extra `buffer` argument with no corresponding format specifier.

**Example**: `fscanf(f,"%d ",&array[k],buffer)` should be `fscanf(f,"%d",&array[k])`

**Files Fixed**:
- `src/character.c` - Lines around animation frame loading
- `src/enemy.c` - Similar animation frame loading
- `src/object.c` - Object animation loading
- `src/fiend/particle.c` - Particle animation loading

### 5. Locale-Dependent Float Parsing Bug ⚠️ **CRITICAL**
**Problem**: When system locale uses comma as decimal separator (e.g., pt_BR, de_DE, fr_FR), fscanf with `%f` fails to parse numbers with periods (e.g., "1.5"). This caused:
- fscanf to read "1" and leave ".5" in the stream
- Subsequent parsing to read ".5" as a string token
- Complete corruption of file parsing sequence
- Segmentation faults during character/enemy loading

**Solution**: Set locale to "C" before parsing, restore original locale after parsing.

**Files Fixed**:
- `src/character.c`:
  - Added `#include <locale.h>`
  - Added `setlocale(LC_NUMERIC, "C")` at start of `load_characters()`
  - Added `setlocale(LC_NUMERIC, old_locale)` before returning

- `src/enemy.c`:
  - Added `#include <locale.h>`
  - Added `setlocale(LC_NUMERIC, "C")` at start of `load_enemys()`
  - Added `setlocale(LC_NUMERIC, old_locale)` before returning

**Impact**: This was the root cause of the runtime segmentation fault. Without this fix, the game would crash during initialization on systems with non-C locales.

### 6. Data File Typo
**Problem**: Character data files had typo where "run_step2" was written as "run_step1" (duplicate label).

**Files Fixed**:
- `release/graphic/characters/nick.txt`
- `release/graphic/characters/george.txt`
- `release/graphic/characters/farmer_f1.txt`
- `release/graphic/characters/farmer_f2.txt`
- `release/graphic/characters/farmer_m1.txt`
- `release/graphic/characters/farmer_m2.txt`

Changed line: `walk_step1: 7 run_step1: 15` → `walk_step1: 7 run_step2: 15`

## Build Instructions

### 1. Install Dependencies
```bash
sudo apt-get install build-essential cmake liballegro4-dev
```

### 2. Build
```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run
```bash
cd ../release
./fiend
```

## Debug Configuration
The CMakeLists.txt has been configured with debug flags for troubleshooting:
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
```

## Known Limitations
1. **No Sound**: FMOD library is not available, so the game runs without audio
2. **Windows Line Endings**: Some data files have CRLF (\\r\\n) line endings, but this doesn't cause issues
3. **Allegro 4 Deprecation**: Allegro 4 is old but still maintained; future systems may require Allegro 5 port

## Testing Status
- ✅ Compiles without errors (warnings present but non-critical)
- ✅ Both executables build successfully (fiend: 481KB, mapeditor: 387KB)
- ✅ Loads all character data (6 characters)
- ✅ Loads all enemy data
- ✅ Loads all object data
- ✅ Loads all particle data
- ✅ No segmentation faults during initialization
- ⚠️ Game window functionality not tested (headless environment)

## Technical Notes

### fscanf and Locale Behavior
The fscanf function respects the LC_NUMERIC locale category for parsing floating-point numbers:
- In "C" locale: Period (.) is decimal separator
- In pt_BR locale: Comma (,) is decimal separator
- In de_DE locale: Comma (,) is decimal separator

When parsing fails, fscanf:
1. Returns the count of successfully parsed items
2. Leaves the file pointer at the failing position
3. Leaves remaining variables unmodified (contain garbage/previous values)

This caused a cascading parse failure where all subsequent reads were offset by one or more tokens.

### Why It Worked in 2001
- Windows (likely original platform) used "C" locale by default for applications
- FMOD was commercially available and widely used
- Allegro 4 fixed-point math was standard
- Windows line endings were the norm

## Credits
- Original Game: Thomas Grip (2001)
- Build Fixes: Modern port for Linux (2024)
- Key Discovery: Locale-dependent float parsing was the root cause of runtime crashes
