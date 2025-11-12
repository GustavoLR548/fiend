#include <stdio.h>
#include <string.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char *buf = malloc(size);
    fread(buf, 1, size, f);
    fclose(f);
    
    // Search for "Set var corben"
    char *search = "Set var corben";
    for (long i = 0; i < size - strlen(search); i++) {
        if (memcmp(&buf[i], search, strlen(search)) == 0) {
            printf("Found 'Set var corben' at position: %ld\n", i);
            
            // Show 48 bytes before (might be previous trigger name)
            printf("\n48 bytes before:\n");
            for (int j = -48; j < 0 && (i+j) >= 0; j++) {
                if (j % 16 == -48 || j % 16 == 0) printf("\n%05ld: ", i+j);
                printf("%02x ", buf[i+j]);
            }
            
            printf("\n\nAt 'Set var corben' (pos %ld):\n", i);
            for (int j = 0; j < 80; j++) {
                if (j % 16 == 0) printf("\n%05ld: ", i+j);
                printf("%02x ", buf[i+j]);
            }
            printf("\n\nAs ASCII: ");
            for (int j = 0; j < 50; j++) {
                printf("%c", (buf[i+j] >= 32 && buf[i+j] < 127) ? buf[i+j] : '.');
            }
            printf("\n");
        }
    }
    
    free(buf);
    return 0;
}
