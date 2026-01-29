#ifndef __PAGELIBPREPROCESSOR__H
#define __PAGELIBPREPROCESSOR__H
#include "../server/log4cpp.h"
#include "../include/tinyxml2.h"
// #include "../simhash/Simhasher.hpp"
// #include "../simhash/jenkins.h"

#include "../cppjieba/Jieba.hpp"
// const char *const DICT_PATH      = "../dict/jieba.dict.utf8";
// const char *const HMM_PATH       = "../dict/hmm_model.utf8";
// const char *const USER_DICT_PATH = "../dict/user.dict.utf8";
// const char *const IDF_PATH       = "../dict/idf.utf8";
// const char *const STOP_WORD_PATH = "../dict/stop_words.utf8";

#include "WebPage.h"

#include <math.h> //倒排索引TF/IDF算法
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>

using namespace std;
using namespace tinyxml2;
// using namespace simhash;

struct RSSIteam {
	string _title;
	string _link;
	string _description;
	string _content;
};

class Configuration;

//网页库预处理类
class PageLibProcessor {
public:
	PageLibProcessor() {}

	PageLibProcessor(Configuration &conf, SplitTool *splitTool);
	~PageLibProcessor();

	//执行预处理
	void doProcess();

	void doProcess2();

	//根据配置信息读取网页库和位置偏移库的内容
	int readInforFromFile(unordered_map<string, map<int, int>> &_wd_page_fqs, unordered_map<string, int> &_wd_sum_fqs);

	//对冗余的网页进行去重
	void curRedundantPages();

	//创建倒排索引表
	void buildInvertIndexTable(); //创建倒排索引表

	//将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上
	void storeOnDisk();

private:
	//网页库的容器对象
	vector<WebPage> _pageList;
	//网页偏移库对象
	unordered_map<int, pair<int, int>> _offsetLib;

	//倒排索引表对象 string 单词  pair:int 文章id, double 权值
	unordered_map<string, vector<pair<int, double>>> _invertIndexTable;
	//JieBa字符处理指针
	SplitTool *_wordCutter;
};

#endif //__PAGELIBPREPROCESSOR__H