#include "EchoServer.h"
#include "LoadFile.h"
#include <csignal>   // for signal() and SIGINT
#include <unistd.h>  // for other system calls
#include <iostream>

using std::cout;
using std::endl;
static EchoServer* global_server = nullptr;

// 信号处理函数
void handle_sigint(int sig) {
    // cout << "Received SIGINT, shutting down..." << endl;
    ERROR("Received SIGINT, shutting down...");
    LoadFile::destroy(); // 调用loadfile的destroy函数
    TODO://调用     server.stop();
    if(nullptr != global_server){
        global_server->stop();
    }
    Mylogger::destroy(); // 调用log的destroy函数
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    INFO("programer begian...");
    signal(SIGINT, handle_sigint);
    // 注册信号处理函数
    EchoServer server(4, 10, "0.0.0.0", 44444);
    // 设置全局指针
    global_server = &server;
    server.start();

    // 服务器正常停止后的清理
    server.stop();
    LoadFile::destroy();
    Mylogger::destroy();
    
    INFO("programer exited normally");
    return 0;
}
