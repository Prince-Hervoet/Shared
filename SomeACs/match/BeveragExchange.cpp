#include <bits/stdc++.h>
using namespace std;

int main()
{
    int bottle, lid = 0, ans = 0;
    cin >> bottle;
    lid = 0;
    while (bottle > 0)
    {
        ans += bottle;
        lid += bottle;
        bottle = lid / 3;
        lid -= bottle * 3;
    }
    cout << ans;
    return 0;
}