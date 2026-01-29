#ifndef __FILEPROCESSOR__H
#define __FILEPROCESSOR__H

#include <string>
using namespace std;

class FileProcessor {
public:
	FileProcessor();
	~FileProcessor();

	//处理对应文件 将其清洗
	string process(string fileName);

private:
	string _titleFeature;
};
#endif // __FILEPROCESSOR__H