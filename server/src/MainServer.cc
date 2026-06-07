#include "EchoServer.h"
#include "LoadFile.h"
#include "Config.h"
#include <csignal>  // for signal() and SIGINT
#include <unistd.h> // for other system calls
#include <iostream>

using std::cout;
using std::endl;
static EchoServer *global_server = nullptr;

// 信号处理函数
void handle_sigint(int sig)
{
	// cout << "Received SIGINT, shutting down..." << endl;
	ERROR("Received SIGINT, shutting down...");
	LoadFile::destroy(); // 调用loadfile的destroy函数
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: TODO: 裸露在函数体中，被当作 C++ 标签（label），不合意图
	// TODO:                    //调用     server.stop();
	// FIX: 将其放入注释中
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// TODO: 调用 server.stop();
	if(nullptr != global_server) {
		global_server->stop();
	}
#ifdef LRUCACHE_ON
	CacheManager::destroy();
#endif
	Config::destroy();
	Mylogger::destroy(); // 调用log的destroy函数
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	DEBUG("DEBUG ON~");
	INFO("programer begian...");
	signal(SIGINT, handle_sigint);
	// 注册信号处理函数

	// 从配置文件读取 ip / port
	Config        *config = Config::getInstance("../config/config.yaml");
	std::string    ip     = config->get("ip");
	unsigned short port   = static_cast<unsigned short>(std::stoi(config->get("port")));
	INFO("server config: bind ip={}, port={}", ip, port);

	EchoServer server(4, 10, ip, port);
	// 设置全局指针
	global_server = &server;
	server.start();

	// 服务器正常停止后的清理
	server.stop();
	LoadFile::destroy();
#ifdef LRUCACHE_ON
	CacheManager::destroy();
#endif
	Config::destroy();
	Mylogger::destroy();

	INFO("programer exited normally");
	return 0;
}
