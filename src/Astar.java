import java.util.Comparator;
import java.util.PriorityQueue;

public class Astar {

    private PriorityQueue<Point> pq;

    private boolean[][] flags;

    private Point[] ans;

    private int steps;

    private static final int[][] DIRS = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    public Astar() {
    }

    public void start(int[][] mp, int x1, int y1, int x2, int y2) {
        Point start = new Point(x1, y1);
        Point end = new Point(x2, y2);
        flags = new boolean[mp.length][mp[0].length];
        pq = new PriorityQueue<>(Comparator.comparingInt(o -> o.cost));
        ans = new Point[mp.length * mp[0].length];
        steps = -1;
        aStar(mp, start, end);
        if (steps == -1) {
            System.out.println("无法抵达终点! ");
        } else {
            System.out.println("最少需要: " + steps + " 步");
        }
        for (int i = 0; i < steps; i++) {
            if (i != steps - 1) {
                System.out.print(ans[i] + " -> ");
            } else {
                System.out.print(ans[i]);
            }
        }
        System.out.println();
        for (int i = 0; i < flags.length; i++) {
            for (int k = 0; k < flags[0].length; k++) {
                if (flags[i][k]) {
                    System.out.print("# ");
                } else {
                    System.out.print("8 ");
                }
            }
            System.out.println();
        }
    }

    private boolean aStar(int[][] mp, Point now, Point end) {
        pq.add(now);
        while (!pq.isEmpty()) {
            Point poll = pq.poll();
            if (poll.x == end.x && poll.y == end.y) {
                flags[poll.x][poll.y] = true;
                ans[poll.curCost] = new Point(poll.x, poll.y);
                steps = poll.curCost;
                return true;
            }
            if (flags[poll.x][poll.y]) {
                continue;
            }
            ans[poll.curCost] = poll;
            flags[poll.x][poll.y] = true;
            for (int i = 0; i < 4; i++) {
                int x = poll.x + DIRS[i][0];
                int y = poll.y + DIRS[i][1];
                if (x >= 0 && x < mp.length && y >= 0 && y < mp[0].length && mp[x][y] != 1 && !flags[x][y]) {
                    Point temp = new Point(x, y);
                    temp.curCost = poll.curCost + 1;
                    temp.prevCost = manhadun(temp, end);
                    temp.cost = temp.prevCost + temp.curCost;
                    pq.add(temp);
                }
            }

        }
        return false;
    }

    public static int manhadun(Point a, Point b) {
        int remainX = a.x >= b.x ? a.x - b.x : b.x - a.x;
        int remainY = a.y >= b.y ? a.y - b.y : b.y - a.y;
        return remainX + remainY;
    }
}

class Point {
    int x;
    int y;
    int cost;
    int curCost;
    int prevCost;

    @Override
    public String toString() {
        return "Point{" +
                "x=" + x +
                ", y=" + y +
                ", cost=" + cost +
                '}';
    }

    public Point() {
    }

    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }

}
