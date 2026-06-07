#include "../../nlohmann/json.hpp" //nlohmann下的头文件 用于将发送数据序列化为json格式
#include "../../nlohmann/json_fwd.hpp"
//#include "../include/SplitTool.h" //处理客户端发来的msg 将其分割 不需要
#include "../../cppjieba/Jieba.hpp"

const char *const DICT_PATH        = "../../dict/jieba.dict.utf8";
const char *const HMM_PATH         = "../../dict/hmm_model.utf8";
const char *const USER_DICT_PATH   = "../../dict/user.dict.utf8";
const char *const IDF_PATH         = "../../dict/idf.utf8";
const char *const STOP_WORD_PATH   = "../../dict/stop_words.utf8";
const char *const CLEANED_WEB_PAGE = "../../webpageinfo/cleanedWebPage.txt"; // TODO:后续改用LoadFile打开，通过LoadFile返回

#include "LoadFile.h"
#include "WordsRecommend.h" //包含最小编辑距离的算法 用于选择推荐词
#include "EchoServer.h"
#include "TcpConnection.h"
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cmath>

using std::set;
using std::map;
using std::cout;
using std::endl;

MyTask::MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
{
}

// no using
static void load_stopwds(string str, unordered_set<string> &stop_words)
{
	std::ifstream file(str); // 替换为你的文件名
	if(!file.is_open()) {
		cerr << "无法打开文件" << __FILE__ << ":" << __LINE__
		     << "\n";
	}

	string line;
	while(getline(file, line)) { // 逐行读取文件，每行即为一个停用词
		// 假设每行只包含一个停用词，并且没有额外的空格或制表符
		// 如果存在这种情况，可以使用 istringstream 和 getline 来处理
		istringstream iss(line);
		string        word;
		//word = line.substr(0, line.find_first_of("\n") - 1);
		iss >> word;
		//word += '\0';
		//cout << word << "\n";

		stop_words.insert(word); // 将读取到的停用词插入到unordered_set中
	}
	stop_words.insert(" ");
	file.close(); // 关闭文件
}

//抄的chatgpt-----------------------------------------------------------------------------------下
static double culculateWight(int N, int TF, int DF)
{
	return TF * log2(N / (DF + 1));
}

// 函数用于计算两个向量的点积
double dotProduct(const std::vector<double> &v1, const std::vector<double> &v2)
{
	double result = 0.0;
	if(v1.size() != v2.size()) {
		throw std::invalid_argument("Vectors must have the same size.");
	}
	for(size_t i = 0; i < v1.size(); ++i) {
		result += v1[i] * v2[i];
	}
	return result;
}

// 函数用于计算向量的欧几里得长度（L2范数）
double vectorLength(const std::vector<double> &v)
{
	double sum = 0.0;
	for(double elem : v) {
		sum += elem * elem;
	}
	return sqrt(sum);
}

// 函数用于计算两个向量的余弦相似度
double cosineSimilarity(const std::vector<double> &v1, const std::vector<double> &v2)
{
	if(v1.empty() || v2.empty() || v1.size() != v2.size()) {
		return 0.0; // 或者可以选择抛出异常
	}
	double dot    = dotProduct(v1, v2);
	double normV1 = vectorLength(v1);
	double normV2 = vectorLength(v2);
	return dot / (normV1 * normV2);
}
//抄的chatgpt-----------------------------------------------------------------------------------上

