#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int *array;
int *result;

struct function_args {
    int start, end, mid;
};

int cmp(const void *a, const void *b) {
	return *((int *)a) - *((int *)b);
}

void* sort_thread(void *arg) {
    struct function_args * args = (struct function_args *)arg;
    int start = args -> start;
    int end = args -> end;
    if (start > end) return NULL;
    
    qsort(array + start, end - start + 1, sizeof(int), cmp); // sort.

    return NULL;
}

void* merge_thread(void *arg) {
    struct function_args * args = (struct function_args *)arg;
    int start = args -> start;
    int mid = args -> mid;
    int end = args -> end;
    
    int loc1, loc2, i;
    loc1 = start;
    loc2 = mid + 1;
    i = 0;
    
    while (i <= end) {
        int flag = 1; // select which sub-array
        if (loc1 <= mid && (array[loc1] < array[loc2])) flag = 0;
        if (loc1 > mid) flag = 1;
        if (loc2 > end) flag = 0;
        if (flag == 0) {
            result[i++] = array[loc1++];
        } else {
            result[i++] = array[loc2++];
        }
    }
    return NULL;
}


int main() {
    int n;
    printf("Input the length of array:");
    scanf("%d", &n); // get the length of array.
    
    array = (int *) malloc (n * sizeof(int)); // allocate memory.
    result = (int *) malloc (n * sizeof(int));
    
    srand((unsigned int)time(0));
    printf("Array: [ "); // generate random array and print it.
    for(int i = 0; i < n; i++) {
        array[i] = rand() % 100;
        printf("%d ", array[i]);
    }
    printf("]\n");

    int start, mid, end; // divide array into two sub-array.
    start = 0;
    mid = n / 2;
    end = n - 1;

    struct function_args args[2]; // arguments for sort threads.
    args[0].start = start;
    args[0].end = mid;
    args[1].start = mid + 1;
    args[1].end = end;

    pthread_t sort_th[2]; // create 2 sort thread to sort sub-array.
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&sort_th[i], NULL, sort_thread, &args[i])) {
            printf("Can't create thread.\n");
            return 1;
        }
    }

    for (int i = 0; i < 2; i++) { // join 2 sort thread.
        void *out;
        if (pthread_join(sort_th[i], &out)) {
            printf("Can't join thread.\n");
            return 1;
        }
    }
    
    args[0].start = start; // arguments for merge thread.
    args[0].mid = mid;
    args[0].end = end;
    
    pthread_t merge_th; // create merge thread.
    if (pthread_create(&merge_th, NULL, merge_thread, &args[0])) {
        printf("Can't create thread.\n");
        return 1;
    }
    
    void *out; 
    if (pthread_join(merge_th, &out)) { // join merge thread.
        printf("Can't join thread.\n");
        return 1;
    }

    printf("Result: [ "); // print the result.
    for(int i = 0; i < n; i++) {
        printf("%d ", result[i]);
    }
    printf("]\n");

    free(array);
    free(result);
    return 0;
}