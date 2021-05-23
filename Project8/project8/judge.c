#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *stream_ans = fopen("output.txt", "r");
	FILE *stream_std = fopen("correct.txt", "r");
    
    int a, b, c;
    int ac = 0; // 0 => ac; 1 => wrong

    while(~fscanf(stream_ans, "Virtual address: %d Physical address: %d Value: %d\n", &a, &b, &c)) {
        int d, e, f;
        if (fscanf(stream_std, "Virtual address: %d Physical address: %d Value: %d\n", &d, &e, &f) == EOF) {
            printf("File length not match!\n");
			ac = 1;
			break;
        }
        if (c != f) {
            printf("Wrong answer!\n");
			ac = 1;
			break;
        }
    }
    
    if (ac == 0) printf("All correct!\n");

    fclose(stream_ans);
    fclose(stream_std);

    return 0;
}