//处理搜索任务
void MyTask::searchProcess()
{
#ifdef REDIS_ON
	WARN("redis on!!!");
	auto redis = Redis("tcp://127.0.0.1:6379");
	try {
		auto val = redis.get(_msg);
		if(val) {
			_con->sendInLoop(*val);
			INFO("redis target!!!");
			std::cout << "redis target!!!" << std::endl;
			return;
		}
		std::cout << "redis miss!!!" << std::endl;
		WARN("redis miss!!!");
	} catch(const sw::redis::ProtoError &e) {
		ERROR("redis get ProtoError for key '{}': {}", _msg, e.what());
	} catch(const sw::redis::Error &e) {
		ERROR("redis get Error for key '{}': {}", _msg, e.what());
	} catch(const std::exception &e) {
		ERROR("redis get exception for key '{}': {}", _msg, e.what());
	}
#endif

#ifdef LRUCACHE_ON
	WARN("lrucache on!!!");
	auto &cm = *CacheManager::getInstance();
	{
		auto val = cm.get(_con, _msg);
		if(val) {
			_con->sendInLoop(*val);
			INFO("lrucache target!!!");
			return;
		}
		INFO("lrucache miss!!!");
	}
#endif

	//TODO:
	//加载清洗后的离线网页库 用seekg(pos)+read(lenght)来获取网页
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: 静态 ifstream 被线程池多个线程共享，seekg/read 存在数据竞争
	// static ifstream web_page(CLEANED_WEB_PAGE, std::ios::binary);
	// FIX: 改为局部 ifstream（每次调用重新打开文件）保证线程安全
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	ifstream web_page(CLEANED_WEB_PAGE, std::ios::binary);
	if(!web_page.is_open()) {
		string file_name{"cleanedWebPage.txt"};
		ERROR("load flie:{} failed", file_name);
	}

	cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
	vector<string>  words;
	jieba.Cut(_msg, words, true);
	unordered_map<int, unordered_map<int, vector<double>>> id_weight; //对应word序列在int->id文章 中出现的 频率序列vector<double>

	//计算客户端发送的_msg的向量
	set<string>                set_words;
	unordered_map<string, int> msg_wd_frq;
	unordered_set<string>     *stop_words = LoadFile::getInstance()->_p_stop_words.get();
	int                        words_size = 0;

	//load_stopwds("../yuliao/webpage_stopwds.txt", stop_words);
	for(auto &word : words) {
		//清洗停用词
		if((*stop_words).find(word) != (*stop_words).end() || word == "\n") {
			//cout << "stop_words: " << word << "\n";
		} else { //不是停用词情况
			//如果是英文 且大写开头
			// if((word[0] & 0x80) == 0 && !ispunct(word[0]))
			// {
			// 	word[0] = tolower(word[0]);
			// }
			//cout << word << "\n";
			set_words.insert(word);
			msg_wd_frq[word]++;
			words_size++;
		}
	}

	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: 静态计数器被多线程共享用于调试日志（数据竞争，值无意义）
	// static uint32_t times = 1;
	// DEBUG("start calculating vectors of message times:{}", times);
	// times++;
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3

	int TF;        //_msg中某word出现的频率
	int DF = 1;    //中某word存在于多少文章中 就是把_msg作为文章 所以DF == 1
	int N  = 3423; //共多少篇文章 就是把_msg看成一篇文章

	vector<double> msg_vector; //_msg向量
	//遍历分词 .计算_msg向量
	for(auto &elem : set_words) {

		cout << "set words:---" << elem << "---"
		     << "\n";

		DEBUG("set words:---{}", elem);
		//过滤_msg中的停用词
		TF = msg_wd_frq[elem];
		msg_vector.emplace_back(culculateWight(3423, TF, 1)); //(N,TF,DF)
	}
	DEBUG("---set_words--");
	DEBUG("_msg words size:{}", msg_vector.size());
	for(auto &elem : msg_vector) {
		cout << "wt:" << elem << "\t";
		DEBUG("wt:{}", elem);
	}
	DEBUG("---set_words--");

	//遍历words 1.计算_msg向量  2.找出同时出现每个word的文章及各word的权重
	cout << set_words.size() << "\n";
	for(auto &elem : set_words) {

		cout << elem << ":\n";
		// for(auto &elemm : _invertIndexTable[elem]) //2024-9-22 03:48:36 停用
		for(auto &elemm : (*LoadFile::getInstance()->_p_invertIndexTable)[elem]) {
			int    id = elemm.first;
			double wt = elemm.second;
			cout << "id:" << id << "  wt:" << wt << "";
			DEBUG("id:{}, wt:{}", id, wt);
			// map<int, vector<double>> _cur;
			//  sleep(3);
			(id_weight[1])[id].emplace_back(wt); //----------------------这儿好像还是没正确
		}
		cout << "\n";
	}

	// DEBUG("(id_weight[1])[id].push_back(wt); //----------------------这儿好像还是没正确");
	int cut_size = set_words.size(); //_msg中不重复word的个数

	//抄的chatgpt-----------------(需要自己熟悉这种写法！！！！)---------------------------------------下 2024-9-21 23:06:11 停用
	auto compare = [](const pair<int, double> &a, const pair<int, double> &b) {
		return a.second < b.second;
	};

	// 创建一个priority_queue，使用自定义的比较函数
	priority_queue<pair<int, double>, vector<pair<int, double>>, decltype(compare)> pri_queue(compare);
	//抄的chatgpt-----------------------------------------------------------------------------------上

	//2024-9-21 23:06:27 新建
	map<double, int> pri_map;

	//计算各文章余弦值
	for(auto &elem : id_weight) //只会走一次
	{
		INFO("similarity culculating");
		for(auto &elemm : elem.second) {
			int    id         = elemm.first;
			size_t march_size = elemm.second.size();
			DEBUG("docid:{}  march_size:{}  cut_size:{}", id, march_size, cut_size);
			if(march_size == cut_size) // 如果当前文章的word个数 与_msg的word匹配成功
			{

				{ //计算每篇文章的向量
					double similarity = cosineSimilarity(msg_vector, elemm.second);
					cout << "_msg: ";
					for(auto &a : msg_vector) {
						cout << "wt:" << a << " ";
					}
					cout << "\n_page";
					for(auto &b : elemm.second) {
						cout << "wt:" << b << " ";
					}
					cout << '\n';
					cout << "docid:" << id << " " << similarity << "\n";
					pri_queue.push({id, similarity});
					pri_map.insert({similarity, id});
				}
			}
		}
	}

	if(pri_queue.empty()) {
		ERROR("nothing found in the data base of:{}", _msg);
		_con->sendInLoop(string("----nothing found!----"));
	} else {
		string msg;
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// BUG: 静态计数器被多线程共享（数据竞争）
		// static uint32_t times = 0;
		while(!pri_queue.empty()) // 2024-9-21 23:10:11 停用
		{
			// DEBUG("pop queue times:{}", ++times);
			// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
			cout << "docid:" << pri_queue.top().first << " similarity:" << pri_queue.top().second << "\n";

			int start  = (*LoadFile::getInstance()->_p_offset_map)[pri_queue.top().first].first;
			int offset = (*LoadFile::getInstance()->_p_offset_map)[pri_queue.top().first].second;
			cout << "start:" << start << " offset:" << offset << endl;
			web_page.seekg(start, std::ios::beg);
			char *buffer = new char[offset + 1];
			web_page.read(buffer, offset);
			buffer[offset] = '\0';
			std::cout << "\n\n\n-------------------------------------------------------------------------------------------\n";
			printf("buffer:%s \n", buffer); // 终端输出
			std::cout << "\n-------------------------------------------------------------------------------------------\n\n\n";
			// sleep(1);
			msg += string(buffer);
			msg += '\n';
			delete[] buffer;
			pri_queue.pop();
		}
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// times = 0;
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		WARN("start send artical");
#ifdef REDIS_ON
		try {
			bool ret = redis.set(_msg, msg);
			if(!ret) {
				ERROR("set key:{} in redis failed!", _msg);
			} else {
				INFO("set key:{} in redis successed!", _msg);
			}
		} catch(const sw::redis::ProtoError &e) {
			ERROR("redis set ProtoError for key '{}': {}", _msg, e.what());
		} catch(const sw::redis::Error &e) {
			ERROR("redis set Error for key '{}': {}", _msg, e.what());
		} catch(const std::exception &e) {
			ERROR("redis set exception for key '{}': {}", _msg, e.what());
		}
#endif
#ifdef LRUCACHE_ON
		cm.set(_con, _msg, msg);
#endif
		_con->sendInLoop(string(msg));
	}
}

