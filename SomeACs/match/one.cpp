#include <iostream>
#include <string>
#include <map>
// 状态，判断当前是指令还是常量
#define COMMAND 1
#define CONST 2
void action(std::string &str);
void calFactor(int *maxFactor, int *minFactor, int status, int count, int type, bool isPos);

int main()
{
    std::string s = "d61+3d5+2-2d3+2d5";
    action(s);
    return 0;
}

void action(std::string &str)
{
    std::map<int, int> countMp;
    int status = CONST;
    bool isPos = true;
    int count = 1, type = 1, value = 0;
    int max = 0, min = 0, f1 = 0, f2 = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '+' || str[i] == '-')
        {
            calFactor(&f1, &f2, status, count, value, isPos);
            max += f1;
            min += f2;
            if (status == COMMAND)
            {
                // 只有是命令才放入map中统计，常量不需要
                countMp[value] = countMp[value] + count;
            }
            isPos = str[i] == '+' ? true : false;
            status = CONST;
            value = 0;
            type = 1;
            count = 1;
        }
        else if (str[i] == 'd')
        {
            status = COMMAND;
            count = value == 0 ? count : value;
            value = 0;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            // 这里需要好好理解，遇到多位数字形式的字符串时怎么转化成int，每次乘以10就可以做到（可以手算一下）
            value = value * 10 + (int)(str[i] - '0');
        }
    }
    // 这里是处理循环退出之后没算进去最后一次的问题
    if (status == COMMAND)
    {
        countMp[value] = countMp[value] + count;
    }
    calFactor(&f1, &f2, status, count, value, isPos);
    max += f1;
    min += f2;
    for (std::map<int, int>::iterator it = countMp.begin(); it != countMp.end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
    std::cout << min << " " << max << std::endl;
}

// 计算最大值和最小值 （最大就是正的取最大可能性，负的取最小可能性；最小则相反）
void calFactor(int *maxFactor, int *minFactor, int status, int count, int type, bool isPos)
{
    if (status == COMMAND)
    {
        *maxFactor = isPos ? (count * type) : (-count);
        *minFactor = isPos ? (count) : (-count * type);
    }
    else if (status == CONST)
    {
        *maxFactor = isPos ? type : (-type);
        *minFactor = isPos ? type : (-type);
    }
}