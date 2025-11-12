#define LIMK_MESSAGE_LENGTH 160

typedef struct
{
int type;
char message[LIMK_MESSAGE_LENGTH];
char map_file[80];
char link_name[20];
}LINK_EVENT;

typedef struct
{
 char name[20];
 int x, y, w, h;
 int event_chosen;
 int type;
 LINK_EVENT event[5]; 
}LINK_DATA;

#include <stdio.h>

int main() {
    printf("With LIMK_MESSAGE_LENGTH=160:\n");
    printf("  sizeof(LINK_EVENT) = %zu\n", sizeof(LINK_EVENT));
    printf("  sizeof(LINK_DATA) = %zu\n", sizeof(LINK_DATA));
    return 0;
}
