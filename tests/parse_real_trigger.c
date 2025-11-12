#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    if (!f) {
        printf("Cannot open file\n");
        return 1;
    }

    // "The Arrival" trigger starts at 0x1FF54
    long trigger_addr = 0x1FF54;
    
    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("  PARSING TRIGGER AT 0x%08lX (The Arrival)\n", trigger_addr);
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    fseek(f, trigger_addr, SEEK_SET);
    
    // Read trigger name (30 bytes)
    char name[31] = {0};
    fread(name, 1, 30, f);
    printf("Trigger name (30 bytes): '%s'\n", name);
    printf("  At: 0x%08lX\n\n", trigger_addr);
    
    // Read next bytes to see the pattern
    printf("Next 1000 bytes after name:\n");
    unsigned char data[1000];
    fread(data, 1, 1000, f);
    
    for (int i = 0; i < 1000; i += 16) {
        printf("%08lX: ", trigger_addr + 30 + i);
        for (int j = 0; j < 16 && i+j < 1000; j++) {
            printf("%02X ", data[i+j]);
        }
        printf(" | ");
        for (int j = 0; j < 16 && i+j < 1000; j++) {
            unsigned char c = data[i+j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
    
    printf("\n════════════════════════════════════════════════════════════════\n\n");
    
    fclose(f);
    return 0;
}
