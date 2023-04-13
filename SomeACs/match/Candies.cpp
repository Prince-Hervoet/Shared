#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> packs;
int flags[25];

int action(int index, int m)
{
    int i = 1;
    for (; i <= m; i++)
    {
        if (flags[i] == 0)
        {
            break;
        }
    }
    if (i > m)
    {
        return 0;
    }
    if (index == packs.size())
    {
        return -1;
    }
    int ans = 999999999;
    for (i = 0; i < packs[index].size(); i++)
    {
        flags[packs[index][i]] += 1;
    }
    int temp = action(index + 1, m);
    if (temp != -1)
    {
        ans = min(ans, temp + 1);
    }
    for (i = 0; i < packs[index].size(); i++)
    {
        flags[packs[index][i]] -= 1;
    }
    temp = action(index + 1, m);
    if (temp != -1)
    {
        ans = min(ans, temp);
    }
    ans = ans == 999999999 ? -1 : ans;
    return ans;
}

int main()
{
    int n, m, k;
    cin >> n;
    cin >> m;
    cin >> k;
    for (int i = 0; i < n; i++)
    {
        vector<int> t;
        for (int p = 0; p < k; p++)
        {
            int temp;
            cin >> temp;
            t.push_back(temp);
        }
        packs.push_back(t);
    }
    cout << action(0, m);
    return 0;
}
