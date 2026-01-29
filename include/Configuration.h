#ifndef __CONFIGURATION__H
#define __CONFIGURATION__H
//读取文件目录名

#include <iostream>
#include <dirent.h>
#include <vector>
#include <string>

using namespace std;

class Configuration {
public:
	Configuration(string dir)
	    : _myconfigPath(dir)
	    , _filepath(dir)
	    , _data("../data/")
	{
	}

	//void readConfig(const string &filename);

public:
	string _myconfigPath;
	string _filepath; //存放路径
	string _data;
	string _ip;
	int    _port;
};

// void Configuration::readConfig(const string &filename)
// {
// }

#endif