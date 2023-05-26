package someacs.some;

import java.util.Random;

public class QuickSort {
    public static void main(String[] args) {
        int[] test = { 233, 1, 23, 4, 54, 23, 21, 123, 345345, 4545, 123, 111, 2223, 3434, 35, 4, 45, 45, 34, 2 };
        quick(test, 0, test.length - 1);
        for (int i = 0; i < test.length; i++) {
            System.out.print(test[i] + " ");
        }
    }

    public static void quick(int[] nums, int left, int right) {
        if (left >= right) {
            return;
        }
        Random random = new Random();
        int nextInt = random.nextInt(right - left + 1);
        nextInt += left;
        int temp = nums[nextInt];
        nums[nextInt] = nums[left];
        nums[left] = temp;
        int x = nums[left];
        int ll = left, rr = right;
        while (ll <= rr) {
            while (ll <= rr && nums[rr] >= x) {
                rr--;
            }
            while (ll <= rr && nums[ll] <= x) {
                ll++;
            }
            if (ll <= rr) {
                temp = nums[ll];
                nums[ll] = nums[rr];
                nums[rr] = temp;
            }
        }
        nums[left] = nums[rr];
        nums[rr] = x;
        quick(nums, left, rr - 1);
        quick(nums, rr + 1, right);
    }

}
