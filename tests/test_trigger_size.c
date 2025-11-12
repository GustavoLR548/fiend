#include <stdio.h>
#include "src/trigger.h"

int main() {
    printf("sizeof(CONDITION_DATA) = %zu\n", sizeof(CONDITION_DATA));
    printf("sizeof(EVENT_DATA) = %zu\n", sizeof(EVENT_DATA));
    printf("sizeof(TRIGGER_DATA) = %zu\n", sizeof(TRIGGER_DATA));
    printf("MAX_CONDITION_NUM = %d\n", MAX_CONDITION_NUM);
    printf("MAX_EVENT_NUM = %d\n", MAX_EVENT_NUM);
    
    // Calculate expected size manually
    // TRIGGER_DATA: char name[30] + int active + int type + CONDITION_DATA[5] + EVENT_DATA[5]
    // CONDITION_DATA: char used + int type + char x[40] + int correct + char z[40] + char string1[40] + char string2[40] + char logic
    // EVENT_DATA: char used + int type + char x[40] + char y[40] + char z[40] + char string1[40] + char string2[40] + char text[160] + char logic
    
    int cond_size = 1 + 4 + 40 + 4 + 40 + 40 + 40 + 1; // = 170
    int event_size = 1 + 4 + 40 + 40 + 40 + 40 + 40 + 160 + 1; // = 366
    int trigger_size = 30 + 4 + 4 + (cond_size * 5) + (event_size * 5); // = 30+8+850+1830 = 2718
    
    printf("\nExpected (32-bit packed):\n");
    printf("  CONDITION_DATA: %d\n", cond_size);
    printf("  EVENT_DATA: %d\n", event_size);
    printf("  TRIGGER_DATA: %d\n", trigger_size);
    
    return 0;
}
