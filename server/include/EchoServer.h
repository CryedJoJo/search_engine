#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "log4cpp.h"

#include "ThreadPool.h"
#include "TcpServer.h"
#include "CacheManager.h"
#include <sw/redis++/redis++.h>

using namespace sw::redis;

class MyTask {
public:
	MyTask(const string &msg, const TcpConnectionPtr &con);
	void searchProcess();
	void recommendProcess();

private:
	string           _msg;
	TcpConnectionPtr _con;
	bool             _dictInitFlag{false};
	// bind函数绑定MyTask时 绑定的类必须时可赋值的，unique指针是不可复制的！！！
	// std::unique_ptr<vector<pair<string, int>>> _dict{nullptr}; // 词典 TODO：改成static 25-6-24 22点33分 //修改为数据成员
	// std::unique_ptr<map<string, set<int>>>     _index {nullptr};

	//TODO:不行，不同的线程还是会去创建不同的MyTask,每次请求的线程不同 也会重新去创建下面的数据 需要用单例！！！！ 2025-07-02 03:03:43
	// std::shared_ptr<vector<pair<string, int>>> _dict {nullptr}; // 词典 TODO：改成static 25-6-24 22点33分 //修改为数据成员
	// std::shared_ptr<map<string, set<int>>>     _index {nullptr};
	// map<string, int>          _wd_freq; //好像没用
};

class EchoServer {
public:
	EchoServer(size_t threadNum, size_t queSize, const string &ip, unsigned short port);
	~EchoServer();

	//服务器的启动与停止
	void start();
	void stop();

	//三个回调
	void onNewConnection(const TcpConnectionPtr &con);
	void onMessage(const TcpConnectionPtr &con);
	void onClose(const TcpConnectionPtr &con);

private:
	ThreadPool _pool;
	TcpServer  _server;
};

#endif
