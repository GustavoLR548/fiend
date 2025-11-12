#include <stdio.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    fseek(f, 104736, SEEK_SET);
    
    printf("At position 104736 (where we try to read triggers):\n");
    for (int i = 0; i < 200; i++) {
        if (i % 16 == 0) printf("\n%06d: ", 104736 + i);
        printf("%02x ", fgetc(f));
    }
    
    fseek(f, 104736, SEEK_SET);
    printf("\n\nAs text:\n");
    for (int i = 0; i < 200; i++) {
        int c = fgetc(f);
        if (i % 80 == 0) printf("\n");
        printf("%c", (c >= 32 && c < 127) ? c : '.');
    }
    printf("\n");
    
    fclose(f);
    return 0;
}
