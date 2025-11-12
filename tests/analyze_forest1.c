#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    if (!f) {
        printf("Cannot open file\n");
        return 1;
    }

    printf("\n");
    printf("════════════════════════════════════════════════════════════════\n");
    printf("  ANALYZING TRIGGER STARTING AT 'forest1'\n");
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    long trigger_start = 0x00018930;
    fseek(f, trigger_start, SEEK_SET);
    
    // Read trigger name
    char name[32] = {0};
    fread(name, 1, 30, f);
    printf("Trigger name (30 bytes): '%s'\n", name);
    printf("  Position: 0x%08lX\n\n", trigger_start);
    
    // Skip padding (2 bytes)
    unsigned char pad[2];
    fread(pad, 1, 2, f);
    printf("Padding (2 bytes): %02X %02X\n", pad[0], pad[1]);
    printf("  Position: 0x%08lX\n\n", trigger_start + 30);
    
    // Read active
    unsigned char active;
    fread(&active, 1, 1, f);
    printf("Active (1 byte): %d\n", active);
    printf("  Position: 0x%08lX\n\n", trigger_start + 32);
    
    // Read type
    unsigned char type;
    fread(&type, 1, 1, f);
    printf("Type (1 byte): %d\n", type);
    printf("  Position: 0x%08lX\n\n", trigger_start + 33);
    
    // Now we should be at conditions
    // But let's see what's actually here
    long current_pos = ftell(f);
    printf("Current file position: 0x%08lX\n", current_pos);
    printf("Expected (if conditions follow): 0x%08lX\n\n", trigger_start + 34);
    
    printf("Next 200 bytes of data:\n");
    unsigned char data[200];
    fread(data, 1, 200, f);
    
    for (int i = 0; i < 200; i += 16) {
        printf("%08lX: ", current_pos + i);
        for (int j = 0; j < 16 && i+j < 200; j++) {
            printf("%02X ", data[i+j]);
        }
        printf(" | ");
        for (int j = 0; j < 16 && i+j < 200; j++) {
            unsigned char c = data[i+j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
    
    printf("\n════════════════════════════════════════════════════════════════\n\n");
    
    fclose(f);
    return 0;
}
