#include "../include/WbConfiguration.h"

WbConfiguration::WbConfiguration(const string &filepath)
:_filepath(filepath)
{
}

WbConfiguration::~WbConfiguration()
{
}

set<string> WbConfiguration::getStopWordList()
{
	return set<string>();
}

map<string, string> &WbConfiguration::getConfigMap()
{
	
	// TODO: 在此处插入 return 语句
}
