#include "Server.h"
Server s;


int main()
{
	s.Init();
	cout << s.serverIp << endl;
	//6�������������ӵ��߳�
	//����Ҫ�������ֱ�ӹر�
	cout << "�����ҷ������ѿ���" << endl;
	//servEventThread(s);
	CloseHandle(CreateThread(NULL, 0, servEventThread, (LPVOID)&s, 0, 0));

	//connect test
		//int addrLen = sizeof(SOCKADDR);//���ڽ��տͻ��˵ĵ�ַ���ṹ�峤��
		//SOCKET cliSOCK = accept(servSock, (SOCKADDR*)&servAddr,&addrLen);
		//if (cliSOCK != INVALID_SOCKET) 
		//{
		//	cout << "���ӳɹ�" << endl;
		//}
		//while (1)
		//{
		//	char buf[100] = { 0 };//���Ի�����
		//	int nrecv = recv(cliSOCK, buf, sizeof(buf), 0);
		//	if (nrecv > 0)//������յ��ͻ��˵���Ϣ���������Ļ
		//	{
		//		cout << buf << endl;
		//	}
		//}
		//��Ҫ�����߳�һֱ������ȥ
		//������Ϣ��ȫ���ͻ���
	while (1)
	{

		char contentBuf[BUFFER_SIZE] = { 0 };
		char sendBuf[BUFFER_SIZE] = { 0 };
		cin.getline(contentBuf, sizeof(contentBuf));
		sprintf(sendBuf, "[����С��]%s", contentBuf);
		for (int j = 1; j <= s.total; j++)
		{
			send(s.cliSock[j], sendBuf, sizeof(sendBuf), 0);
		}

	}
	//1-�ر�socket�����β����
	WSACleanup();
	return 0;
}
