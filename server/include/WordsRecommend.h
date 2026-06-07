#ifndef __WORDSRECOMMEND__H
#define __WORDSRECOMMEND__H

#include <string>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int editDistance(const std::string &lhs, const std::string &rhs);

struct CompareByDistance {

	bool operator()(const pair<int, string> &lhs, const pair<int, string> &rhs) const
	{
		// 这里使用字符串长度的差的绝对值作为编辑距离的简化版本
		//return abs(lhs.first - static_cast<int>(query.size())) > abs(rhs.first - static_cast<int>(query.size()));
		return lhs.first < rhs.first;
	}
};

void seekRecommendWd(string &_msg, map<string, int> &condidate, vector<string> &ret)
{
	priority_queue<pair<int, string>, vector<pair<int, string>>, CompareByDistance> pq;

	// 假设我们存储字符串长度作为与查询的“编辑距离”的简化表示

	int distence;
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: topFreq 未初始化即使用（未定义行为）
	// int topFreq;
	// FIX: 初始化为 0
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	int topFreq = 0;
	for(auto &elem : condidate)
	{
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// BUG: 运算符优先级 — '=' 优先级低于 '<'，distence 始终为 0 或 1（bool）
		// if(distence = editDistance(_msg, elem.first) < 5)
		// FIX: 添加显式括号保证正确赋值
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		if((distence = editDistance(_msg, elem.first)) < 5) //编辑距离小于3
		{
			if(topFreq < elem.second)
				topFreq = elem.second;

			if(elem.second <= topFreq - 5) //频率不小于最高频率 减5
				pq.push({distence, elem.first});
		}
	}

	int maxRecommend = 15;
	while(!pq.empty() && maxRecommend--)
	{

		auto top = pq.top();
		ret.emplace_back(top.second);
		//cout << "String: " << top.second << ", 'Distance': " << top.first << endl;
		pq.pop();
	}
}

//清洗中文的.cc文件中重复写了nBytesCode 因为没包含这个文件
size_t nBytesCode(const char ch)
{
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}
std::size_t length(const std::string &str)
{
	std::size_t ilen = 0;
	for(std::size_t idx = 0; idx != str.size(); ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}

int triple_min(const int &a, const int &b, const int &c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int editDistance(const std::string &lhs, const std::string &rhs)
{
	//计算最小编辑距离-包括处理中英文
	size_t lhs_len = length(lhs);
	size_t rhs_len = length(rhs);
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: VLA（变长数组）非标准 C++，存在栈溢出风险
	// int    editDist[lhs_len + 1][rhs_len + 1];
	// FIX: 改用 vector 在堆上分配
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	vector<vector<int>> editDist(lhs_len + 1, vector<int>(rhs_len + 1, 0));
	for(size_t idx = 0; idx <= lhs_len; ++idx)
	{
		editDist[idx][0] = idx;
	}
	for(size_t idx = 0; idx <= rhs_len; ++idx)
	{
		editDist[0][idx] = idx;
	}
	std::string sublhs, subrhs;
	for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,
	                ++lhs_idx)
	{
		size_t nBytes = nBytesCode(lhs[lhs_idx]);
		sublhs        = lhs.substr(lhs_idx, nBytes);
		lhs_idx += (nBytes - 1);
		for(std::size_t dist_j = 1, rhs_idx = 0;
		    dist_j <= rhs_len; ++dist_j, ++rhs_idx)
		{
			nBytes = nBytesCode(rhs[rhs_idx]);
			subrhs = rhs.substr(rhs_idx, nBytes);
			rhs_idx += (nBytes - 1);
			if(sublhs == subrhs)
			{
				editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
			}
			else
			{
				editDist[dist_i][dist_j] =
				    triple_min(editDist[dist_i][dist_j - 1] + 1,
				               editDist[dist_i - 1][dist_j] + 1,
				               editDist[dist_i - 1][dist_j - 1] + 1);
			}
		}
	}
	return editDist[lhs_len][rhs_len];
}

#endif // __WORDSRECOMMEND__H
