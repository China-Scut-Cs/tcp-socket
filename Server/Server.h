#pragma once
#include<Winsock2.h>//socketͷ�ļ�
#include<cstring>
#include"pch.h"//Ԥ����ͷ
#include<iostream>
#include"info.h"
#pragma warning(disable:4996)
using namespace std;
//����ϵͳ�ṩ��socket��̬���ӿ�
#pragma comment(lib,"ws2_32.lib")   //socket��

//==============================ȫ�ֱ�����=============================
const int BUFFER_SIZE = 1024;//��������С

//==============================��������===============================
DWORD WINAPI servEventThread(LPVOID tmp);//�������˴����߳�

//==============================����===================================
class Server //:public QDialog
{

public:
	void Init();
	int serchIp(char* ip);
	void setHostnameIp();
	void sendToAll(char*);
	//void sendMsgTo(send_info info);

	WSADATA wsaData;//��ȡ�汾��Ϣ��˵��Ҫʹ�õİ汾
	SOCKADDR_IN servAddr; //sockaddr_in ��internet�������׽��ֵĵ�ַ��ʽ
	SOCKET servSock;
	char serverName[1024];
	LPCSTR serverIp;



	int RECV_TIMEOUT;//������Ϣ��ʱ
	int SEND_TIMEOUT = 10;//������Ϣ��ʱ
	const int WAIT_TIME = 10;//ÿ���ͻ��˵ȴ��¼���ʱ�䣬��λ����
	const int MAX_LINK_NUM = 10;//��������������
	SOCKET cliSock[10];//�ͻ����׽��� 0��Ϊ�����
	SOCKADDR_IN cliAddr[10];//�ͻ��˵�ַ
	WSAEVENT cliEvent[10];//�ͻ����¼� 0��Ϊ�����,�������ó����һ���ֵȴ�������һ���ֵ��źš����磬�����ݴ��׽��ֱ�Ϊ����ʱ��winsock ��Ὣ�¼�����Ϊ�ź�״̬
	int total;//���Ѿ����ӵĿͷ��˷�����
	int life;//���Ŀͷ��˷�����
	char clientIp[10][64];
	bool isLife[10];
	int sendTo[10];
	int sendType[10];
	char offLineMsg[10][128];

};




