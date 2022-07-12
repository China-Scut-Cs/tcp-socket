#include "Server.h"
Server s;


int main()
{
	s.Init();
	cout << s.serverIp << endl;
	//6、创建接受链接的线程
	//不需要句柄所以直接关闭
	cout << "聊天室服务器已开启" << endl;
	//servEventThread(s);
	CloseHandle(CreateThread(NULL, 0, servEventThread, (LPVOID)&s, 0, 0));

	//connect test
		//int addrLen = sizeof(SOCKADDR);//用于接收客户端的地址包结构体长度
		//SOCKET cliSOCK = accept(servSock, (SOCKADDR*)&servAddr,&addrLen);
		//if (cliSOCK != INVALID_SOCKET) 
		//{
		//	cout << "链接成功" << endl;
		//}
		//while (1)
		//{
		//	char buf[100] = { 0 };//测试缓冲区
		//	int nrecv = recv(cliSOCK, buf, sizeof(buf), 0);
		//	if (nrecv > 0)//如果接收到客户端的信息就输出到屏幕
		//	{
		//		cout << buf << endl;
		//	}
		//}
		//需要让主线程一直运行下去
		//发送消息给全部客户端
	while (1)
	{

		char contentBuf[BUFFER_SIZE] = { 0 };
		char sendBuf[BUFFER_SIZE] = { 0 };
		cin.getline(contentBuf, sizeof(contentBuf));
		sprintf(sendBuf, "[智能小易]%s", contentBuf);
		for (int j = 1; j <= s.total; j++)
		{
			send(s.cliSock[j], sendBuf, sizeof(sendBuf), 0);
		}

	}
	//1-关闭socket库的收尾工作
	WSACleanup();
	return 0;
}
