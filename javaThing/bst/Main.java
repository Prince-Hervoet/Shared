package javaThing.bst;

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
        test.insert(123, 3434);
        test.insert(145, 999);
        test.insert(166, 5675);
        test.insert(34, 3434);
        test.insert(1, 999);
        test.insert(3, 5675);
        test.insert(989, 3434);
        test.insert(45, 999);
        test.insert(4567, 5675);
        System.out.println(test.size());
    }
}
