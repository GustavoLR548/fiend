#include <stdio.h>
#include <string.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    if (!f) {
        printf("Can't open file\n");
        return 1;
    }
    
    // Search for the string "Nothing" which should be the first trigger name
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char buf[100000];
    fread(buf, 1, 100000, f);
    
    // Search for "Nothing"
    for (long i = 0; i < 100000 - 7; i++) {
        if (memcmp(&buf[i], "Nothing", 7) == 0) {
            printf("Found 'Nothing' at file position: %ld\n", i);
            
            // Show 80 bytes before it
            printf("\n80 bytes before:\n");
            for (int j = -80; j < 0; j += 16) {
                printf("%04ld: ", i+j);
                for (int k = 0; k < 16 && (i+j+k) >= 0; k++) {
                    printf("%02x ", buf[i+j+k]);
                }
                printf("\n");
            }
            
            printf("\nAt 'Nothing':\n%04ld: ", i);
            for (int j = 0; j < 40; j++) {
                printf("%02x ", buf[i+j]);
                if ((j+1) % 16 == 0 && j < 39) printf("\n%04ld: ", i+j+1);
            }
            printf("\n");
            break;
        }
    }
    
    fclose(f);
    return 0;
}
