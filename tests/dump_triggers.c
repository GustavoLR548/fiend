#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    if (!f) {
        printf("Can't open file\n");
        return 1;
    }
    
    // Seek to trigger data
    // From our previous analysis: triggers start around file position 104664
    fseek(f, 104664, SEEK_SET);
    
    printf("=== RAW TRIGGER DATA (first 200 bytes of first trigger) ===\n");
    unsigned char buf[200];
    fread(buf, 1, 200, f);
    
    for (int i = 0; i < 200; i++) {
        if (i % 16 == 0) printf("\n%04d: ", i);
        printf("%02x ", buf[i]);
        if ((i+1) % 16 == 0) {
            printf(" | ");
            for (int j = i-15; j <= i; j++) {
                printf("%c", (buf[j] >= 32 && buf[j] < 127) ? buf[j] : '.');
            }
        }
    }
    printf("\n\n");
    
    fclose(f);
    return 0;
}
