#include "../include/DictProducer.h"
#include "../include/SplitTool.h" //这儿包含SplitTool.h 而不是在头文件中去包含SplitTool.h
#include "../include/Configuration.h"

#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype> // 引入tolower和ispunct 清洗english

DictProducer::DictProducer(const string &dir)
    : _conf(dir)
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
}

//构造函数 专门处理中文
DictProducer::DictProducer(const string &dir, SplitTool *splitTool)
    : _conf(dir)
    , _cuttor(splitTool)
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
}

DictProducer::~DictProducer()
{
}

//清洗指定目录下的中文文件，将\n清除
string DictProducer::cleanCnTxt(string filepath)
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	// 打开源文件
	ifstream infile(filepath);
	if(!infile.is_open())
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}

	stringstream buffer;
	buffer << infile.rdbuf(); // 读取文件内容到buffer
	infile.close();           // 关闭文件

	// 去除换行符
	string content = buffer.str();
	size_t pos     = 0;
	while((pos = content.find("\n")) != string::npos)
	{
		content.erase(pos, 1); // 假设使用'\n'作为换行符，对于Windows可能需要检查'\r\n'
	}

	// 输出或保存处理后的内容
	//  cout << "处理后的内容：" <<  endl
	//           << content <<  endl;

	// // 保存到新文件
	//  ofstream outfile("./chinese_text_no_newlines.txt");
	// if(outfile.is_open())
	// {
	// 	outfile << content;
	// 	outfile.close();
	// }
	// else
	// {
	// 	 cerr << "无法打开文件以保存内容！" <<  endl;
	// }

	return content;
}

//将英文文件中的大写字母换成小写
string DictProducer::cleanEnTxt(string filepath)
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	string   content;
	bool     inWord = false; // 标记是否正在处理单词
	ifstream infile(filepath);
	if(!infile.is_open())
	{
		cerr << "无法打开文件！" << endl;
	}

	stringstream buffer;
	buffer << infile.rdbuf(); // 读取文件内容到buffer
	infile.close();           // 关闭文件
	string str = buffer.str();

	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	for(string::iterator it = str.begin(); it != str.end(); ++it)
	{
		// 如果字符不是标点符号且不是空格或换行
		if(!ispunct(*it) && !(*it == ' ' || *it == '\n'))
		{

			content += tolower(*it); // 转换为小写并添加到结果字符串
			inWord = true;           // 标记为在单词中
		}
		else if(inWord && (*it == ' ' || *it == '\n'))
		{

			// 如果之前在处理单词，并且现在遇到空格或换行，则保留空格或换行
			content += *it;
			inWord = false; // 标记为不在单词中
		}
		// 如果遇到标点符号，则用空格替代
		else
		{
			content += " ";
			inWord = false; //标记为不在单词中
		}
	}

	return content;
}

//创建英文字典
void DictProducer::buildEnDict()
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	//遍历没篇文章
	int i = 0;

	for(auto &elem : _files)
	{
		cout << "file:" << elem << "\n";

		log->info("info", __FILE__, __FUNCTION__, __LINE__);
		i++; //控制for循环只读三篇文章

		//用JieBa提取出中文单词
		string         tmp   = cleanEnTxt(elem);
		vector<string> words = _cuttor->cut(tmp);
		log->info("info", __FILE__, __FUNCTION__, __LINE__);

		int      pos = 0;
		set<int> wd_pos;
		ofstream outfile("../data/words/_words.txt", ios::app);
		if(outfile.is_open())
		{
			for(auto &elem : words)
			{
				//如果字符串的起始字符是数字字符 就跳过这个字符串
				if(elem.c_str()[0] >= 48 && elem.c_str()[0] <= 57)
				{
					continue;
				}

				_dict.push_back(make_pair(elem, pos));

				//如果对应string中的set为空，会在if语句自动构建一个对应的elem的key
				if(_index[elem].empty())
				{
					set<int> new_word;
					new_word.insert(pos);
					_index[elem] = new_word;
				}
				else
				{
					//向set<int>中插入pos数据
					_index[elem].insert(pos);
				}
				//将字符串写入文件
				outfile << elem << '\n';
				pos++;
			}
			if(i == 1)
				break;

			outfile.close();
		}
		else
		{
			log->error("error", __FILE__, __FUNCTION__, __LINE__);
		}
	}
}

//创建中文字典
void DictProducer::buildCnDict()
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	//遍历每篇文章
	int i = 0;
	for(auto &elem : _files)
	{
		//cleanCnTxt("../yuliao/C3-Art0019.txt");

		//i++; //控制for循环只读三篇文章

		//用JieBa提取出中文单词
		vector<string> words = _cuttor->cut(cleanCnTxt(elem));

		int      pos = 0;
		set<int> wd_pos;
		ofstream outfile("../data/words/_words.txt", ios::app);
		if(outfile.is_open())
		{

			for(auto &elem : words)
			{
				//跳过数字字符开头的串
				if(elem.c_str()[0] >= 48 && elem.c_str()[0] <= 57)
				{
					continue;
				}
				_dict.push_back(make_pair(elem, pos));

				//如果对应string中的set为空，会在if语句自动构建一个对应的elem的key
				if(_index[elem].empty())
				{
					set<int> new_word;
					new_word.insert(pos);
					_index[elem] = new_word;
				}
				else
				{
					//向set<int>中插入pos数据
					_index[elem].insert(pos);
				}
				//将字符串写入文件
				outfile << elem << '\n';
				pos++;
			}

			outfile.close();
		}
		else
		{
			log->error("error", __FILE__, __FUNCTION__, __LINE__);
		}

		if(i == 1)
			break;
	}
}

