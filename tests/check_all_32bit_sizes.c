#include <stdio.h>
#include <stddef.h>

// Copy all the structure definitions to check their 32-bit sizes

typedef struct
{
    int value;
    char name[40];
} VARIABLE_DATA;

typedef struct
{
    int tile_set;
    int tile_num;
} TILE_DATA;

typedef struct
{
    char name[40];
    int active;
    int x;
    int y;
    int w;
    int h;
} AREA_DATA;

typedef struct
{
    char name[30];
    int active;
    int x;
    int y;
    int w;
    int h;
    char message[120];
} LOOK_AT_AREA_DATA;

#define LIMK_MESSAGE_LENGTH 240

typedef struct
{
    int type;
    char message[LIMK_MESSAGE_LENGTH];
    char map_file[80];
    char link_name[20];
} LINK_EVENT;

typedef struct
{
    char name[20];
    int x;
    int y;
    int w;
    int h;
    int event_chosen;
    int type;
    LINK_EVENT event[5];
} LINK_DATA;

typedef struct
{
    int active;
    int x;
    int y;
    int loop;
    char sound_name[40];
    int emitor_type;
    int sound_type;
    int voice_num;
} SOUNDEMITOR_DATA;

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
    printf("32-bit structure sizes:\n");
    printf("TILE_DATA: %zu\n", sizeof(TILE_DATA));
    printf("AREA_DATA: %zu\n", sizeof(AREA_DATA));
    printf("LOOK_AT_AREA_DATA: %zu\n", sizeof(LOOK_AT_AREA_DATA));
    printf("LINK_EVENT: %zu\n", sizeof(LINK_EVENT));
    printf("LINK_DATA: %zu\n", sizeof(LINK_DATA));
    printf("SOUNDEMITOR_DATA: %zu\n", sizeof(SOUNDEMITOR_DATA));
    printf("CONDITION_DATA: %zu\n", sizeof(CONDITION_DATA));
    printf("EVENT_DATA: %zu\n", sizeof(EVENT_DATA));
    printf("TRIGGER_DATA: %zu\n", sizeof(TRIGGER_DATA));
    
    return 0;
}
