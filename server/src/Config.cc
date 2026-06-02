#include "Config.h"
#include "log4cpp.h"

#include <fstream>

Config *Config::pointer = nullptr;

Config *Config::getInstance(const std::string &path)
{
	if(nullptr == pointer){
		pointer = new Config(path);
	}
	return pointer;
}

void Config::destroy()
{
	if(nullptr != pointer){
		delete pointer;
		pointer = nullptr;
	}
}

Config::Config(const std::string &path)
{
	if(!loadFromFile(path)){
		ERROR("config file '{}' failed to load", path);
	}
}

std::string Config::trim(const std::string &s) const
{
	size_t start = s.find_first_not_of(" \t\r\n");
	if(start == std::string::npos){
		return "";
	}
	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

bool Config::loadFromFile(const std::string &path)
{
	std::ifstream file(path);
	if(!file.is_open()){
		ERROR("open config file '{}' failed", path);
		return false;
	}

	std::string line;
	while(std::getline(file, line)){
		std::string trimmed = trim(line);
		if(trimmed.empty() || trimmed[0] == '#'){
			continue;
		}
		size_t pos = trimmed.find(':');
		if(pos == std::string::npos){
			continue;
		}
		std::string key   = trim(trimmed.substr(0, pos));
		std::string value = trim(trimmed.substr(pos + 1));

		size_t commentPos = value.find('#');
		if(commentPos != std::string::npos){
			value = trim(value.substr(0, commentPos));
		}

		if(!key.empty()){
			_data[key] = value;
		}
	}

	file.close();
	return true;
}

std::string Config::get(const std::string &key) const
{
	auto it = _data.find(key);
	if(it != _data.end()){
		return it->second;
	}
	WARN("config key '{}' not found", key);
	return "";
}
