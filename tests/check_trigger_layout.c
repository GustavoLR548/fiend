#include <stdio.h>
#include <stddef.h>

typedef struct
{
    char used;
    int type;
    char x[40];
    int correct;
    char z[40];
    char string1[40];
    char string2[40];
    char logic;
} CONDITION_DATA;

typedef struct
{
    char used;
    int type;
    char x[40];
    char y[40];
    char z[40];
    char string1[40];
    char string2[40];
    char text[160];
    char logic;
} EVENT_DATA;

typedef struct 
{
    char name[30];
    int active;
    int type;
    CONDITION_DATA condition[5];
    EVENT_DATA event[5];
} TRIGGER_DATA;

int main() {
    printf("TRIGGER_DATA layout:\n");
    printf("  sizeof(TRIGGER_DATA) = %zu\n", sizeof(TRIGGER_DATA));
    printf("  offsetof(name) = %zu\n", offsetof(TRIGGER_DATA, name));
    printf("  offsetof(active) = %zu\n", offsetof(TRIGGER_DATA, active));
    printf("  offsetof(type) = %zu\n", offsetof(TRIGGER_DATA, type));
    printf("  offsetof(condition) = %zu\n", offsetof(TRIGGER_DATA, condition));
    printf("  offsetof(event) = %zu\n", offsetof(TRIGGER_DATA, event));
    
    printf("\nCONDITION_DATA size: %zu\n", sizeof(CONDITION_DATA));
    printf("EVENT_DATA size: %zu\n", sizeof(EVENT_DATA));
    
    return 0;
}
