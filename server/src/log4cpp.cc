#include "log4cpp.h"
Mylogger *Mylogger::_pointor = nullptr;
#ifdef __CONSOLE__LOG
PatternLayout *Mylogger::ptn1 = nullptr;
OstreamAppender *Mylogger::pos = nullptr;
#endif
PatternLayout *Mylogger::_ptn2 = nullptr;

RollingFileAppender *Mylogger::_roll = nullptr;
Category *Mylogger::_searcher2 = nullptr;

Mylogger::~Mylogger()
{
}

// 可变参数日志实现

// void Mylogger::info(const char *format, const char *file_name, const char *fun_name, int line, ...) {
//     // char buffer[1024];
//     // va_list args;
//     // // 
//     // // 处理可变参数
//     // va_start(args, line);
//     // vsnprintf(buffer, sizeof(buffer), format, args);
//     // va_end(args);

// 	va_list args;
//     va_start(args, line);
    
//     // 使用fmt处理{}格式化
//     std::string buffer = fmt::vformat(format, fmt::make_format_args(args));
    
//     va_end(args);
    
//     // 提取文件名
//     const char* filename = extractFileName(file_name);
    
//     // 组装日志消息
//     string ter_msg = " " + string(filename) + " " + string(fun_name) + ":" + to_string(line) + " :" + buffer;
    
//     // 记录日志
//     Category::getRoot().getInstance("searchEngine").info(ter_msg);
// }

// void Mylogger::warn(const char *format, const char *file_name, const char *fun_name, int line, ...) {
// 	va_list args;
//     va_start(args, line);
    
//     // 使用fmt处理{}格式化
//     std::string buffer = fmt::vformat(format, fmt::make_format_args(args));
    
//     va_end(args);
    
//     // 提取文件名
//     const char* filename = extractFileName(file_name);
    
//     // 组装日志消息
//     string ter_msg = " " + string(filename) + " " + string(fun_name) + ":" + to_string(line) + " :" + buffer;
    
//     // 记录日志
//     Category::getRoot().getInstance("searchEngine").warn(ter_msg);
// }

// void Mylogger::error(const char *format, const char *file_name, const char *fun_name, int line, ...) {
// 	va_list args;
//     va_start(args, line);
    
//     // 使用fmt处理{}格式化
//     std::string buffer = fmt::vformat(format, fmt::make_format_args(args));
    
//     va_end(args);
    
//     // 提取文件名
//     const char* filename = extractFileName(file_name);
    
//     // 组装日志消息
//     string ter_msg = " " + string(filename) + " " + string(fun_name) + ":" + to_string(line) + " :" + buffer;
    
//     // 记录日志
//     Category::getRoot().getInstance("searchEngine").error(ter_msg);
// }

// void Mylogger::fatal(const char *format, const char *file_name, const char *fun_name, int line, ...) {
// 	va_list args;
//     va_start(args, line);
    
//     // 使用fmt处理{}格式化
//     std::string buffer = fmt::vformat(format, fmt::make_format_args(args));
    
//     va_end(args);
    
//     // 提取文件名
//     const char* filename = extractFileName(file_name);
    
//     // 组装日志消息
//     string ter_msg = " " + string(filename) + " " + string(fun_name) + ":" + to_string(line) + " :" + buffer;
    
//     // 记录日志
//     Category::getRoot().getInstance("searchEngine").fatal(ter_msg);
// }

// void Mylogger::crit(const char *format, const char *file_name, const char *fun_name, int line, ...) {
// 	va_list args;
//     va_start(args, line);
    
//     // 使用fmt处理{}格式化
//     std::string buffer = fmt::vformat(format, fmt::make_format_args(args));
    
//     va_end(args);
    
//     // 提取文件名
//     const char* filename = extractFileName(file_name);
    
//     // 组装日志消息
//     string ter_msg = " " + string(filename) + " " + string(fun_name) + ":" + to_string(line) + " :" + buffer;
    
//     // 记录日志
//     Category::getRoot().getInstance("searchEngine").crit(ter_msg);
// }

// void Mylogger::debug(const char *msg, const char *file_name, const char *fun_name, int line)
// {
// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.debug(ter_msg);
// }

// void Mylogger::info(const char *msg, const char *file_name, const char *fun_name, int line)
// {
// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.info(ter_msg);
// }

// void Mylogger::warn(const char *msg, const char *file_name, const char *fun_name, int line)
// {
// 	//3.创建记录器
// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.warn(ter_msg);
// }

// void Mylogger::error(const char *msg, const char *file_name, const char *fun_name, int line)
// {

// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.error(ter_msg);
// }

// void Mylogger::fatal(const char *msg, const char *file_name, const char *fun_name, int line)
// {
// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.fatal(ter_msg);
// }

// void Mylogger::crit(const char *msg, const char *file_name, const char *fun_name, int line)
// {
// 	Category &searcher = Category::getRoot().getInstance("searchEngine");

// 	string ter_msg = " " + (string)file_name + " " + (string)fun_name + ":" + to_string(line) + " :" + msg;
// 	searcher.crit(ter_msg);
// }
