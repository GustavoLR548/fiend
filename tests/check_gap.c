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
    printf("  CHECKING THE MYSTERIOUS 16-BYTE GAP\n");
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    // The trigger address from user's investigation
    long trigger_addr = 0x0001896C;
    
    printf("Trigger found at: 0x%08lX (%ld bytes from start)\n", trigger_addr, trigger_addr);
    
    // If we skip 16 bytes back, what's there?
    long gap_start = trigger_addr - 16;
    
    printf("16 bytes before trigger (the gap): 0x%08lX\n\n", gap_start);
    
    // Read those 16 bytes
    fseek(f, gap_start, SEEK_SET);
    unsigned char gap_bytes[16];
    fread(gap_bytes, 1, 16, f);
    
    printf("Gap bytes (hex): ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", gap_bytes[i]);
    }
    printf("\n");
    
    printf("Gap bytes (dec): ");
    for (int i = 0; i < 16; i++) {
        printf("%3d ", gap_bytes[i]);
    }
    printf("\n");
    
    printf("Gap bytes (ASCII): ");
    for (int i = 0; i < 16; i++) {
        printf("%c", (gap_bytes[i] >= 32 && gap_bytes[i] <= 126) ? gap_bytes[i] : '.');
    }
    printf("\n\n");
    
    // Check if it's all zeros
    int all_zeros = 1;
    int zero_count = 0;
    for (int i = 0; i < 16; i++) {
        if (gap_bytes[i] == 0) {
            zero_count++;
        } else {
            all_zeros = 0;
        }
    }
    
    printf("Zero bytes: %d/16\n\n", zero_count);
    
    if (all_zeros) {
        printf("✓ The 16-byte gap contains ALL ZEROS - likely alignment padding!\n");
    } else if (zero_count > 8) {
        printf("⚠ The 16-byte gap is mostly zeros (%d/16) - probably padding with some remnants\n", zero_count);
    } else {
        printf("✗ The 16-byte gap contains SIGNIFICANT DATA - we might be missing a structure!\n\n");
        
        // Try to interpret as ints
        int *int_values = (int*)gap_bytes;
        printf("Interpreted as 4 integers:\n");
        printf("  int[0] = %d (0x%08X)\n", int_values[0], int_values[0]);
        printf("  int[1] = %d (0x%08X)\n", int_values[1], int_values[1]);
        printf("  int[2] = %d (0x%08X)\n", int_values[2], int_values[2]);
        printf("  int[3] = %d (0x%08X)\n", int_values[3], int_values[3]);
    }
    
    // Also check what comes immediately after the trigger
    printf("\n--- Checking 32 bytes BEFORE the gap ---\n");
    fseek(f, gap_start - 32, SEEK_SET);
    unsigned char before_gap[32];
    fread(before_gap, 1, 32, f);
    
    printf("Hex: ");
    for (int i = 0; i < 32; i++) {
        printf("%02X ", before_gap[i]);
        if ((i+1) % 16 == 0) printf("\n     ");
    }
    printf("\n");
    
    printf("ASCII: ");
    for (int i = 0; i < 32; i++) {
        printf("%c", (before_gap[i] >= 32 && before_gap[i] <= 126) ? before_gap[i] : '.');
    }
    printf("\n\n");
    
    printf("════════════════════════════════════════════════════════════════\n\n");
    
    fclose(f);
    return 0;
}
