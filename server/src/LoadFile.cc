#include "LoadFile.h"

LoadFile *LoadFile::pointer = LoadFile::getInstance();

// 用于搜索功能
std::shared_ptr<vector<pair<string, int>>> LoadFile::_p_dict {nullptr}; //词典
std::shared_ptr<unordered_map<int, pair<int, int>>> LoadFile::_p_offset_map {nullptr}; //网页偏移
std::shared_ptr<unordered_map<string, vector<pair<int, double>>>> LoadFile::_p_invertIndexTable {nullptr}; //倒排索引
std::shared_ptr<unordered_set<string>> LoadFile::_p_stop_words {nullptr}; //停用词
// 用于推荐词功能
std::shared_ptr<vector<pair<string, int>>> LoadFile::_dict {nullptr}; // 词典 TODO：改成static 25-6-24 22点33分 //修改为数据成员
std::shared_ptr<map<string, set<int>>>     LoadFile::_index {nullptr};