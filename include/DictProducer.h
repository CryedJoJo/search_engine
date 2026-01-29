#ifndef __DICTPRODUCER__H
#define __DICTPRODUCER__H
#include "../server/log4cpp.h"
#include "Configuration.h"

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class SplitTool; //前向声明
class tool;

class DictProducer {
public:
	DictProducer(const string &dir);

	DictProducer(const string &, SplitTool *splitTool); //构造函数 专门处理中文

	~DictProducer();

	string cleanCnTxt(string filepath);

	//将英文文件中的大写字母换成小写
	string cleanEnTxt(string filepath);

	//创建英文字典
	void buildEnDict();

	//创建中文字典
	void buildCnDict();
	//构建索引
	void createIndex();
	//将词典写入文件

	void storeDict(const char *filepath);
	//获取文件绝对路径
	void getFiles();
	//存储某个单词
	void pushDict(pair<string, int> elem);

public:
	//日志单例
	Mylogger *log = Mylogger::getInstance();

private:
	//语料库文件的绝对路径集合
	vector<string> _files;
	//词典
	vector<pair<string, int>> _dict;
	//分词工具
	map<string, set<int>> _index;
	//词典索引
	SplitTool    *_cuttor;
	Configuration _conf;
};

#endif