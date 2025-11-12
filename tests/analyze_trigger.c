#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copy the structure definitions from trigger.h
#pragma pack(push, 1)

typedef struct {
    char used;
    char _pad1[3];  // Padding to align to 4 bytes
    int type;
    char x[40];
    int correct;
    char z[40];
    char string1[40];
    char string2[40];
    char logic;
    char _pad2[3];  // Padding at end
} CONDITION_DATA;

typedef struct {
    char used;
    char _pad1[3];  // Padding to align to 4 bytes
    int type;
    char x[40];
    int correct;
    char z[40];
    char string1[40];
    char string2[40];
    int value1;
    int value2;
    char message[200];
    char logic;
    char _pad2[3];  // Padding at end
} EVENT_DATA;

typedef struct {
    char name[30];
    char _pad_name[2];  // 2 bytes padding after name[30] to align to 4 bytes
    int active;
    int type;
    CONDITION_DATA condition[5];
    EVENT_DATA event[5];
} TRIGGER_DATA;

#pragma pack(pop)

void print_hex_dump(const unsigned char *data, int len, long offset) {
    printf("\nHex dump starting at file offset 0x%08lX:\n", offset);
    for (int i = 0; i < len; i += 16) {
        printf("%08lX: ", offset + i);
        for (int j = 0; j < 16 && i + j < len; j++) {
            printf("%02X ", data[i + j]);
        }
        printf(" | ");
        for (int j = 0; j < 16 && i + j < len; j++) {
            unsigned char c = data[i + j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
    }
}

void analyze_trigger(const char *filename, long offset) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("ERROR: Cannot open %s\n", filename);
        return;
    }

    printf("════════════════════════════════════════════════════════════════\n");
    printf("  ANALYZING TRIGGER AT 0x%08lX\n", offset);
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    printf("Structure sizes:\n");
    printf("  sizeof(TRIGGER_DATA)    = %zu bytes\n", sizeof(TRIGGER_DATA));
    printf("  sizeof(CONDITION_DATA)  = %zu bytes\n", sizeof(CONDITION_DATA));
    printf("  sizeof(EVENT_DATA)      = %zu bytes\n\n", sizeof(EVENT_DATA));

    // Read raw bytes
    unsigned char raw_bytes[3000];
    fseek(f, offset, SEEK_SET);
    size_t bytes_read = fread(raw_bytes, 1, sizeof(raw_bytes), f);
    
    printf("Read %zu bytes from file\n", bytes_read);
    print_hex_dump(raw_bytes, 256, offset);  // First 256 bytes

    // Try to parse as TRIGGER_DATA
    fseek(f, offset, SEEK_SET);
    TRIGGER_DATA trigger;
    fread(&trigger, sizeof(TRIGGER_DATA), 1, f);

    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("  PARSED TRIGGER DATA (at exact offset 0x%08lX)\n", offset);
    printf("════════════════════════════════════════════════════════════════\n\n");
    printf("Name: '%s'\n", trigger.name);
    printf("Active: %d (should be 0 or 1)\n", trigger.active);
    printf("Type: %d (should be 0, 1, or 2)\n", trigger.type);

    printf("\n--- CONDITIONS ---\n");
    for (int i = 0; i < 5; i++) {
        if (trigger.condition[i].used) {
            printf("\nCondition %d:\n", i);
            printf("  used: %d\n", trigger.condition[i].used);
            printf("  type: %d\n", trigger.condition[i].type);
            printf("  x: '%s'\n", trigger.condition[i].x);
            printf("  correct: %d\n", trigger.condition[i].correct);
            printf("  z: '%s'\n", trigger.condition[i].z);
            printf("  string1: '%s'\n", trigger.condition[i].string1);
            printf("  string2: '%s'\n", trigger.condition[i].string2);
            printf("  logic: %d\n", trigger.condition[i].logic);
        }
    }

    printf("\n--- EVENTS ---\n");
    for (int i = 0; i < 5; i++) {
        if (trigger.event[i].used) {
            printf("\nEvent %d:\n", i);
            printf("  used: %d\n", trigger.event[i].used);
            printf("  type: %d\n", trigger.event[i].type);
            printf("  x: '%s'\n", trigger.event[i].x);
            printf("  correct: %d\n", trigger.event[i].correct);
            printf("  z: '%s'\n", trigger.event[i].z);
            printf("  string1: '%s'\n", trigger.event[i].string1);
            printf("  string2: '%s'\n", trigger.event[i].string2);
            printf("  value1: %d\n", trigger.event[i].value1);
            printf("  value2: %d\n", trigger.event[i].value2);
            printf("  message: '%s'\n", trigger.event[i].message);
            printf("  logic: %d\n", trigger.event[i].logic);
        }
    }

    // Now let's search for the expected strings
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("  SEARCHING FOR EXPECTED STRINGS IN NEARBY DATA\n");
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    // Search for "forest" in the raw bytes
    for (size_t i = 0; i < bytes_read - 10; i++) {
        if (memcmp(&raw_bytes[i], "forest", 6) == 0) {
            printf("Found 'forest' at offset +%zu (file 0x%08lX)\n", i, offset + i);
            printf("  Context: ");
            for (int j = -20; j < 60 && (i+j) < bytes_read && (i+j) >= 0; j++) {
                unsigned char c = raw_bytes[i+j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            printf("\n\n");
        }
    }

    // Try reading with different offsets to find valid-looking data
    printf("\n════════════════════════════════════════════════════════════════\n");
    printf("  TRYING DIFFERENT OFFSETS (looking for valid trigger data)\n");
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    for (int offset_adjust = -64; offset_adjust <= 64; offset_adjust += 4) {
        fseek(f, offset + offset_adjust, SEEK_SET);
        TRIGGER_DATA test_trigger;
        if (fread(&test_trigger, sizeof(TRIGGER_DATA), 1, f) != 1) continue;
        
        // Check if this looks like valid data
        if (test_trigger.active >= 0 && test_trigger.active <= 1 &&
            test_trigger.type >= 0 && test_trigger.type <= 2 &&
            strlen(test_trigger.name) < 30) {
            
            printf("Offset %+4d (0x%08lX): name='%s' active=%d type=%d\n", 
                   offset_adjust, offset + offset_adjust, test_trigger.name, test_trigger.active, test_trigger.type);
            
            // Check for the expected message in events
            for (int i = 0; i < 5; i++) {
                if (test_trigger.event[i].used && 
                    (strstr(test_trigger.event[i].message, "forest") != NULL ||
                     strstr(test_trigger.event[i].message, "wander") != NULL)) {
                    printf("  *** FOUND EXPECTED MESSAGE at offset %+d, event %d:\n", offset_adjust, i);
                    printf("      '%s'\n", test_trigger.event[i].message);
                    printf("      Event type: %d\n", test_trigger.event[i].type);
                    printf("      string1: '%s'\n", test_trigger.event[i].string1);
                }
            }
        }
    }

    fclose(f);
}

int main() {
    printf("\n");
    printf("████████████████████████████████████████████████████████████████\n");
    printf("  FIEND TRIGGER STRUCTURE ANALYZER\n");
    printf("████████████████████████████████████████████████████████████████\n\n");
    
    // The trigger you found at address 0x0001896C
    analyze_trigger("release/maps/lauder_alive/lauder.map", 0x0001896C);
    
    printf("\n\n");
    return 0;
}
