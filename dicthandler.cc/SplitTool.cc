#include "../include/SplitTool.h"
#include "../server/log4cpp.h"

chinese_tool::chinese_tool(string stop_words_name)
    : _stop_words(new unordered_set<string>)
{
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	log->info("info", __FILE__, __FUNCTION__, __LINE__);
	//这儿加载停用词的语料 存入unorder_set中
	_conf = new Configuration("../yuliao/");
	//初始化停用词容器
	//string         en_stop_wds = _conf->_filepath + "stop_words_eng.txt";
	string cn_stop_wds = _conf->_filepath + stop_words_name;
	vector<string>
	    files;
	files.push_back(cn_stop_wds);
	//files.push_back(en_stop_wds);

	//unordered_set<std::string> stopwords;
	for(auto &elem : files)
	{
		std::ifstream file(elem); // 替换为你的文件名
		cout << elem << "\n";
		if(!file.is_open())
		{
			cerr << "无法打开文件" << __FILE__ << ":" << __LINE__ 
			     << "\n";
		}

		string line;
		while(getline(file, line))
		{ // 逐行读取文件，每行即为一个停用词
			// 假设每行只包含一个停用词，并且没有额外的空格或制表符
			// 如果存在这种情况，可以使用 istringstream 和 getline 来处理
			istringstream iss(line);
			string        word;
			//word = line.substr(0, line.find_first_of("\n") - 1);
			iss >> word;
			//word += '\0';
			//cout << word << "\n";

			_stop_words->insert(word); // 将读取到的停用词插入到unordered_set中
		}
		_stop_words->insert(" ");
		file.close(); // 关闭文件
	}

	//输出unordered_set中的所有停用词来验证
	// for(const auto &word : *_stop_words)
	// {
	// 	cout << word << "\n";
	// }
}

chinese_tool::~chinese_tool()
{
}

vector<string> chinese_tool::cut(const string &sentence)
{
	Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	cout << "splitTool.cc: cut();" << endl;
	log->info("info 6", __FILE__, __FUNCTION__, __LINE__);
	// 创建了一个jieba对象
	// jieba对象只构建一次，每次用的时候传指针
	cppjieba::Jieba jieba(DICT_PATH,
	                      HMM_PATH,
	                      USER_DICT_PATH,
	                      IDF_PATH,
	                      STOP_WORD_PATH);
	vector<string>  words;
	vector<string>  re_words;
	jieba.Cut(sentence, words, true);

	// auto it = _stop_words->find("那");
	// if(it != nullptr)
	// 	cout << "stop:" << *it << "\n";
	log->info("info 6", __FILE__, __FUNCTION__, __LINE__);
	for(auto &word : words)
	{
		//清洗停用词
		if(_stop_words->find(word) != _stop_words->end())
		{

			//cout << "stop_words: " << word << "\n";
		}
		else
		{ //不是停用词情况
			//如果是英文 且大写开头
			if((word[0] & 0x80) == 0 && !ispunct(word[0]))
			{
				word[0] = tolower(word[0]);
			}
			//cout << word << "\n";
			re_words.push_back(word);
		}
	}
	return re_words;
}

vector<string> chinese_tool::cut(const string &sentence, int web)
{
	//Mylogger *log = Mylogger::getInstance(); //日志单例 记录任务执行过程
	cout << "splitTool.cc: cut();" << endl;
	//log->info("info 6", __FILE__, __FUNCTION__, __LINE__);
	// 创建了一个jieba对象
	// jieba对象只构建一次，每次用的时候传指针
	cppjieba::Jieba jieba(DICT_PATH,
	                      HMM_PATH,
	                      USER_DICT_PATH,
	                      IDF_PATH,
	                      STOP_WORD_PATH);
	vector<string>  words;
	vector<string>  re_words;
	jieba.Cut(sentence, words, true);

	// auto it = _stop_words->find("那");
	// if(it != nullptr)
	// 	cout << "stop:" << *it << "\n";
	///log->info("info 6", __FILE__, __FUNCTION__, __LINE__);
	for(auto &word : words)
	{
		//清洗停用词
		if(_stop_words->find(word) != _stop_words->end())
		{

			//cout << "stop_words: " << word << "\n";
		}
		else
		{ //不是停用词情况
			//如果是英文 且大写开头
			// if((word[0] & 0x80) == 0 && !ispunct(word[0]))
			// {
			// 	word[0] = tolower(word[0]);
			// }
			//cout << word << "\n";
			re_words.push_back(word);
		}
	}
	return re_words;
}

english_tool::english_tool()
    : _stop_words(new unordered_set<string>)
{
	//这儿加载停用词的语料 存入unorder_set中
	_conf = new Configuration("../yuliao/");
	//初始化停用词容器
	string en_stop_wds = _conf->_filepath + "stop_words_eng.txt";
	//string         cn_stop_wds = _conf->_filepath + "stop_words_zh.txt";
	vector<string> files;
	//files.push_back(cn_stop_wds);
	files.push_back(en_stop_wds);

	//unordered_set<std::string> stopwords;
	for(auto &elem : files)
	{
		std::ifstream file(elem); // 替换为你的文件名
		cout << elem << "\n";
		if(!file.is_open())
		{
			cerr << "无法打开文件"<< __FILE__ << ":" << __LINE__ 
			     << "\n";
		}

		string line;
		while(getline(file, line))
		{ // 逐行读取文件，每行即为一个停用词
			// 假设每行只包含一个停用词，并且没有额外的空格或制表符
			// 如果存在这种情况，可以使用 std::istringstream 和 std::getline 来处理
			istringstream iss(line);
			string        word;
			//word = line.substr(0, line.find_first_of("\n") - 1);
			iss >> word;
			//word += '\0';
			//cout << word << "\n";

			_stop_words->insert(word); // 将读取到的停用词插入到unordered_set中
		}
		_stop_words->insert(" ");
		file.close(); // 关闭文件
	}

	//输出unordered_set中的所有停用词来验证
	// for(const auto &word : *_stop_words)
	// {
	// 	cout << word << "\n";
	// }
}

english_tool::~english_tool()
{
}

vector<string> english_tool::cut(const string &sentence)
{
	cout << "en_cut" << endl;
	string         word;
	vector<string> re_words;
	istringstream  iss(sentence);
	while(iss >> word)
	{
		//清洗停用词
		if(_stop_words->find(word) != _stop_words->end())
		{
			//cout << "stop_words: " << word << "\n";
		}
		else
		{
			//不是停用词情况
			re_words.push_back(word);
		}
	}
	return re_words;
}

vector<string> english_tool::cut(const string &sentence, int web)
{
	cout << "en_cut" << endl;
	string         word;
	vector<string> re_words;
	istringstream  iss(sentence);
	while(iss >> word)
	{
		//清洗停用词
		if(_stop_words->find(word) != _stop_words->end())
		{
			//cout << "stop_words: " << word << "\n";
		}
		else
		{
			//不是停用词情况
			re_words.push_back(word);
		}
	}
	return re_words;
}