//推荐词
void MyTask::recommendProcess()
{
	INFO("recommendProcess:{}", _msg);

	std::shared_ptr<vector<pair<string, int>>> _dict{LoadFile::getInstance()->_dict}; // 词典 TODO：改成static 25-6-24 22点33分 //修改为数据成员
	std::shared_ptr<map<string, set<int>>>     _index{LoadFile::getInstance()->_index};
	DEBUG("_index.size:{}", (*_index).size());

	// 解析_msg 根据：①最小编辑距离 ②热度 两个标准选取推荐词
	vector<string> vStr;
	vector<string> command;

	string tmp = _msg;
	int    len = _msg.length();
	vStr.clear();
	size_t i = 0;
	while(i != _msg.length()) {
		size_t next = nBytesCode(_msg[i]);
		vStr.emplace_back(_msg.substr(i, next));
		cout << *(vStr.end() - 1) + " ";
		i += next;
	}

	//_index中的set是vector中string 的下标，通过set中的int 就能找到vector中对应的string 及 string出现的frequency
	//用vStr去找_index中的set<int> 每次将每个vStr的char对应的set<int> 合并起来 这个set<int> 就是对应_msg在_dict中所以可以
	//出现char的string集合以及 此string的频率

	//最小编辑距离
	set<int> wd_pos;
	for(auto &c : vStr) {
		wd_pos.insert((*_index)[c].begin(), (*_index)[c].end());
	}

	map<string, int> condidate;

	for(auto &elem : wd_pos) {
		condidate.insert({(*_dict)[elem].first, (*_dict)[elem].second});
	}

	vector<string> recommedWds;
	seekRecommendWd(_msg, condidate, recommedWds);

	// 返回序列化候选词为json格式
	nlohmann::json jsonMsg;
	uint32_t       num = 0U;
	std::cout << "\n";
	for(auto &elem : recommedWds) {
		std::cout << "-----------------------------------推荐词" << num++ << ":" << elem << std::endl;
		jsonMsg.emplace_back(elem);
	}

	// 发送json到客户端
	{
		_con->sendInLoop(jsonMsg.dump());
	}
}

