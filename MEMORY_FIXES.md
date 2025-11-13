# Memory Safety Fixes

This document details all memory-related bugs fixed to make the game stable in Release builds with `-O2` optimization and `FORTIFY_SOURCE`.

## Summary

The game was crashing during map transitions in Release builds due to multiple memory corruption issues exposed by compiler optimizations. Using AddressSanitizer (ASAN), we identified and fixed:

- **3 buffer overflow vulnerabilities**
- **1 heap buffer overflow**
- **2 memory leaks**
- **1 double-free error**

All issues have been resolved. The game now runs stably with AddressSanitizer reporting no errors.

---

## 1. Buffer Overflow in `trigger.c` - Save Functions

### Issue
Multiple `sprintf()` calls writing map names into fixed-size buffers without bounds checking.

### Files Affected
- `src/trigger.c`

### Locations Fixed
- Line 153: `save_global_triggers()` - writing to `temp.map_name`
- Line 199: `save_triggers()` - writing to `temp.map_name`
- Lines 213-214: `save_objects()` - writing to `saved_object[].name`
- Lines 236-237: `save_objects()` - writing to `saved_object[].name`

### Fix Applied
```c
// BEFORE (unsafe):
sprintf(temp.map_name, "%s", map->name);

// AFTER (safe):
strncpy(temp.map_name, map->name, sizeof(temp.map_name) - 1);
temp.map_name[sizeof(temp.map_name) - 1] = '\0';
```

### Root Cause
`sprintf()` doesn't check buffer boundaries. With `-O2` optimization and `FORTIFY_SOURCE`, the compiler detected potential overflows and aborted.

---

## 2. Map Name Null-Termination in `mapio.c`

### Issue
Map names loaded via `fread()` were not guaranteed to be null-terminated, causing undefined behavior in string operations.

### Files Affected
- `src/mapio.c`

### Locations Fixed
- Line 74: `load_map()` - after reading `map->name`
- Line 227: `load_edit_map()` - after reading `map->name`
- Line 290: `load_edit_map()` - after reading `map->name`

### Fix Applied
```c
fread(map->name, sizeof(map->name), 1, f);
map->name[sizeof(map->name) - 1] = '\0';  // Ensure null-termination
```

### Additional Validation
Added comprehensive map dimension validation to prevent allocation overflows:

```c
// Validate map dimensions before allocation (lines 298-319)
if (map->w <= 0 || map->w > 1000 || map->h <= 0 || map->h > 1000) {
    sprintf(fiend_errorcode, "Invalid map dimensions: %dx%d", map->w, map->h);
    fclose(f);
    return 0;
}
```

### Root Cause
Binary data read with `fread()` is not automatically null-terminated. String operations on unterminated buffers cause buffer overruns.

---

## 3. Off-by-One Error in `los.c` - Global Buffer Overflow

### Issue
**CRITICAL**: Off-by-one array access in line-of-sight calculations. Array `light_source[18]` was being accessed with indices 0-18 (19 elements), causing heap corruption.

### Files Affected
- `src/fiend/los.c`

### Locations Fixed
- Line 296: Changed `if(l_i < 18)` → `if(l_i < 17)`
- Line 308: Changed `if(l_j < 18)` → `if(l_j < 17)`

### Fix Applied
```c
// BEFORE (incorrect - allows index 18):
if(l_i < 18) {
    light_source[l_i].x = ...;
}

// AFTER (correct - max index 17):
if(l_i < 17) {
    light_source[l_i].x = ...;
}
```

### ASAN Output
```
==274564==ERROR: AddressSanitizer: global-buffer-overflow on address 0x55acbd8dd920
WRITE of size 4 at 0x55acbd8dd920 thread T0
    #0 0x55acbd6cf1d4 in calc_los src/fiend/los.c:298
```

### Root Cause
This was the **primary cause of heap corruption** during map transitions. The off-by-one error caused writes beyond the array boundary, corrupting adjacent memory. This manifested as crashes during map loading when the corrupted memory was later accessed.

---

## 4. Heap Buffer Overflow in `trigger.c` - Object Iteration

