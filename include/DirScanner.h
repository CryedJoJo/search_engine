#ifndef __DIRSCANNER__H
#define __DIRSCANNER__H

#include <vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fstream>
#include "../server/log4cpp.h"
using namespace std;

//递归扫描
class DirScanner {

public:
	DirScanner();

	~DirScanner();
	//重载函数调用运算符，调用traverse 函数
	void operator()();
	//返回_vecFilesfiles 的引用
	vector<string> &files();
	//获取某一目录下的所有文件
	void traverse(const string &dirName);

private:
	//存放每个语料文件的绝对路
	vector<string> _xmls;
};

#endif // __DIRSCANNER__H