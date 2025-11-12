#include <stdio.h>
#include "src/map.h"

int main() {
    printf("sizeof(AREA_DATA) = %zu (expected: 52)\n", sizeof(AREA_DATA));
    printf("  char name[40] = 40\n");
    printf("  int active = 4\n");
    printf("  int x = 4\n");
    printf("  int y = 4\n");
    printf("  int w = 4\n");
    printf("  int h = 4\n");
    printf("  Total = 60 bytes\n");
    return 0;
}