### Issue
Loops iterating over `MAX_OBJECT_NUM` (hardcoded 1000) instead of actual `map->num_of_objects`, accessing uninitialized memory.

### Files Affected
- `src/trigger.c`
- `src/mapio.c`

### Locations Fixed
**trigger.c:**
- Line 204: `save_objects()` - loop condition
- Line 227: `save_objects()` - loop condition  
- Line 293: `load_objects()` - loop condition

**mapio.c:**
- Line 326: Changed allocation from `MAX_OBJECT_NUM` to `map->num_of_objects`

### Fix Applied
```c
// BEFORE (heap buffer overflow):
for(i = 0; i < MAX_OBJECT_NUM; i++) {
    // Accesses beyond allocated memory!
}

// AFTER (correct bounds):
for(i = 0; i < map->num_of_objects; i++) {
    // Only accesses valid elements
}
```

### ASAN Output
```
==274564==ERROR: AddressSanitizer: heap-buffer-overflow
READ of size 4 at 0x6190000049c8 thread T0
    #0 0x55acbd6d4891 in save_objects src/trigger.c:204
```

### Root Cause
Maps don't always have 1000 objects. Iterating beyond `num_of_objects` accesses uninitialized heap memory, causing undefined behavior.

---

## 5. Memory Leaks in `object.c` - RLE Sprite Cleanup

### Issue
RLE sprites created during object loading were never freed, leaking memory on every map transition.

### Files Affected
- `src/object.c`

### Locations Fixed
- Lines 365-402: `release_objects()` function - added RLE sprite cleanup

### Fix Applied
```c
void release_objects(void)
{
    for(i = 0; i < num_of_objects; i++)
        for(j = 0; j < object_info[i].num_of_frames; j++)
        {
            if(!object_info[i].angles || object_info[i].door) {
                // Single bitmap - free it
                if(object_info[i].pic[j][0].data)
                    destroy_bitmap(object_info[i].pic[j][0].data);
            }
            else if(object_info[i].additive) {
                // Additive objects - free angle bitmaps
                for(k = 0; k < num_angles; k++) {
                    if(object_info[i].pic[j][k].data)
                        destroy_bitmap(object_info[i].pic[j][k].data);
                }
            }
            else {
                // Normal objects - free RLE sprites (MEMORY LEAK FIX)
                for(k = 0; k < num_angles; k++) {
                    if(object_info[i].rle_pic[j][k])
                        destroy_rle_sprite(object_info[i].rle_pic[j][k]);
                }
            }
        }
}
```

### ASAN Output
```
Direct leak of 344064 byte(s) in 1 object(s) allocated from:
    #0 0x7ff4267ebb37 in malloc
    #1 0x55acbd6ae44d in load_objects src/object.c:338
```

### Root Cause
During loading, normal objects convert bitmaps to RLE sprites for faster rendering. The original bitmaps are freed immediately, but the RLE sprites were never cleaned up.

---

## 6. Double-Free in `object.c` - Cleanup Logic Mismatch

### Issue
Objects with `trans=true` and `additive=false` were freed during loading (normal objects branch) but also during cleanup (additive/trans branch), causing double-free errors.

### Files Affected
- `src/object.c`

### Locations Fixed
- Line 382: Changed cleanup condition from `else if(additive || trans)` to `else if(additive)`

### Fix Applied
```c
// BEFORE (double-free):
// Loading logic:
else if(object_info[i].additive) {
    // Bitmaps NOT freed
}
else {
    // Normal objects (includes trans without additive)
    // Bitmaps FREED here
}

// Cleanup logic (MISMATCH):
else if(object_info[i].additive || object_info[i].trans) {
    // Tries to free bitmaps AGAIN!
}

// AFTER (correct):
// Cleanup now matches loading:
else if(object_info[i].additive) {
    // Only additive objects, matches loading
}
```

### ASAN Output
```
==282156==ERROR: AddressSanitizer: attempting double-free on 0x514000014040
    #0 0x7f8cc61eef3f in __interceptor_free
    #1 0x55c93e0c6db9 in release_objects src/object.c:388
```

