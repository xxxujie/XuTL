#include <cstdio>
#include <vector>

int FindNumber(const std::vector<int>& nums)
{
    int right_sum = 0;  // 右半边数总和
    int left_sum = 0;   // 左半边数总和
    // 先看第一个数（下标为 0）
    for (int i = 1; i < nums.size(); ++i)
    {
        right_sum += nums[i];  // 右半边总和就是下标为 1 至 nums.size()-1
                               // 的所有数之和 第一个数的左半边为 0
    }
    // 如果符合要求，返回下标 0
    if (right_sum - left_sum == nums[0])
    {
        return 0;
    }
    // 从第二个数遍历到最后一个
    for (int i = 1; i < nums.size(); ++i)
    {
        // 每次右半边总和减去当前下标的数
        right_sum -= nums[i];
        // 左半边总和加上前一个下标的数
        left_sum += nums[i - 1];
        // 如果符合要求，返回当前下标
        if (right_sum - left_sum == nums[i])
        {
            return i;
        }
    }
    return -1;  // 如果没有符合要求的数字，返回 -1
}

int main()
{
    std::vector<int> nums{5, 4, 3, 8, 4, 15, 8, 5, 6, 12, 8};
    int index = FindNumber(nums);
    printf("第一个符合要求的数字的下标为：%d\n", index);
    return 0;
}