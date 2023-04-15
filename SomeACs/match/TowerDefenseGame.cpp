/*
    现在有一个塔防游戏，可以建造三种防御塔抵抗进攻者的进攻。
    进攻者每次都走同样长度为A个单位的路线，且他们的进攻速度为E秒/单位长度，每个防御塔占一个单位长度，
    防御搭的类型如攻击塔:直接攻击，当进攻者经过塔时，对进攻者每秒造成B点伤害放毒塔:释放毒液，当攻击者走过这座塔之后，每秒受到C点伤减速塔: 当进攻者经过这座塔之后，经过每一个单位长度的时间变为E+D秒塔的效果都可以叠加，
    比如经过两个放毒塔之后，那么他每秒受到的伤害为2*C，经过2座减速塔之后，速度变为E+2D秒/单位长度

    输入
        5 4 3 2 1
    输出
        82
*/

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