### Root Cause
The loading code had three branches: `!angles || door`, `additive`, and `else` (normal). The cleanup code incorrectly used `additive || trans` instead of just `additive`, causing transparent objects to match the wrong branch.

---

## 7. Tile Array Bounds Checking

### Issue
Multiple files accessed `tile_info[]` array without validating indices, risking out-of-bounds access.

### Files Affected
- `src/tile.c`
- `src/lightmap.c`
- `src/fiend/collision.c`
- `src/fiend/player.c`
- `src/fiend/enemy_update.c`
- `src/fiend/npc_update.c`

### Fix Applied
Added bounds checking before all `tile_info[]` accesses:

```c
// Example from tile.c:
if (tile >= 0 && tile < num_of_tiles) {
    return tile_info[tile].sprite;
}
```

### Root Cause
Preventive hardening. While not actively causing crashes, these could trigger corruption if invalid tile indices are used.

---

## 8. Map File Read Verification in `mapio.c`

### Issue
`fread()` calls did not verify that all bytes were successfully read, potentially using partially-loaded data.

### Files Affected
- `src/mapio.c`

### Locations Fixed
- Lines 333-342: Added verification for object data reads

### Fix Applied
```c
size_t bytes_read = fread(&map->object[i], 1, sizeof(SAVED_OBJECT), f);
if (bytes_read != sizeof(SAVED_OBJECT)) {
    sprintf(fiend_errorcode, "Corrupt map file: incomplete object data");
    fclose(f);
    return 0;
}
```

### Root Cause
I/O operations can fail or return partial data. Using unverified data leads to undefined behavior.

---

## Testing Methodology

### Tools Used
1. **AddressSanitizer (ASAN)**: Memory error detector
   - Detects: buffer overflows, use-after-free, double-free, memory leaks
   - Enabled with: `-fsanitize=address`

2. **GCC with FORTIFY_SOURCE**: Compile-time buffer overflow detection
   - Enabled with: `-D_FORTIFY_SOURCE=2` (automatic in `-O2`)

### Build Commands

**Debug build with ASAN:**
```bash
make asan
```

**Run with ASAN:**
```bash
make asan-run
```

### Test Results
- ✅ No buffer overflows detected
- ✅ No heap corruption detected
- ✅ No memory leaks detected
- ✅ No double-free errors detected
- ✅ Game runs stably through multiple map transitions
- ✅ No ASAN errors reported

---

## Impact

### Before Fixes
- **Status**: Release builds crashed during map transitions
- **Symptoms**: Heap corruption, segmentation faults, undefined behavior
- **Reliability**: Game unplayable in optimized builds

### After Fixes
- **Status**: Release builds stable
- **Symptoms**: None - clean ASAN reports
- **Reliability**: Game fully playable with no memory errors

---

## Verification

To verify these fixes, build and run with AddressSanitizer:

```bash
# Build with memory debugging
make asan

# Run the game
make asan-run

# Check the log for any ASAN errors
cat release/asan-test.log
```

A successful run will show no ASAN errors.

---

## Notes for Future Development

1. **Always use `strncpy()` instead of `sprintf()` for fixed-size buffers**
   - Remember to null-terminate: `buffer[size-1] = '\0'`

2. **Null-terminate all `fread()` string data**
   - Binary reads don't include null terminators

3. **Validate array indices before access**
   - Check `index >= 0 && index < array_size`

4. **Use actual counts, not hardcoded maximums**
   - Loop to `num_of_items`, not `MAX_ITEMS`

5. **Match allocation and deallocation logic exactly**
   - If loading has branches A/B/C, cleanup must have identical branches

6. **Test with AddressSanitizer regularly**
   - Run `make asan-run` after significant changes
   - Don't wait for Release builds to discover memory issues

7. **Verify `fread()` return values**
   - Check that expected bytes were read
   - Handle partial reads gracefully

---

## References

- AddressSanitizer: https://github.com/google/sanitizers/wiki/AddressSanitizer
- FORTIFY_SOURCE: https://www.gnu.org/software/libc/manual/html_node/Source-Fortification.html
- Secure Coding: https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard
