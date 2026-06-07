#include "log4cpp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <unistd.h>
#include <memory>

using namespace std;

static const string CLEANED_PAGE_OFFSET{"../../webpageinfo/cleanedPageOffset.txt"};
static const string NORMAL_REVERSE_INDEX{"../../webpageinfo/NormalReverseIndex.txt"};
static const string WEBPAGE_STOP_WORDS{"../../yuliao/webpage_stopwds.txt"};
// static const string CLEANED_WEB_PAGE{"../../webpageinfo/cleanedWebPage.txt"}; // TODO:后续加入
static const string DICTIONARY{"../../data/dictionary/_dictionary.dat"};
static const string DICT_INDEX{"../../data/dictIndex.dat"};

class LoadFile {
public:
	static LoadFile *getInstance()
	{
		if(nullptr == pointer) {
			atexit(destroy);
			pointer = new LoadFile();
		}
		return pointer;
	}

	static void destroy()
	{
		WARN("start destroy load file...");
		if(nullptr == pointer) {
			return;
		}
		delete pointer;
		pointer = nullptr;
	}

private:
	static bool init()
	{
		try {
			string line;
			string word;
			{
				// ifstream page_offset("../../webpageinfo/cleanedPageOffset.txt");
				ifstream page_offset(CLEANED_PAGE_OFFSET);
				if(!page_offset.is_open()) {
					string fileName{"cleanedPageOffset.txt"};
					ERROR("load file:{} failed!!!", fileName);
					return false;
				}
				//读入容器中
				_p_offset_map = std::make_shared<std::unordered_map<int, std::pair<int, int>>>();
				int id, start, end, offset;
				int time = 10;
				while(getline(page_offset, line)) {
					istringstream iss(line);
					iss >> id >> start >> end;
					cout << "id:" << id << "  start:" << start << "  end:" << end << "\n";
					DEBUG("id:{}, start:{}, end:{}", id, start, end);
					offset               = end - start;
					(*_p_offset_map)[id] = {start, offset};
				}
				page_offset.close();
			}

			{
				//加载归一化倒排索引
				ifstream normal_reverse_index(NORMAL_REVERSE_INDEX);
				if(!normal_reverse_index.is_open()) {
					string fileName{"NormalReverseIndex.txt"};
					ERROR("load flie: {} failed!!!", fileName);
					return false;
				}
				//倒排索引表对象 string->单词  pair:int->文章id, double->权值
				_p_invertIndexTable = std::make_shared<std::unordered_map<std::string, std::vector<std::pair<int, double>>>>();
				while(getline(normal_reverse_index, line)) {
					istringstream iss(line);
					string        word;
					int           id;
					double        wt;
					iss >> word;
					while(iss >> id >> wt) {
						(*_p_invertIndexTable)[word].push_back({id, wt});
					}
				}
				normal_reverse_index.close();
			}

			{
				_p_stop_words = std::make_shared<std::unordered_set<std::string>>();
				std::ifstream webpage_stop_words(WEBPAGE_STOP_WORDS); // 加载停用词
				if(!webpage_stop_words.is_open()) {
					string fileName{"webpage_stopwds.txt"};
					ERROR("load flie: {} failed!!!", fileName);
					return false;
				}
				while(getline(webpage_stop_words, line)) { // 逐行读取文件，每行即为一个停用词
					// 假设每行只包含一个停用词，并且没有额外的空格或制表符
					// 如果存在这种情况，可以使用 istringstream 和 getline 来处理
					istringstream iss(line);
					string        word;
					//word = line.substr(0, line.find_first_of("\n") - 1);
					iss >> word;
					//word += '\0';
					std::cout << word << "\n";

					(*_p_stop_words).insert(word); // 将读取到的停用词插入到unordered_set中
					                               // (*(_p_stop_words.get())).insert(word); // 不推荐这样get之后用裸指针， 直接用shared_ptr重载的* 运算符，也是对象本身
				}
				(*_p_stop_words).insert(" ");
				webpage_stop_words.close(); // 关闭文件
			}

			{
				_dict = make_shared<vector<pair<string, int>>>();
				ifstream dictionary(DICTIONARY);
				if(!dictionary.is_open()) {
					string fileName{"_dictionary.dat"};
					ERROR("load flie: {} failed!!!", fileName);
					return false;
				}
				line.clear();

				string freq;
				// 2.存入 vector vector<pair<string, int>> _dict;
				while(getline(dictionary, line)) {
					istringstream iss(line);
					iss >> word;
					iss >> freq;
					_dict->emplace_back(pair<string, int>(word, stoi(freq)));
				}
				dictionary.close();
			}

			{
				// 3.遍历 ../data/dictIndex.dat 构建索引 用map<stirng,set<int>>映射 单个汉字或字符 对应在vector中出现的位置
				ifstream dict_index(DICT_INDEX);
				if(!dict_index.is_open()) {
					string fileName{"dictIndex.dat"};
					ERROR("load flie:{} failed!!!", fileName);
					return false;
				}
				//读取数据到_index;
				_index = make_shared<map<string, set<int>>>();
				while(getline(dict_index, line)) {
					istringstream iss(line);
					iss >> word;
					(*_index)[word];
					string set_int;
					int    first = 1;
					while(iss >> set_int) {
						int pos = stoi(set_int);
						if(first) {
							set<int> new_char;
							new_char.insert(pos);
							(*_index)[word] = new_char;
							first           = 0;
						} else {
							//向set<int>中插入当前elem在vecto中的位置
							(*_index)[word].insert(pos);
						}
					}
				}
				dict_index.close();
			}
			INFO("LoadFile success!");

		} catch(const exception &e) {
			ERROR("exception {} occur in LoadFile!!!", e.what());
			return false;
		} catch(...) {
			ERROR("unknow exception occur in LoadFile!!!");
			return false;
		}
		return true;
	}

	LoadFile()
	{
		if(init()) {

		} else {
		}
	}

	~LoadFile()                              = default;
	LoadFile(LoadFile &rhs)                  = delete;
	LoadFile &operator=(const LoadFile &rhs) = delete;

private:
	static LoadFile *pointer;

public:
	// 搜索功能
	static std::shared_ptr<vector<pair<string, int>>>                        _p_dict;             //词典
	static std::shared_ptr<unordered_map<int, pair<int, int>>>               _p_offset_map;       //网页偏移
	static std::shared_ptr<unordered_map<string, vector<pair<int, double>>>> _p_invertIndexTable; //倒排索引
	static std::shared_ptr<unordered_set<string>>                            _p_stop_words;       //停用词
	// 推荐词功能
	static std::shared_ptr<vector<pair<string, int>>> _dict; // 词典 TODO：改成static 25-6-24 22点33分 //修改为数据成员
	static std::shared_ptr<map<string, set<int>>>     _index;
};
