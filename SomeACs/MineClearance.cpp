/*
小明最近迷上了一款名为《扫雷》的游戏。其中有一个关卡的任务如下，在一个二维平面上放置着 $n$ 个炸雷，第 $i$ 个炸雷 $\left(x_{i}, y_{i}, r_{i}\right)$ 表示在坐标 $\left(x_{i}, y_{i}\right)$ 处存在一个炸雷，它的爆炸范围是以半径为 $r_{i}$ 的一个圆。

为了顺利通过这片土地，需要玩家进行排雷。玩家可以发射 $m$ 个排雷火箭，小明已经规划好了每个排雷火箭的发射方向，第 $j$ 个排雷火箭 $\left(x_{j}, y_{j}, r_{j}\right)$ 表示这个排雷火箭将会在 $\left(x_{j}, y_{j}\right)$ 处爆炸，它的爆炸范围是以半径为 $r_{j}$ 的一个圆，在其爆炸范围内的炸雷会被引爆。同时，当炸雷被引爆时，在其爆炸范围内的炸雷也会被引爆。现在小明想知道他这次共引爆了几颗炸雷?

你可以把炸雷和排雷火箭都视为平面上的一个点。一个点处可以存在多个炸雷和排雷火箭。当炸雷位于爆炸范围的边界上时也会被引爆。

## 输入格式

输入的第一行包含两个整数 $n$、$m$。

接下来的 $n$ 行, 每行三个整数 $x_{i}, y_{i}, r_{i}$, 表示一个炸雷的信息。

再接下来的 $m$ 行，每行三个整数 $x_{j}, y_{j}, r_{j}$, 表示一个排雷火箭的信息。

## 输出格式

输出一个整数表示答案。

## 样例 #1

### 样例输入 #1

```
2 1
2 2 4
4 4 2
0 0 5
```

### 样例输出 #1

```
2
```

## 提示

**【样例说明】**

示例图如下, 排雷火箭 1 覆盖了炸雷 1 , 所以炸雷 1 被排除; 炸雷 1 又覆 盖了炸雷 2 , 所以炸雷 2 也被排除。

![](https://luogu.oss-cn-hangzhou.aliyuncs.com/upload/vjudge_pic/lanqiao/2022_09_29_3d61ad9326a0012c9fdag-13.jpg)

**【评测用例规模与约定】**

对于 $40 \%$ 的评测用例: $0 \leq x, y \leq 10^{9}, 0 \leq n, m \leq 10^{3}, 1 \leq r \leq 10$.

对于 $100 \%$ 的评测用例: $0 \leq x, y \leq 10^{9}, 0 \leq n, m \leq 5 \times 10^{4}, 1 \leq r \leq 10$.


蓝桥杯 2022 省赛 B 组 H 题。
*/

#include <bits/stdc++.h>
using namespace std;

typedef struct
{
    int x;
    int y;
    int r;
    bool isBoom;
} Mine;

typedef struct
{
    int x;
    int y;
    int r;
} Rocket;
vector<Mine> mines;
vector<Rocket> rockets;

bool cmp(Mine e1, Mine e2)
{
    return e1.x < e2.x;
}

long dis(long x1, long y1, long x2, long y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

int action(int x, int y, int r)
{
    int left = 0, right = mines.size() - 1;
    int ans = 0;
    int xl = x - r, xr = x + r;
    int mineL = 999999999, mineR = -1;
    while (left <= right)
    {
        int mid = ((right - left) >> 1) + left;
        if (mines[mid].x < xl)
        {
            left = mid + 1;
        }
        else if (mines[mid].x >= xl)
        {
            right = mid - 1;
            mineL = mineL < mid ? mineL : mid;
        }
    }
    left = 0, right = mines.size() - 1;
    while (left <= right)
    {
        int mid = ((right - left) >> 1) + left;
        if (mines[mid].x > xr)
        {
            right = mid - 1;
        }
        else if (mines[mid].x <= xr)
        {
            left = mid + 1;
            mineR = mineR > mid ? mineR : mid;
        }
    }
    for (int i = mineL; i <= mineR; i++)
    {
        if (!mines[i].isBoom && dis(x, y, mines[i].x, mines[i].y) <= r * r)
        {
            mines[i].isBoom = true;
            ans++;
            ans += action(mines[i].x, mines[i].y, mines[i].r);
        }
    }
    return ans;
}

int main()
{
    int n, m;
    cin >> n;
    cin >> m;

    for (int i = 0; i < n; i++)
    {
        Mine a;
        cin >> a.x;
        cin >> a.y;
        cin >> a.r;
        a.isBoom = false;
        mines.push_back(a);
    }

    for (int i = 0; i < m; i++)
    {
        Rocket ro;
        cin >> ro.x;
        cin >> ro.y;
        cin >> ro.r;
        rockets.push_back(ro);
    }
    sort(mines.begin(), mines.end(), cmp);
    int ans = 0;
    for (int i = 0; i < rockets.size(); i++)
    {
        ans += action(rockets[i].x, rockets[i].y, rockets[i].r);
    }
    cout << ans;
    return 0;
}