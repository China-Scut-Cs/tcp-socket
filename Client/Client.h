#pragma once
// �������ͻ���

#include"pch.h"//Ԥ����ͷ
#include<iostream>
#include<Winsock2.h>//socketͷ�ļ�
#include<cstring>
#include"Info.h"
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")   //socket��

using namespace std;

/************ȫ�ֱ���***********/
const int BUFFER_SIZE = 1024;//��������С

class Client
{
public:
	char name[32];//���ڴ��������
	size_t namelen = 256;
	int sendType = 0;//������Ϣ����	0-���� 1-ͼƬ���ļ�
	int recvType = 0;//������Ϣ����
	LPCSTR ip;
	/*int sendType = 0;
	int recvType = 0;*/
	char filename[100] = "C:\\Users\\root\\Desktop\\tmpfile\\Client1.txt";
	char photoname[100] = "C:\\Users\\root\\Desktop\\tmpfile\\Client1.png";
	SOCKET cliSock;//�ͻ���sock
	SOCKADDR_IN cliAddr;//�ͻ���
	SOCKADDR_IN servAddr;//�����
	Message sendInfo;
	Message recvInfo;
	int sendTo;//�������ſͻ���
	int recvFrom;//�Ӽ����յ���



	void sendMsg();
	//void recvMsg();
	void Init();
	void setHostnameIp();//��ȡ����������ip
};

DWORD WINAPI recvMsgThread(LPVOID IpParameter);

