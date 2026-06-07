#include "Acceptor.h"
#include <stdio.h>
#include <iostream>
#include <stdexcept>

Acceptor::Acceptor(const string &ip, unsigned short port)
    : _sock()
    , _addr(ip, port)
{
}

Acceptor::~Acceptor()
{
}

void Acceptor::ready()
{
	setReuseAddr();
	setReusePort();
	bind();
	listen();
}

void Acceptor::setReuseAddr()
{
	int on  = 1;
	int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret) {
		perror("setsockopt");
		return;
	}
}

void Acceptor::setReusePort()
{
	int on  = 1;
	int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if(-1 == ret) {
		perror("setsockopt");
		return;
	}
}

void Acceptor::bind()
{
	int ret = ::bind(_sock.fd(),
	                 (struct sockaddr *)_addr.getInetAddrPtr(),
	                 sizeof(struct sockaddr));
	if(-1 == ret) {
		perror("bind");
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// BUG: exit(-1) 直接终止进程，不调用任何析构函数或清理代码
		// exit(-1);
		// FIX: 抛出异常以执行正确的栈展开
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		throw std::runtime_error("bind() failed");
		return;
	}
}

void Acceptor::listen()
{
	int ret = ::listen(_sock.fd(), 128);
	if(-1 == ret) {
		perror("listen");
		return;
	}
}

int Acceptor::accept()
{
	int connfd = ::accept(_sock.fd(), nullptr, nullptr);
	if(-1 == connfd) {
		perror("accept");
		return -1;
	}
	return connfd;
}

int Acceptor::fd() const
{
	return _sock.fd();
}
