#include <stdio.h>
#include "map.h"
#include "trigger.h"

int main() {
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("  STRUCTURE SIZES ON THIS SYSTEM (64-bit Linux)\n");
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    printf("sizeof(CONDITION_DATA) = %zu\n", sizeof(CONDITION_DATA));
    printf("sizeof(EVENT_DATA) = %zu\n", sizeof(EVENT_DATA));
    printf("TRIGGER_DATA) = %zu\n\n", sizeof(TRIGGER_DATA));
    
    printf("sizeof(TILE_DATA) = %zu\n", sizeof(TILE_DATA));
    printf("sizeof(OBJECT_DATA) = %zu\n", sizeof(OBJECT_DATA));
    printf("sizeof(AREA_DATA) = %zu\n", sizeof(AREA_DATA));
    printf("sizeof(LOOK_AT_AREA_DATA) = %zu\n", sizeof(LOOK_AT_AREA_DATA));
    printf("sizeof(LINK_DATA) = %zu\n", sizeof(LINK_DATA));
    printf("sizeof(SOUNDEMITOR_DATA) = %zu\n", sizeof(SOUNDEMITOR_DATA));
    printf("sizeof(LIGHT_DATA) = %zu\n", sizeof(LIGHT_DATA));
    
    printf("\nFor lauder.map (64x50, 39 objects, 1 area, 8 look_at_areas, 10 links, 0 soundemitors, 11 triggers):\n\n");
    
    // File layout calculation
    long offset = 0;
    
    // MAP_DATA header written with fwrite() on 32-bit system
    // Pointers are 4 bytes each on 32-bit
    long map_header_32bit = 40 + 7*4 + 10*4 + 20*44 + 4 + 64*16;  // name + ints + pointers + vars + path_nodes
    printf("MAP_DATA header (32-bit): ~%ld bytes\n", map_header_32bit);
    offset += map_header_32bit;
    
    // LIGHT_DATA array[0]
    offset += 0 * sizeof(LIGHT_DATA);
    
    // TILE_DATA arrays  
    offset += 3 * 64 * 50 * sizeof(TILE_DATA);
    printf("After 3 tile layers: 0x%lX (%ld)\n", offset, offset);
    
    // shadow array
    offset += 64 * 50;
    printf("After shadow: 0x%lX (%ld)\n", offset, offset);
    
    // OBJECT_DATA array[39]
    offset += 39 * sizeof(OBJECT_DATA);
    printf("After objects: 0x%lX (%ld)\n", offset, offset);
    
    // AREA_DATA array[1]
    offset += 1 * sizeof(AREA_DATA);
    printf("After areas: 0x%lX (%ld)\n", offset, offset);
    
    // LOOK_AT_AREA_DATA array[8]
    offset += 8 * sizeof(LOOK_AT_AREA_DATA);
    printf("After look_at_areas: 0x%lX (%ld)\n", offset, offset);
    
    // LINK_DATA array[10]
    offset += 10 * sizeof(LINK_DATA);
    printf("After links: 0x%lX (%ld)\n", offset, offset);
    
    // SOUNDEMITOR_DATA array[0]
    offset += 0 * sizeof(SOUNDEMITOR_DATA);
    printf("After soundemitors: 0x%lX (%ld)\n", offset, offset);
    
    printf("\n** TRIGGERS SHOULD START AT: 0x%lX (%ld) **\n\n", offset, offset);
    printf("'The Arrival' trigger name found at: 0x1FF54 (130900)\n");
    printf("Difference: %ld bytes = 0x%lX\n", 0x1FF54 - offset, 0x1FF54 - offset);
    
    printf("\n════════════════════════════════════════════════════════════════\n\n");
    
    return 0;
}
