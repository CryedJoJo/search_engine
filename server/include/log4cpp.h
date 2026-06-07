#ifndef __LOG4CPP__H
#define __LOG4CPP__H
// #define __LOG_DEBUG
// #define __LOG_INFO

#include <iostream>
#include <fstream>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <fmt/core.h>
#include <fmt/format.h>
// #include <57header.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace log4cpp;

// 定义宏函数 INFO(msg)

#define DEBUG(format, ...)                                                                       \
	do {                                                                                         \
		Mylogger::getInstance()->debug(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

#define INFO(format, ...)                                                                       \
	do {                                                                                        \
		Mylogger::getInstance()->info(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

#define WARN(format, ...)                                                                       \
	do {                                                                                        \
		Mylogger::getInstance()->warn(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

#define ERROR(format, ...)                                                                       \
	do {                                                                                         \
		Mylogger::getInstance()->error(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

#define FATAL(format, ...)                                                                       \
	do {                                                                                         \
		Mylogger::getInstance()->fatal(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

#define CRIT(format, ...)                                                                       \
	do {                                                                                        \
		Mylogger::getInstance()->crit(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
	} while(0)

//用所学过的类和对象的知识，封装log4cpp，让其使用起来更方便，要求：可以像printf一样，
//同时输出的日志信息中最好能有文件的名字，函数的名字及其所在的行号(这个在C/C++里面有对应的宏，可以查一下)
//g++ log4cppTest.cc -llog4cpp -lpthread
static void func(const char *fun_name, const char *name, int line)
{
	printf("\n%s: %s: %d \n", name, fun_name, line);
}

static const char *extractFileName(const char *fullPath)
{
	const char *p    = fullPath;
	const char *last = fullPath;
	while(*p) {
		if(*p == '/' || *p == '\\') {
			last = p + 1;
		}
		p++;
	}
	return last;
}

// 生成当前时间格式化的字符串：2025-06-29_17:17:10
static std::string getCurrentTimeString()
{
	std::time_t now    = std::time(nullptr);
	std::tm    *tm_now = std::localtime(&now);

	std::ostringstream oss;
	oss << std::put_time(tm_now, "%Y-%m-%d_%H:%M:%S");
	return oss.str();
}

class Mylogger {
public:
	template <typename... Args>
	void debug(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).debug(ter_msg);
	}

	template <typename... Args>
	void info(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).info(ter_msg);
	}

	template <typename... Args>
	void warn(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).warn(ter_msg);
	}

	template <typename... Args>
	void error(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).error(ter_msg);
	}

	template <typename... Args>
	void fatal(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).fatal(ter_msg);
	}

	template <typename... Args>
	void crit(const char *file_name, const char *fun_name, int line, fmt::format_string<Args...> format, Args &&...args)
	{
		// 使用fmt::format直接处理{}格式化和参数包
		std::string buffer = fmt::format(format, std::forward<Args>(args)...);
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(),
		                            [](char c) { return c == '\n' || c == '\r'; }),
		             buffer.end());

		// 提取文件名
		const char *filename = extractFileName(file_name);

		pthread_t   tid = pthread_self();
		std::string PID = fmt::format("[{}]", reinterpret_cast<uintptr_t>(tid));

		// 组装日志消息
		std::string ter_msg = PID + " [" + std::string(filename) + " "
		                      + std::string(fun_name) + ":"
		                      + std::to_string(line) + "] "
		                      + buffer;

		// 记录日志
		(*_searcher2).crit(ter_msg);
	}

	static Mylogger *getInstance()
	{
		if(nullptr == _pointor) {
			_pointor = new Mylogger();
#ifdef __CONSOLE__LOG
			ptn1 = new PatternLayout();
			ptn1->setConversionPattern("%d %c [%p] %m%n");
			// 2.创建目的地对象
			pos = new OstreamAppender("console", &cout);
			pos->setLayout(ptn1);
#endif

			// 获取当前时间戳字符串
			std::string timestamp = getCurrentTimeString();

			// 构建动态文件名：原文件名 + 时间戳 + 扩展名
			std::string baseFilename = "../ServerLog/searchEngine";
			std::string logFilename  = baseFilename + timestamp + ".log";

			_ptn2 = new PatternLayout();
			_ptn2->setConversionPattern("%d %c [%p] %m%n");
			_roll = new RollingFileAppender("RollingFile", logFilename, 1024 * 1024, 5);
			_roll->setLayout(_ptn2);

			//3.创建记录器
			// Category &searcher = Category::getRoot().getInstance("searchEngine");
			_searcher2 = &Category::getRoot().getInstance("searchEngine");
#ifdef __LOG_DEBUG
			(*_searcher2).setPriority(Priority::DEBUG);
#endif

#ifdef __LOG_INFO
			(*_searcher2).setPriority(Priority::INFO);
#endif

#ifdef __CONSOLE_LOG
			(*_searcher2).addAppender(pos);
#endif
			(*_searcher2).addAppender(_roll);
		} else { //输出已经存在单例
			     // Mylogger::_pointor->info("there have been an instance of Mylogger exsited!",
			     //  __FILE__,__FUNCTION__,__LINE__);
		}
		return _pointor;
	}

	static void destroy()
	{
		WARN("start destroy log");

#ifdef __CONSOLE__LOG
		if(nullptr == ptn1) {

		} else {
			delete ptn1;
		}

		if(nullptr == pos) {

		} else {
			delete pos;
		}
#endif

		if(nullptr == _ptn2) {

		} else {
			delete _ptn2;
		}

		if(nullptr == _roll) {

		} else {
			delete _roll;
		}

		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// BUG: delete log4cpp Category 对象导致未定义行为（由 log4cpp 内部管理）
		// if(nullptr == _searcher2){
		// } else {
		//     delete _searcher2;
		// }
		// FIX: 仅置 nullptr，不删除
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		_searcher2 = nullptr;

		if(nullptr == _pointor) {
			// Mylogger::_pointor->warn("pointer have been freed!",__FILE__, __FUNCTION__, __LINE__);
			return;
		} else {
			delete _pointor;
			_pointor = nullptr;
			cout << "destroy signle Mylogger" << endl;
		}
	}

private:
	Mylogger() = default;
	~Mylogger();

	static pid_t getThreadId()
	{
		static thread_local pid_t cached_tid = 0;
		if(cached_tid == 0) {
			cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
		}
		return cached_tid;
	}

private:
	//......
	static Mylogger *_pointor;
#ifdef __CONSOLE__LOG
	static PatternLayout   *ptn1;
	static OstreamAppender *pos;
#endif
	static PatternLayout       *_ptn2;
	static RollingFileAppender *_roll;
	static Category            *_searcher2;
};

#endif
