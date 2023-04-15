#include <bits/stdc++.h>
using namespace std;

int A, B, C, D, E;
// C毒伤 D减速伤
int hurtC;
int buffers[1000][100][100];
int action(int index)
{
    if (index > A)
    {
        return 0;
    }
    int base = hurtC * E, ans = 0;
    int a = 0;
    if (buffers[index][hurtC][E] != -1)
    {
        return buffers[index][hurtC][E];
    }
    // 1
    a = action(index + 1);
    ans = max(ans, base + E * B + a);

    hurtC += C;
    a = action(index + 1);
    hurtC -= C;
    ans = max(ans, base + a);

    E += D;
    a = action(index + 1);
    E -= D;
    ans = max(ans, base + a);
    buffers[index][hurtC][E] = ans;
    return ans;
}

int main()
{
    cin >> A;
    cin >> B;
    cin >> C;
    cin >> D;
    cin >> E;
    for (int i = 0; i < 1000; i++)
    {
        for (int k = 0; k < 100; k++)
        {
            for (int p = 0; p < 100; p++)
            {
                buffers[i][k][p] = -1;
            }
        }
    }
    cout << action(1);
    return 0;
}