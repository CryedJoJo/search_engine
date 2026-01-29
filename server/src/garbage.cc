// #include <iostream>
// #include <queue>
// #include <string>
// #include <vector>
// #include <algorithm> // 用于 abs

// using namespace std;

// struct CompareByDistance {
// 	string query;
// 	CompareByDistance(const string &q)
// 	    : query(q)
// 	{
// 	}

// 	bool operator()(const pair<int, string> &lhs, const pair<int, string> &rhs) const
// 	{
// 		// 这里使用字符串长度的差的绝对值作为编辑距离的简化版本
// 		//return abs(lhs.first - static_cast<int>(query.size())) > abs(rhs.first - static_cast<int>(query.size()));
// 	}
// };

// int main()
// {

// 	priority_queue<pair<int, string>, vector<pair<int, string>>, CompareByDistance> pq(CompareByDistance(" "));

// 	// 假设我们存储字符串长度作为与查询的“编辑距离”的简化表示
// 	pq.push({5, "hello"});
// 	pq.push({4, "test1"});
// 	pq.push({4, "test2"});
// 	pq.push({6, "world"});

// 	while(!pq.empty())
// 	{
// 		auto top = pq.top();
// 		pq.pop();
// 		cout << "String: " << top.second << ", 'Distance': " << top.first << endl;
// 	}

// 	return 0;
// }