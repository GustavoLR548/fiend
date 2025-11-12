#include <stdio.h>

int main() {
    FILE *f = fopen("release/maps/lauder_alive/lauder.map", "rb");
    fseek(f, 91096, SEEK_SET);
    
    printf("From position 91096 to 91200:\n");
    for (int i = 0; i < 104; i++) {
        if (i % 16 == 0) printf("\n%05d: ", 91096 + i);
        printf("%02x ", fgetc(f));
    }
    printf("\n\n");
    
    fseek(f, 91096, SEEK_SET);
    printf("As text:\n");
    for (int i = 0; i < 104; i++) {
        int c = fgetc(f);
        printf("%c", (c >= 32 && c < 127) ? c : '.');
    }
    printf("\n");
    
    fclose(f);
    return 0;
}
