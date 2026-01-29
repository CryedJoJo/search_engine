#include "../include/SplitTool.h"
#include "../server/log4cpp.h"
#include "../include/tinyxml2.h"
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <fstream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ofstream;

using namespace tinyxml2;

struct RSSIteam {
	string _title;
	string _link;
	string _description;
	string _content;
};

class RSS {
public:
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程

	RSS(size_t capa)
	{
		_rss.reserve(capa);
	}

	void getFiles(string path)
	{
		//log->info("info", __FILE__, __FUNCTION__, __LINE__);

		DIR           *d;
		struct dirent *dir;

		d = opendir(path.c_str()); // 打开指定目录
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
				_xmls.push_back(path + dir->d_name);
				// 注意：这里添加了 "/" 以确保路径的正确性，特别是在 _filepath 不以 "/" 结尾时
			}
			closedir(d);
		}
		else
		{
			// 处理无法打开目录的情况
			log->error("error", __FILE__, __FUNCTION__, __LINE__);
		}
	}
	//读文件
	void read(const string &filename)
	{
		//log->info("info", __FILE__, __FUNCTION__, __LINE__);
		XMLDocument doc;

		//TODO:这儿用一个
		vector<string> htmlPages; //存储需要洗的网页 将网页路径全部读入htmlPages中

		//遍历网页路径
		getFiles("/home/pigsy/search_engine/webres/");
		//取出一份网页进行清洗
		int xml_num = 1;
		int size    = -3;
		for(auto &elem : _xmls)
		{
			doc.LoadFile(elem.c_str());
			if(doc.ErrorID())
			{
				std::cerr << "load xml page " << elem << ":" << xml_num << "fail\n";
				xml_num++;
				continue;
			}

			size_t idx = 0;

			SplitTool  *splitTool = new chinese_tool("webpage_stopwds.txt");
			XMLElement *itemNode  = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");

			while(itemNode)
			{
				string title = itemNode->FirstChildElement("title")->GetText();
				if(nullptr == itemNode->FirstChildElement("title"))
				{
					log->warn("warn:title empty", __FILE__, __FUNCTION__, __LINE__);
					itemNode = itemNode->NextSiblingElement("item");
					continue;
				}
				string link = itemNode->FirstChildElement("link")->GetText();

				if(nullptr == itemNode->FirstChildElement("link"))
				{
					log->warn("warn:link empty", __FILE__, __FUNCTION__, __LINE__);
					itemNode = itemNode->NextSiblingElement("item");
					continue;
				}

				string description;
				string content;
				int    use_content = 0;
				if(nullptr != itemNode->FirstChildElement("description"))
					description = itemNode->FirstChildElement("description")->GetText(); //这儿需要处理content 将'\n'去掉
				else
				{
					log->warn("warn:description empty", __FILE__, __FUNCTION__, __LINE__);
					if(nullptr != itemNode->FirstChildElement("content"))
					{
						description = itemNode->FirstChildElement("content")->GetText();
						use_content = 1;
					}
					else
					{
						log->warn("warn:content empty", __FILE__, __FUNCTION__, __LINE__);
						itemNode = itemNode->NextSiblingElement("item");
						continue;
					}
				}

				description.erase(remove_if(description.begin(), description.end(), [=](char c) { return c == '\n'; }), description.end()); //洗掉换行

				std::regex reg("<[^>]+>"); //通用正则表达式
				description = regex_replace(description, reg, "");
				//description = splitTool->cut(regex_replace(description, reg, ""), 1);

				RSSIteam rssItem;

				rssItem._title = title;
				rssItem._link  = link;

				rssItem._description = description;
				cout << description << "\n";

				_rss.push_back(rssItem);
				++idx;

				itemNode = itemNode->NextSiblingElement("item");
			}
			xml_num++;
			// if(size++)
			//break;
		}
	}
	//写文件
	void store(const string &filename)
	{
		ofstream ofs(filename);
		if(!ofs)
		{
			std::cerr << "open " << filename << " fail!" << endl;
			return;
		}

		for(size_t idx = 0; idx != _rss.size(); ++idx)
		{
			ofs << "<doc>\n\t<docid>" << idx + 1
			    << "</docid>\n\t<title>" << _rss[idx]._title
			    << "</title>\n\t<link>" << _rss[idx]._link
			    << "</link>\n\t<description>" << _rss[idx]._description
			    << "</description>\n</doc>";
			ofs << '\n';
		}

		ofs.close();
	}

private:
	vector<RSSIteam> _rss;
	vector<string>   _xmls;
};

void test0()
{
	RSS rss(4000);
	rss.read("coolshell.xml");
	rss.store("./page2.txt");
}

int main(void)
{
	test0();
	return 0;
}
