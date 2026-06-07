#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <arpa/inet.h>

using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd, EventLoop *loop)
    : _loop(loop)
    , _sockIO(fd)
    , _sock(fd)
    , _localAddr(getLocalAddr())
    , _peerAddr(getPeerAddr())
{
}

TcpConnection::~TcpConnection()
{
}

// bool TcpConnection::send(const string &msg)
// {
// 	int ret = _sockIO.writen(msg.c_str(), msg.size());
// 	if(-1 == ret){
// 		ERROR("sockIO writen bad return ret:{}", ret);
// 		return false;
// 	}
// 	return true;
// }

bool TcpConnection::send(const string &msg)
{
	// 1. 准备4字节网络字节序的长度头
	uint32_t len    = msg.size(); // 原始长度
	uint32_t netLen = htonl(len); // 转换为网络字节序（大端）

	// 2. 先发送长度头 (固定4字节)
	int ret = _sockIO.writen(reinterpret_cast<const char *>(&netLen), sizeof(netLen));
	if(ret != sizeof(netLen)) { // 必须完整发送4字节
		ERROR("Failed to send length header, ret:{}", ret);
		return false;
	}

	// 3. 再发送消息内容
	if(len > 0) { // 避免空消息
		ret = _sockIO.writen(msg.c_str(), len);
		if(ret != static_cast<int>(len)) {
			ERROR("Failed to send message content, sent {}/{} bytes", ret, len);
			return false;
		}
	}
	return true;
}

//需要做什么？
//此处的参数msg就是线程池处理好之后的可以发送出去的数据
//需要将其传递个EventLoop
//因为EventLoop本身并没有发送数据的能力，即使将线程池处理好之后的
//msg发给EventLoop，EventLoop也不能将msg发给客户端
//所有需要将发送的数据msg，以及发送数据能力的send函数，以及执行
//send函数的连接TcpConnection的对象都传递给（发送给）EventLoop，
//这样才能将数据发给客户端
bool TcpConnection::sendInLoop(const string &msg)
{
	if(_loop) {
		/* function<void()> f = bind(&TcpConnection::send, this, msg); */
		//void runInLoop(function<void()> &&)
		_loop->runInLoop(bind(&TcpConnection::send, this, msg));
		return true;
	}
	return false;
}
string TcpConnection::receive()
{
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	// BUG: send() 发送的是 4 字节二进制长度头（htonl），但 receive() 用 readLine()（文本式，遇 '\n' 停止）
	// 若二进制长度字节中出现 0x0A，会导致读取提前截断，解析错位
	// char head[4] = {0};
	// _sockIO.readLine(head, sizeof(int));
	// string h(head);
	// char buff[65535] = {0};
	// _sockIO.readLine(buff, sizeof(buff));
	// FIX: 改用 readn 读取固定 4 字节长度头，再 readn 读取完整报文
	// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
	char head[4] = {0};
	_sockIO.readn(head, sizeof(head));
	uint32_t netLen;
	memcpy(&netLen, head, sizeof(netLen));
	uint32_t len = ntohl(netLen);

	vector<char> buf(len + 1, 0);
	_sockIO.readn(buf.data(), len);
	buf[len] = '\0';

	return string(buf.data());
}

bool TcpConnection::isClosed() const
{
	char buff[100] = {0};
	int  ret       = ::recv(_sock.fd(), buff, sizeof(buff), MSG_PEEK);

	return (0 == ret);
}

string TcpConnection::toString()
{
	ostringstream oss;
	oss << _localAddr.ip() << ":"
	    << _localAddr.port() << "---->"
	    << _peerAddr.ip() << ":"
	    << _peerAddr.port();
	return oss.str();
}

//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr()
{
	struct sockaddr_in addr;
	socklen_t          len = sizeof(struct sockaddr);
	//获取本端地址的函数getsockname
	int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
	if(-1 == ret) {
		perror("getsockname");
		ERROR("getsockname bad return ret:{}", ret);
	}

	return InetAddress(addr);
}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr()
{
	struct sockaddr_in addr;
	socklen_t          len = sizeof(struct sockaddr);
	//获取对端地址的函数getpeername
	int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
	if(-1 == ret) {
		perror("getpeername");
		ERROR("getpeername bad return ret:{}", ret);
	}

	return InetAddress(addr);
}

//三个回调的注册
void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback &cb)
{
	_onNewConnectionCb = cb;
}

void TcpConnection::setMessageCalback(const TcpConnectionCallback &cb)
{
	_onMessageCb = cb;
}

void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb)
{
	_onCloseCb = cb;
}

//三个回调的执行
void TcpConnection::handleNewConnectionCallback()
{
	if(_onNewConnectionCb) {
		/* shared_ptr<TcpConnection> ttt(this); */
		/* _onNewConnectionCb(shared_ptr<TcpConnection>(this)); */
		_onNewConnectionCb(shared_from_this());
	} else {
		cout << "_onNewConnectionCb == nullptr" << endl;
		ERROR("_onNewConnectionCb == nullptr");
	}
}

void TcpConnection::handleMessageCallback()
{
	if(_onMessageCb) {
		_onMessageCb(shared_from_this());
	} else {
		cout << "_onMessageCb == nullptr" << endl;
		ERROR("_onMessageCb == nullptr");
	}
}

void TcpConnection::handleCloseCallback()
{
	if(_onCloseCb) {
		_onCloseCb(shared_from_this());
	} else {
		cout << "_onCloseCb == nullptr" << endl;
		ERROR("_onCloseCb == nullptr");
	}
}
