#include "../include/WebPage.h"

bool operator==(const WebPage &page1, const WebPage &page2)
{
	return false;
}

bool operator<(const WebPage &page1, const WebPage &page2)
{
	return false;
}

WebPage::WebPage()
{
}

WebPage::WebPage(string doc, int id, string title, string link, string description)
    : _doc(doc)
    , _id(id)
    , _link(link)
    , _description(description)
{
	//处理description 提取出频率最高的20个词
	cout << id << "\n"
	     << title << "\n"
	     << link << "\n"
	     << description << "\n";
}

WebPage::WebPage(string &doc, Configuration &config)
{
}

WebPage::~WebPage()
{
}

//获取文档docid
int WebPage::getDocId()
{
	return _id;
}

//获取文档
string WebPage::getDoc()
{
	return _doc;
}

//对格式化文档进行处理
void WebPage::processDoc(const string &doc, Configuration &config)
{
}

//求取文档的topk 词集
void WebPage::calcTopK(vector<string> &wordsVec, int k, set<string> &stopWordList)
{
}
