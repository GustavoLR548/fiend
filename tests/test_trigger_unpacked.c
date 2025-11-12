#include <stdio.h>

#define MAX_CONDITION_NUM 5
#define MAX_EVENT_NUM 5

// Without pack(1) - natural alignment
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
}CONDITION_DATA_UNPACKED;

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
}EVENT_DATA_UNPACKED;

typedef struct 
{
	char name[30];
	int active;
	int type;
	CONDITION_DATA_UNPACKED condition[MAX_CONDITION_NUM];
	EVENT_DATA_UNPACKED event[MAX_EVENT_NUM];
}TRIGGER_DATA_UNPACKED;

// With pack(1)
#pragma pack(1)
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
}CONDITION_DATA_PACKED;

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
}EVENT_DATA_PACKED;

typedef struct 
{
	char name[30];
	int active;
	int type;
	CONDITION_DATA_PACKED condition[MAX_CONDITION_NUM];
	EVENT_DATA_PACKED event[MAX_EVENT_NUM];
}TRIGGER_DATA_PACKED;
#pragma pack()

int main() {
    printf("UNPACKED (natural 32-bit alignment):\n");
    printf("  CONDITION_DATA: %zu\n", sizeof(CONDITION_DATA_UNPACKED));
    printf("  EVENT_DATA: %zu\n", sizeof(EVENT_DATA_UNPACKED));
    printf("  TRIGGER_DATA: %zu\n", sizeof(TRIGGER_DATA_UNPACKED));
    
    printf("\nPACKED (#pragma pack(1)):\n");
    printf("  CONDITION_DATA: %zu\n", sizeof(CONDITION_DATA_PACKED));
    printf("  EVENT_DATA: %zu\n", sizeof(EVENT_DATA_PACKED));
    printf("  TRIGGER_DATA: %zu\n", sizeof(TRIGGER_DATA_PACKED));
    
    printf("\nField offsets in TRIGGER_DATA_UNPACKED:\n");
    TRIGGER_DATA_UNPACKED t;
    printf("  name: %zu\n", (size_t)&t.name - (size_t)&t);
    printf("  active: %zu\n", (size_t)&t.active - (size_t)&t);
    printf("  type: %zu\n", (size_t)&t.type - (size_t)&t);
    
    printf("\nField offsets in TRIGGER_DATA_PACKED:\n");
    TRIGGER_DATA_PACKED tp;
    printf("  name: %zu\n", (size_t)&tp.name - (size_t)&tp);
    printf("  active: %zu\n", (size_t)&tp.active - (size_t)&tp);
    printf("  type: %zu\n", (size_t)&tp.type - (size_t)&tp);
    
    return 0;
}
