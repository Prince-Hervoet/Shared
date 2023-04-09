#include <bits/stdc++.h>
using namespace std;

vector<long long> factors;
vector<long long> counts;
int main()
{
    long long n;
    cin >> n;
    int count = -1;
    for (long long i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
        {
            factors.push_back(i);
            counts.push_back(1);
            n /= i;
            count++;
        }
        else
        {
            continue;
        }
        while (n % i == 0)
        {
            counts[count] += 1;
            n /= i;
        }
    }
    if (n > 1)
    {
        factors.push_back(n);
        counts.push_back(1);
    }

    long long ans = 1;
    for (int i = 0; i < factors.size(); i++)
    {
        if ((counts[i] & 1) == 1)
        {
            ans *= factors[i];
        }
    }
    cout << ans;
    return 0;
}