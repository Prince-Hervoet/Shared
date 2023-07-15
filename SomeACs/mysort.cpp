#include <iostream>
#include <random>
#include <stack>

// 冒泡排序
void bubbleSort(int arr[], int len)
{
    int x = len - 1, temp = 0;
    bool flag = true;
    for (int i = 0; i < x; i++)
    {
        for (int k = 0; k < x - i; k++)
        {
            if (arr[k] > arr[k + 1])
            {
                flag = false;
                temp = arr[k];
                arr[k] = arr[k + 1];
                arr[k + 1] = temp;
            }
        }
        if (flag)
        {
            break;
        }
        flag = true;
    }
}

// 快速排序
void quickSort(int arr[], int len)
{
    int ll = 0, rr = len - 1, temp = 0;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::stack<int> cur;
    if (ll < rr)
    {
        cur.push(rr);
        cur.push(ll);
    }

    while (cur.size() > 0)
    {
        ll = cur.top();
        cur.pop();
        rr = cur.top();
        cur.pop();
        std::uniform_int_distribution<int> distr(ll, rr);
        int xIndex = distr(eng);
        int x = arr[xIndex];
        arr[xIndex] = arr[ll];
        arr[ll] = x;
        int i = ll, k = rr;
        while (i < k)
        {
            while (i < k && arr[k] >= x)
            {
                k -= 1;
            }

            while (i < k && arr[i] <= x)
            {
                i += 1;
            }

            if (i < k)
            {
                temp = arr[i];
                arr[i] = arr[k];
                arr[k] = temp;
            }
        }
        arr[ll] = arr[i];
        arr[i] = x;
        if (ll < i - 1)
        {
            cur.push(i - 1);
            cur.push(ll);
        }
        if (i + 1 < rr)
        {
            cur.push(rr);
            cur.push(i + 1);
        }
    }
}

int main()
{
    int arr[] = {234, 4, 4, 56, 8, 7, 45, 2, 4, 43, 54, 123, 41, 344, 55, 46, 56, 345, 2334, 452334, 234, 4, 999, 4, 5, 4, 52, 34, 2, 90, 2, 2, 4};
    quickSort(arr, sizeof(arr) / 4);
    for (int i = 0; i < sizeof(arr) / 4; i++)
    {
        std::cout << arr[i] << std::endl;
    }

    return 0;
}
