#include <stdio.h>

// Current structure with padding
typedef struct
{
char used;
char _padding1[3];
int type;
char x[40];
int correct;
char z[40];
char string1[40];
char string2[40];
char logic;
char _padding2[3];
}CONDITION_DATA;

int main() {
    CONDITION_DATA c;
    printf("sizeof(CONDITION_DATA) = %zu\n", sizeof(CONDITION_DATA));
    printf("Offsets:\n");
    printf("  used: %zu\n", (size_t)&c.used - (size_t)&c);
    printf("  type: %zu\n", (size_t)&c.type - (size_t)&c);
    printf("  x: %zu\n", (size_t)&c.x - (size_t)&c);
    printf("  correct: %zu\n", (size_t)&c.correct - (size_t)&c);
    printf("  z: %zu\n", (size_t)&c.z - (size_t)&c);
    printf("  string1: %zu\n", (size_t)&c.string1 - (size_t)&c);
    printf("  string2: %zu\n", (size_t)&c.string2 - (size_t)&c);
    printf("  logic: %zu\n", (size_t)&c.logic - (size_t)&c);
    return 0;
}
