#include <stdio.h>

// We need (91160 - 91096) / 10 = 6.4 bytes per link
// But that's impossible. Let me check if maybe num_of_links is wrong

// OR maybe triggers don't start at 91160!
// Let me search for what structure "george1" belongs to

int main() {
    // After reading 10 links starting at 91096, we should end up at the trigger start
    // Triggers are at 91160 (where "Nothing" is)
    // So 10 links = 64 bytes
    //That's only 6.4 bytes per link - IMPOSSIBLE
    
    // This means either:
    // 1. num_of_links is wrong in the header
    // 2. The file doesn't have links where we think
    // 3. The trigger data doesn't start where we think
    
    printf("Expected: start=91096, end after 10 links=%ld, but triggers at=91160\n", 91096L + 10*1364);
    printf("Difference: %ld bytes\n", 91160L - (91096L + 64L));
    printf("This suggests links take only 64 bytes total, or 6.4 bytes each - IMPOSSIBLE!\n");
    
    return 0;
}
