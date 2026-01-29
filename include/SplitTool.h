#ifndef __SPLITTOOL__H
#define __SPLITTOOL__H

#include "../cppjieba/Jieba.hpp"
#include "Configuration.h"

#include <vector>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
using namespace std;
const char *const DICT_PATH      = "../dict/jieba.dict.utf8";
const char *const HMM_PATH       = "../dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../dict/user.dict.utf8";
const char *const IDF_PATH       = "../dict/idf.utf8";
const char *const STOP_WORD_PATH = "../dict/stop_words.utf8";

class Configuration;

class SplitTool {
public:
	virtual ~SplitTool() {}
	virtual vector<string> cut(const string &sentence) = 0;

	//网页JieBa
	virtual vector<string> cut(const string &sentence, int web) = 0;
};

class SplitToolCppJieba : public SplitTool {
public:
	SplitToolCppJieba() {}
	virtual ~SplitToolCppJieba() {}
	//分词函数，虚函数 提供接口
	virtual vector<string> cut(const string &sentence) = 0;

	//网页JieBa
	virtual vector<string> cut(const string &sentence, int web) = 0;

	Configuration *_conf; //配置文件路径
};

//将文章中的每个单词切出来 将每个单词存入到DictProcuder类的vector<pair<string,int>> 中
class chinese_tool : public SplitToolCppJieba {
public:
	chinese_tool(string stop_words_name);
	~chinese_tool();

	virtual vector<string> cut(const string &sentence) override;

	//网页JieBa
	virtual vector<string> cut(const string &sentence, int web) override;

private:
	unique_ptr<unordered_set<string>> _stop_words; //用与存放中英文停用词
};

class english_tool : public SplitToolCppJieba {
public:
	english_tool();
	~english_tool();

	virtual vector<string> cut(const string &sentence) override;

	//网页JieBa
	virtual vector<string> cut(const string &sentence, int web) override;

private:
	unique_ptr<unordered_set<string>> _stop_words; //用与存放中英文停用词
};
#endif