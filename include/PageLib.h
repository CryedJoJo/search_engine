#ifndef __PAGELIB__H
#define __PAGELIB__H

#include "../include/DirScanner.h"
#include "../include/FileProcessor.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

class Configuration;

class PageLib {
public:
	PageLib(Configuration &conf, DirScanner &dirScanner, FileProcessor &fileProcessor); //
	~PageLib();
	//创建网页库
	void create();
	//存储网页库和位置偏移库
	void store();

private:
	//目录扫描对象的引用
	DirScanner _dirScanner;
	//执行
	FileProcessor _fileProcessor;
	//存放格式化之后的网页的容器
	vector<string> _pages;
	//存放每篇文档在网页库的位置信息
	map<int, pair<int, int>> _offsetLib;
};

#endif // __PAGELIB__H