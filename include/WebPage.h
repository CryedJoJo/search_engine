#ifndef __WEBPAGE__H
#define __WEBPAGE__H

#include "SplitTool.h"

#include <string>
#include <vector>
#include <map>

using namespace std;
class Configuration;

class WebPage {
public:
	//判断两篇文档是否相等
	friend bool operator==(const WebPage &lhs, const WebPage &rhs);

	//对文档按Docid 进行排序
	friend bool operator<(const WebPage &lhs, const WebPage &rhs);

	WebPage();
	WebPage(string doc, int id, string docTitle, string docUrl, string docContent);
	WebPage(string &doc, Configuration &configl);
	~WebPage();

	//获取文档docid
	int getDocId();

	//获取文档
	string getDoc();

	//对格式化文档进行处理
	void processDoc(const string &doc, Configuration &config);

	//求取文档的topk 词集
	void calcTopK(vector<string> &wordsVec, int k, set<string> &stopWordList);

private:
	const static int TOPK_NUMBER = 20;

	//整篇文档 包含xml在内
	string _doc;

	//文档id
	int _id;

	//文档标题
	string _docTitle;

	//文档URL
	string _link;

	//文档内容
	string _description;

	//词频最高额20个词
	vector<string> _topwds;

	//保存每篇文档的所有词语和词频，不包括停用词
	map<string, int> _wordsMap;

	//SplitTool *_splitTool;
};

#endif // __WEBPAGE__H