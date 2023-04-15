/*
## 题目描述

对于一棵多叉树，我们可以通过“左孩子右兄弟”表示法，将其转化成一棵二叉树。

如果我们认为每个结点的子结点是无序的，那么得到的二叉树可能不唯一。换句话说，每个结点可以选任意子结点作为左孩子，并按任意顺序连接右兄弟。

给定一棵包含 $N$ 个结点的多叉树，结点从 $1$ 至 $N$ 编号，其中 $1$ 号结点是根，每个结点的父结点的编号比自己的编号小。请你计算其通过"左孩子右兄弟"表示法转化成的二叉树，高度最高是多少。（只有根结点这一个结点的树高度为 $0$）

例如如下的多叉树：

![](https://luogu.oss-cn-hangzhou.aliyuncs.com/upload/vjudge_pic/lanqiao/2022_09_29_d8f144a59f4c0ce9322ag-11.jpg)

可能有以下 $3$ 种 (这里只列出 $3$ 种, 并不是全部) 不同的 “左孩子右兄弟” 表示:

![](https://luogu.oss-cn-hangzhou.aliyuncs.com/upload/vjudge_pic/lanqiao/2022_09_29_d8f144a59f4c0ce9322ag-12.jpg)

其中最后一种高度最高, 为 $4$。

## 输入格式

输入的第一行包含一个整数 $N$ 。

以下 $N-1$ 行, 每行包含一个整数, 依次表示 $2$ 至 $N$ 号结点的父结点编号。

## 输出格式

输出一个整数表示答案。

## 样例 #1

### 样例输入 #1

```
5
1
1
1
2
```

### 样例输出 #1

```
4
```

## 提示

对于 $30 \%$ 的评测用例，$1 \leq N \leq 20$;

对于所有评测用例，$1 \leq N \leq 10^5$ 。
*/

#include <bits/stdc++.h>
using namespace std;
vector<int> tree[100002];

int action(int id)
{
    if (tree[id].size() == 0)
    {
        return 0;
    }
    int maxx = 0;
    for (int i = 0; i < tree[id].size(); i++)
    {
        maxx = max(maxx, action(tree[id][i]));
    }
    return tree[id].size() + maxx;
}

int main()
{
    int n = 0;
    cin >> n;
    for (int i = 2; i <= n; i++)
    {
        int temp = 0;
        cin >> temp;
        tree[temp].push_back(i);
    }
    cout << action(1);
}