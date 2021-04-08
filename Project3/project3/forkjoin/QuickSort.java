import java.util.concurrent.*;
import java.util.Scanner;

public class QuickSort extends RecursiveAction {
    private static final long serialVersionUID = 1L;
    static final int THRESHOLD = 10;
    private int start, end;
    private Integer[] array;

    public QuickSort(int start, int end, Integer[] array) {
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
        } else { // divide into sub-sort
            int mid = start + (end - start) / 2;
            Integer comp = array[start];
            int l, h;
            l = start;
            h = end;

            while (l < h) {
                while (l < h && array[h].compareTo(comp) >= 0) h--;
                if (l < h) array[l++] = array[h];
				while (l < h && array[l].compareTo(comp) <= 0) l++;
				if (l < h) array[h--] = array[l];
            }
            array[l] = comp;
            
            QuickSort taskl = new QuickSort(start, l - 1, array);
            QuickSort taskr = new QuickSort(l + 1, end, array);

            taskl.fork();
            taskr.fork();

            taskl.join();
            taskr.join();
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
        
        QuickSort task = new QuickSort(0, n - 1, array);
        pool.invoke(task);

        System.out.print("Result: [ ");
        for (int i = 0; i < n; ++ i) {
            System.out.print(array[i] + " ");
        }
		System.out.println("]");	
    }
}