EchoServer::EchoServer(size_t threadNum, size_t queSize, const string &ip, unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
{
	/* start(); */
}

EchoServer::~EchoServer()
{
	this->stop();
}

//服务器的启动与停止
void EchoServer::start()
{
	INFO("start initing...");
	_pool.start();

#ifdef LRUCACHE_ON
	CacheManager::getInstance()->startSync();
#endif

	using namespace std::placeholders;
	//将所有的回调都写到这里来
	_server.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1), std::bind(&EchoServer::onMessage, this, _1), std::bind(&EchoServer::onClose, this, _1));

	_server.start();
}

void EchoServer::stop()
{
	WARN("start stop...!!!");
	_pool.stop();
#ifdef LRUCACHE_ON
	CacheManager::getInstance()->stopSync();
#endif
	_server.stop();
}

//三个回调
void EchoServer::onNewConnection(const TcpConnectionPtr &con)
{
	string str{con->toString()};
	cout << str << " has connected!" << endl;
	INFO("{} has connected!", str);
#ifdef LRUCACHE_ON
	CacheManager::getInstance()->onNewConnection(con);
#endif
}

//接收客户端msg，根据msg选择不同任务
void EchoServer::onMessage(const TcpConnectionPtr &con)
{
	string msg = con->receive();
	INFO("recv from client: {}", msg);
	string client_head = msg.substr(0, 1);
	string client_msg  = msg.substr(1, msg.length());

	cout << client_head << " ";
	cout << client_msg << "\n";
	MyTask task(client_msg, con);
	// 获取msg的第一个字符 如果是1 就执行推荐词recommendProcess
	if(client_head == "1") {
		//这儿的task是局部变量，在函数栈销毁后也会销毁，所以bind函数中传入的是本体，将本体拷贝构造一份给addTask，使得栈销毁后，不影响task的调度
		_pool.addTask(std::bind(&MyTask::recommendProcess, task));
	}
	//如果是0 就执行搜索代码
	else if(client_head == "0") {
		_pool.addTask(std::bind(&MyTask::searchProcess, task));
	}
}

void EchoServer::onClose(const TcpConnectionPtr &con)
{
	cout << con->toString() << " has closed!!!" << endl;
	ERROR(" {} has closed!!!", con->toString());
#ifdef LRUCACHE_ON
	CacheManager::getInstance()->onClose(con);
#endif
}
