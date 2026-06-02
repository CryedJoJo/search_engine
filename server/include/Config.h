#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <unordered_map>

class Config {
public:
	static Config *getInstance(const std::string &path = "../config/config.yaml");
	static void destroy();

	std::string get(const std::string &key) const;

private:
	Config(const std::string &path);
	~Config() = default;
	Config(const Config &rhs)                  = delete;
	Config &operator=(const Config &rhs)       = delete;

	bool loadFromFile(const std::string &path);
	std::string trim(const std::string &s) const;

	static Config *pointer;
	std::unordered_map<std::string, std::string> _data;
};

#endif
