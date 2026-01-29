#ifndef __WBCONFIGURATION__H
#define __WBCONFIGURATION__H

#include <string>
#include <map>
#include <set>

using namespace std;

class WbConfiguration {

public:
	WbConfiguration(const string &filepath);
	~WbConfiguration();

	//获取存放配置文件内容的map
	map<string, string> &getConfigMap();

	//获取停用词词集
	set<string> getStopWordList();

private:
	//配置文件路径
	string _filepath;
	//配置文件内容
	map<string, string> _configMap;
	//停用词
	set<string> _stopWordList;
};

#endif // __WBCONFIGURATION__H