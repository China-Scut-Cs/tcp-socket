#pragma once
// 聊天程序客户端

#include"pch.h"//预编译头
#include<iostream>
#include<Winsock2.h>//socket头文件
#include<cstring>
#include"Info.h"
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")   //socket库

using namespace std;

/************全局变量***********/
const int BUFFER_SIZE = 1024;//缓冲区大小

class Client
{
public:
	char name[32];//用于存放主机名
	size_t namelen = 256;
	int sendType = 0;//发送消息类型	0-文字 1-图片、文件
	int recvType = 0;//接受消息类型
	LPCSTR ip;
	/*int sendType = 0;
	int recvType = 0;*/
	char filename[100] = "C:\\Users\\root\\Desktop\\tmpfile\\Client1.txt";
	char photoname[100] = "C:\\Users\\root\\Desktop\\tmpfile\\Client1.png";
	SOCKET cliSock;//客户端sock
	SOCKADDR_IN cliAddr;//客户端
	SOCKADDR_IN servAddr;//服务端
	Message sendInfo;
	Message recvInfo;
	int sendTo;//发给几号客户端
	int recvFrom;//从几号收到的



	void sendMsg();
	//void recvMsg();
	void Init();
	void setHostnameIp();//获取本机域名和ip
};

DWORD WINAPI recvMsgThread(LPVOID IpParameter);

