package someacs.com;

import java.util.Scanner;

public class Decoding {
    public static void main(String[] args) {
        Scanner s = new Scanner(System.in);
        String str = s.nextLine();
        StringBuilder ans = new StringBuilder();
        for (int i = 0; i < str.length(); i++) {
            char now = str.charAt(i);
            if (now >= '0' && now <= '9') {
                continue;
            }
            if (i + 1 < str.length()) {
                char c = str.charAt(i + 1);
                if (c >= '0' && c <= '9') {
                    int count = c - '0';
                    for (int k = 0; k < count; k++) {
                        ans.append(now);
                    }
                } else {
                    ans.append(now);
                }
            } else {
                ans.append(now);
            }
        }
        System.out.println(ans.toString());
    }

}
