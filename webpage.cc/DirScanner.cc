#include "../include/DirScanner.h"

DirScanner::DirScanner()
{
}

DirScanner::~DirScanner()
{
}

void DirScanner::operator()()
{
}

void DirScanner::traverse(const string &dirName)
{
	Mylogger      *log = Mylogger::getInstance();
	DIR	       *d;
	struct dirent *dir;

	d = opendir(dirName.c_str()); // 打开指定目录
	if(d)
	{
		while((dir = readdir(d)) != NULL)
		{
			log->info("info", __FILE__, __FUNCTION__, __LINE__);
			// 检查是否是 "." 或 ".."
			if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			{
				continue; // 跳过这两个特殊目录
			}

			// 否则，将文件路径添加到 _files 中
			_xmls.push_back(dirName + dir->d_name);
			// 注意：这里添加了 "/" 以确保路径的正确性，特别是在 _filedirName 不以 "/" 结尾时
		}

		closedir(d);
	}
	else
	{
		// 处理无法打开目录的情况
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}
}

vector<string> &DirScanner::files()
{
	return _xmls;
}
