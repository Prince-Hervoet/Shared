package someacs.com;

import java.util.*;

public class Two {
    public static void main(String[] args) {
        int[] arr1 = { 2, 5, 6, 7, 3, 19, 4, 2, 5, 9 };
        int[] arr2 = { 2, 3, 5, 7, 6 };
        int[] ans = new int[arr1.length];
        int run = 0;
        int flag = ans.length - 1;
        Map<Integer, Integer> map = new HashMap<>();
        for (int i = 0; i < arr1.length; i++) {
            if (!map.containsKey(arr1[i])) {
                map.put(arr1[i], 1);
            } else {
                map.put(arr1[i], map.get(arr1[i]) + 1);
            }
        }

        for (int i = 0; i < arr2.length; i++) {
            if (map.containsKey(arr2[i])) {
                int count = map.get(arr2[i]);
                for (int k = 0; k < count; k++) {
                    ans[run] = arr2[i];
                    run++;
                }
                map.remove(arr2[i]);
            }
        }
        flag = run;
        Set<Integer> set = map.keySet();
        for (Integer key : set) {
            int count = map.get(key);
            for (int i = 0; i < count; i++) {
                ans[run] = key;
                run++;
            }
        }
        Arrays.sort(ans, flag, ans.length);
        for (int i = 0; i < ans.length; i++) {
            System.out.print(ans[i] + " ");
        }
    }
}
