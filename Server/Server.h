#pragma once
#include<Winsock2.h>//socket头文件
#include<cstring>
#include"pch.h"//预编译头
#include<iostream>
#include"info.h"
#pragma warning(disable:4996)
using namespace std;
//载入系统提供的socket动态链接库
#pragma comment(lib,"ws2_32.lib")   //socket库

//==============================全局变量区=============================
const int BUFFER_SIZE = 1024;//缓冲区大小

//==============================函数声明===============================
DWORD WINAPI servEventThread(LPVOID tmp);//服务器端处理线程

//==============================类区===================================
class Server //:public QDialog
{

public:
	void Init();
	int serchIp(char* ip);
	void setHostnameIp();
	void sendToAll(char*);
	//void sendMsgTo(send_info info);

	WSADATA wsaData;//获取版本信息，说明要使用的版本
	SOCKADDR_IN servAddr; //sockaddr_in 是internet环境下套接字的地址形式
	SOCKET servSock;
	char serverName[1024];
	LPCSTR serverIp;



	int RECV_TIMEOUT;//接收消息超时
	int SEND_TIMEOUT = 10;//发送消息超时
	const int WAIT_TIME = 10;//每个客户端等待事件的时间，单位毫秒
	const int MAX_LINK_NUM = 10;//服务端最大链接数
	SOCKET cliSock[10];//客户端套接字 0号为服务端
	SOCKADDR_IN cliAddr[10];//客户端地址
	WSAEVENT cliEvent[10];//客户端事件 0号为服务端,它用于让程序的一部分等待来自另一部分的信号。例如，当数据从套接字变为可用时，winsock 库会将事件设置为信号状态
	int total;//总已经链接的客服端服务数
	int life;//存活的客服端服务数
	char clientIp[10][64];
	bool isLife[10];
	int sendTo[10];
	int sendType[10];
	char offLineMsg[10][128];

};




