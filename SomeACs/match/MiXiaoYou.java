package someacs.match;

import java.util.Scanner;

/**
 * 
 */
public class MiXiaoYou {
    public static void main(String[] args) {
        Scanner s = new Scanner(System.in);
        int n = s.nextInt();
        int[] arr = new int[n];
        int[][] buffer = new int[n + 1][n + 1];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = s.nextInt();
        }
        for (int i = 0; i < arr.length; i++) {
            for (int k = 0; k < n; k++) {
                buffer[i][k] = -1;
            }
        }
        System.out.println(process5(arr, -1, 1, buffer));
    }

    public static long process5(int[] arr, int index, int num, int[][] buffer) {
        if (index == arr.length) {
            return 0;
        }
        if (index >= 0 && buffer[index][num] != -1) {
            return buffer[index][num];
        }
        int ans = 0;
        for (int i = index + 1; i < arr.length; i++) {
            if (num > 1 && (arr[i] % arr[index] == 0)) {
                ans += 1;
            }
            ans += process5(arr, i, num + 1, buffer);
        }
        if (index >= 0) {
            buffer[index][num] = ans;
        }
        return ans;
    }

}