static void departStr(string &str, map<string, set<int>> &index)
{
}

//判断下一个单词多少字节
static size_t nBytesCode(const char ch)
{
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}
//构建索引
void DictProducer::createIndex()
{
	//读取中英文词典 用vector<pair<string,int>> 来存储每个串及其位置
	// 1. 打开词典
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	ifstream _ifs("../data/dictionary/_dictionary.dat");
	if(!_ifs.is_open())
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}

	string line;
	string word;
	int    cn_pos = 0;

	// 2.存入 vector vector<pair<string, int>> _dict;
	while(getline(_ifs, line))
	{
		istringstream iss(line);
		iss >> word;
		_dict.push_back(pair(word, cn_pos));
		cn_pos++;
		while(iss >> word)
			;
	}

	log->info("info", __FILE__, __FUNCTION__, __LINE__);

	// 3.遍历中文词典 构建索引
	for(auto elem : _dict)
	{
		pushDict(elem); //构建当前elem.first单词的索引
	}

	// //1.
	// ifstream en_ifs("../data/words/en_words.txt");
	// if(!en_ifs.is_open())
	// {
	// 	log->error("error", __FILE__, __FUNCTION__, __LINE__);
	// }

	// // 2.
	// _dict.clear();
	// int en_pos = 0;
	// while(getline(en_ifs, line))
	// {
	// 	istringstream iss(line);
	// 	iss >> word;
	// 	_dict.push_back(pair(word, en_pos));
	// 	en_pos++;
	// }

	// 3.遍历英文词典 构建索引 用map<stirng,set<int>>映射 单个汉字或字符 对应在vector中出现的位置，
	// for(auto elem : _dict)
	// {
	// 	pushDict(elem); //构建当前elem.first单词的索引
	// }
	log->info("info", __FILE__, __FUNCTION__, __LINE__);

	// 4.输出 map<stirng,set<int> _index; 到索引文件
	ofstream outfile("../data/dictIndex.dat");
	if(outfile.is_open())
	{
		log->info("info", __FILE__, __FUNCTION__, __LINE__);
		for(auto &elem : _index)
		{
			outfile << elem.first << ' ';
			for(auto pos : elem.second)
			{
				outfile << pos << ' ';
			}
			outfile << '\n';
		}
		outfile.close();
	}
	else
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}
}

//将词典写入文件
void DictProducer::storeDict(const char *filepath)
{
	//将words容器中的单词全部取出 保存到../data/dictionary.txt中
	ofstream outfile(filepath, ios::app);
	if(outfile.is_open())
	{

		for(auto &elem : _index)
		{
			//跳过数字打头的字符串
			// if(*elem.first.begin() >= 48 && *elem.first.begin() <= 57)
			// {
			// 	cout << elem.first << endl;
			// 	continue;
			// }
			outfile << elem.first << "  " << elem.second.size() << "\n";
		}
		outfile.close();
	}
	else
	{
		log->error("error", __FILE__, __FUNCTION__, __LINE__);
	}
}

//获取文件绝对路径
void DictProducer::getFiles()
{
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	DIR	       *d;
	struct dirent *dir;
	_files.clear(); // 可能保存了其他语料的路径，先清空路径

	d = opendir(_conf._filepath.c_str()); // 打开指定目录
	if(d)
	{
		while((dir = readdir(d)) != NULL)
		{
			// 检查是否是 "." 或 ".."
			if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
			{
				continue; // 跳过这两个特殊目录
			}

			// 否则，将文件路径添加到 _files 中
			_files.push_back(_conf._filepath + dir->d_name);
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

//存储某个单词
void DictProducer::pushDict(pair<string, int> elem)
{
	//遍历串每个字符 构建索引
	for(size_t i = 0; i < elem.first.size();)
	{
		if(0x80 & elem.first[i] == 0) //字母
		{
			string s = elem.first.substr(i, 1);
			if(_index[s].empty())
			{
				set<int> new_char;
				new_char.insert(elem.second);

				_index[s] = new_char;
			}
			else
			{
				//向set<int>中插入当前elem在vector中的位置
				_index[s].insert(elem.second);
			}
			i++;
		}
		else //汉字
		{
			size_t cn_len = nBytesCode(elem.first[0]);
			string s      = elem.first.substr(i, cn_len);
			if(_index[s].empty())
			{
				set<int> new_char;
				new_char.insert(elem.second);

				_index[s] = new_char;
			}
			else
			{
				//向set<int>中插入当前elem在vecto中的位置
				_index[s].insert(elem.second);
			}
			i += cn_len;
		}
	}
}

void createCnDict()
{
	Configuration conf("../yuliao/art/");
	SplitTool    *splitTool1 = new chinese_tool("stop_words_zh.txt");
	DictProducer  cn_d(conf._filepath, splitTool1);
	cn_d.getFiles();
	cn_d.buildCnDict();
	cn_d.storeDict("../data/dictionary/_dictionary.dat");
}

void createEnDict()
{
	SplitTool   *splitTool2 = new english_tool();
	DictProducer en_d("../yuliao/english/", splitTool2);
	en_d.log->info("info", __FILE__, __FUNCTION__, __LINE__);
	en_d.getFiles();
	en_d.buildEnDict();
	en_d.storeDict("../data/dictionary/_dictionary.dat");
}

void createWdIndex()
{
	DictProducer index_d("");
	index_d.createIndex();
}

int main()
{
	//createCnDict();
	//createEnDict();
	createWdIndex();
}