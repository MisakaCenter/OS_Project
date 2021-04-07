import java.util.concurrent.*;
import java.util.Scanner;

public class MergeSort extends RecursiveAction {
    private static final long serialVersionUID = 1L;
    static final int THRESHOLD = 10;
    private int start, end;
    private Integer[] array;

    public MergeSort(int start, int end, Integer[] array) {
        this.start = start;
        this.end = end;
        this.array = array;
    }

    protected void compute() {
        if (end - start < THRESHOLD) {
            for (int i = start; i < end; i++) {
                for (int j = i; j <= end; j++) {
                    if (array[i].compareTo(array[j]) > 0) {
                        Integer t = array[i];
                        array[i] = array[j];
                        array[j] = t;
                    }
                }
            }
        } else { // divide into sub-merge
            int mid = start + (end - start) / 2;
            
            MergeSort taskl = new MergeSort(start, mid, array);
            MergeSort taskr = new MergeSort(mid + 1, end, array);

            taskl.fork();
            taskr.fork();

            taskl.join();
            taskr.join();

            Integer[] result = new Integer [end - start + 1];
            
            int loc1, loc2, i; // merge taskl and taskr
            loc1 = start;
            loc2 = mid + 1;
            i = 0;
            
            while (i < end - start + 1) {
                int flag = 1; // select which sub-array
                if (loc1 > mid) flag = 1;
                else if (loc2 > end) flag = 0; 
                else if (loc1 <= mid && (array[loc1] < array[loc2])) flag = 0;
                if (flag == 0) {
                    result[i++] = array[loc1++];
                } else {
                    result[i++] = array[loc2++];
                }
            }
            
            for (int j = 0; j < end - start + 1; j++) { // merge array from result
                array[start + j] = result[j];
            }
        }
    }
    
    public static void main(String[] args) {
        ForkJoinPool pool = new ForkJoinPool();
        int n;

        Scanner input = new Scanner(System.in);
        System.out.print("Input the length of array:");
        n = input.nextInt();
        input.close();

        Integer[] array = new Integer [n];
        java.util.Random rand = new java.util.Random();

        System.out.print("Array: [ ");
        for (int i = 0; i < n; ++ i) {
            array[i] = rand.nextInt(1000);
            System.out.print(array[i] + " ");
        }
		System.out.println("]");	
        
        MergeSort task = new MergeSort(0, n - 1, array);
        pool.invoke(task);

        System.out.print("Result: [ ");
        for (int i = 0; i < n; ++ i) {
            System.out.print(array[i] + " ");
        }
		System.out.println("]");	
    }
}