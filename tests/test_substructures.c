#include <stdio.h>

// 32-bit unpacked
typedef struct
{
	char used;      // 1 byte
	int type;       // 4 bytes (aligned to offset 4 = 3 bytes padding after used)
	char x[40];     // 40 bytes
	int correct;    // 4 bytes (aligned to offset 48)
	char z[40];     // 40 bytes
	char string1[40]; // 40 bytes
	char string2[40]; // 40 bytes
	char logic;     // 1 byte
}CONDITION_DATA_UNPACKED;

typedef struct
{
	char used;      // 1 byte
	int type;       // 4 bytes (aligned to offset 4 = 3 bytes padding after used)
	char x[40];     // 40 bytes
	char y[40];     // 40 bytes
	char z[40];     // 40 bytes
	char string1[40]; // 40 bytes
	char string2[40]; // 40 bytes
	char text[160]; // 160 bytes
	char logic;     // 1 byte
}EVENT_DATA_UNPACKED;

int main() {
    CONDITION_DATA_UNPACKED c;
    EVENT_DATA_UNPACKED e;
    
    printf("CONDITION_DATA_UNPACKED: %zu bytes\n", sizeof(CONDITION_DATA_UNPACKED));
    printf("  used offset: %zu\n", (size_t)&c.used - (size_t)&c);
    printf("  type offset: %zu\n", (size_t)&c.type - (size_t)&c);
    printf("  x offset: %zu\n", (size_t)&c.x - (size_t)&c);
    printf("  correct offset: %zu\n", (size_t)&c.correct - (size_t)&c);
    printf("  logic offset: %zu\n", (size_t)&c.logic - (size_t)&c);
    
    printf("\nEVENT_DATA_UNPACKED: %zu bytes\n", sizeof(EVENT_DATA_UNPACKED));
    printf("  used offset: %zu\n", (size_t)&e.used - (size_t)&e);
    printf("  type offset: %zu\n", (size_t)&e.type - (size_t)&e);
    printf("  x offset: %zu\n", (size_t)&e.x - (size_t)&e);
    printf("  logic offset: %zu\n", (size_t)&e.logic - (size_t)&e);
    
    return 0;
}
