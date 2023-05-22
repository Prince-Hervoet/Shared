package javaThings.bst;

public class Main {
    public static void main(String[] args) {
        BstStruct<Integer, Integer> test = new BstStruct<>(new BstStruct.Comparer<>() {
            @Override
            public int compare(Integer a, Integer b) {
                if (a > b) {
                    return 1;
                } else if (a < b) {
                    return -1;
                } else {
                    return 0;
                }
            }
        });
        long start = System.currentTimeMillis();
        for (int i = 0; i < 5000; i++) {
            test.insert(i, i);
        }
        long end = System.currentTimeMillis();
        System.out.println(test.get(1000002));
        System.out.println("time: " + (end - start) + "ms");
    }
}
