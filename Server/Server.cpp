#include "Server.h"
#pragma warning(disable:4996)

void Server::Init() {

	RECV_TIMEOUT = 10;//接收消息超时
	SEND_TIMEOUT = 10;//发送消息超时
	total = 0;//当前已经链接的客服端服务数
	isLife[0] = true;//表示服务端存活
	//isLife.length()为何报错？
	for (int i = 1; i < 10; i++) isLife[i] = false;
	setHostnameIp();

	//1、初始化socket库
	WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(主版本号, 副版本号)
	//2、创建socket
	servSock = socket(AF_INET, SOCK_STREAM, 0);//面向网路的流式套接字

	//3、将服务器地址打包在一个结构体里面
	servAddr.sin_family = AF_INET;//和服务器的socket一样，sin_family表示协议簇，一般用AF_INET表示TCP/IP协议。
	servAddr.sin_addr.S_un.S_addr = inet_addr(serverIp);//服务端地址设置为本地回环地址,192.168.125.155
	servAddr.sin_port = htons(12345);//host to net short 端口号设置为12345
	//4、绑定服务端的socket和打包好的地址
	bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
	//4.5给服务端sokect绑定一个事件对象，用来接收客户端链接的事件
	WSAEVENT servEvent = WSACreateEvent();//创建一个人工重设为传信的事件对象
	WSAEventSelect(servSock, servEvent, FD_ALL_EVENTS);//绑定事件对象，并且监听所有事件
	cliSock[0] = servSock;
	cliEvent[0] = servEvent;

	//5、开启监听
	listen(servSock, 10);//监听队列长度为10

	//6、其他初始化
	for (int i = 0; i < 10; i++) {
		sendTo[i] = 0;
		sendType[i] = 0;
	}

	//test
	sendTo[1] = 2;
}

//关键更改——可能出错
int Server::serchIp(char* ip) {
	for (int i = 2; i < total + 1; i++) {
		//if (!isLife[i]) continue;
		if (strcmp(inet_ntoa(cliAddr[i].sin_addr), ip) == 0) return i;
	}
	return -1;
}

//设置主机名和ip号
void Server::setHostnameIp() {
	WORD wVersionRequested;//WORD类型变量，用于存放Winsock版本的值
	WSADATA wsaData;

	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);
	//调用MAKEWORD()函数获得Winsock的版本，用于加载Winsock库
	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		//加载Winsock库，如果WSAStartup()函数的返回值为0，说明加载成功
		if (gethostname(serverName, sizeof(serverName)) == 0)
		{
			//判断是否成功的将本地主机名存放入由name参数指定的缓冲区中
			if ((hostinfo = gethostbyname(serverName)) != NULL)
			{
				//如果获得主机名成功的话，调用inet_ntoa()函数取得IP地址
				serverIp = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				cout << "我是服务端\n";
				printf("本机的IP地址是：%s\n", serverIp);//输出IP地址 
				printf("本机的名称是：%s\n\n\n", serverName);
			}
		}
		WSACleanup();//卸载Winsock库，并释放所有资源
	}

}

//群发消息功能
void Server::sendToAll(char* buf) {
	for (int j = 1; j <= total; j++)
	{
		cout << sizeof(buf[j]) << endl;
		if (isLife[j])send(cliSock[j], buf, 1024, 0);
	}
}

//void Server::sendTo(send_info info) {
//
//}

DWORD WINAPI servEventThread( LPVOID tmp) //服务器端线程
{
	Server s =*(Server*) tmp;
	//该线程负责处理服务端和各个客户端发生的事件
	//将传入的参数初始化
	SOCKET servSock = s.servSock;//LPVOID为空指针类型，需要先转成SOCKET类型再引用，即可使用传入的SOCKET
	while (1) //不停执行
	{
		for (int i = 0; i < s.total + 1; i++)//i代表现在正在监听事件的终端
		{
			if (!s.isLife[i]) continue;

			//若有一个客户端链接，total==1，循环两次，包含客户端和服务端
			//对每一个终端（客户端和服务端），查看是否发生事件，等待WAIT_TIME毫秒
			int index = WSAWaitForMultipleEvents(1, &s.cliEvent[i], false, s.WAIT_TIME, 0);

			index -= WSA_WAIT_EVENT_0;//此时index为发生事件的终端下标

			if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
			{
				continue;//如果出错或者超时，即跳过此终端
			}



			else if (index == 0)
			{
				WSANETWORKEVENTS networkEvents;
				WSAEnumNetworkEvents(s.cliSock[i], s.cliEvent[i], &networkEvents);//查看是什么事件

				//事件选择
				if (networkEvents.lNetworkEvents & FD_ACCEPT)//若产生accept事件（此处与位掩码相与）
				{
					if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						cout << "连接时产生错误，错误代码" << networkEvents.iErrorCode[FD_ACCEPT_BIT] << endl;
						continue;
					}
					//接受链接
					if (s.total + 1 < s.MAX_LINK_NUM)//若增加一个客户端仍然小于最大连接数，则接受该链接
					{

						//total为曾经连接总客户端数量
						int nextIndex = s.total + 1;//分配给新客户端的下标
						int addrLen = sizeof(SOCKADDR);
						SOCKET newSock = accept(servSock, (SOCKADDR*)&s.cliAddr[nextIndex], &addrLen);

						//auto client = inet_ntoa(s.cliAddr[nextIndex].sin_addr);
						//获取客户端IP
						int tmp = s.serchIp(inet_ntoa(s.cliAddr[nextIndex].sin_addr));
						if (tmp != -1) {
							int tmp1 = nextIndex;
							nextIndex = tmp;
							s.cliAddr[nextIndex] = s.cliAddr[tmp1];
							s.cliAddr[tmp1] = s.cliAddr[tmp1 + 1];
							//发送离线信息
							//\n
							char offLineMsg[512] = "您最近的一条消息：";
							strcat(offLineMsg, s.offLineMsg[nextIndex]);
							/*send(s.cliSock[nextIndex], "您最近的一条消息：", sizeof("您最近的一条消息："), 0);
							send(s.cliSock[nextIndex], s.offLineMsg[nextIndex], sizeof(s.offLineMsg[nextIndex]), 0);*/
							send(s.cliSock[nextIndex], offLineMsg, 1024,0);
						}
						else s.total++;//客户端连接数增加

						//存储ip地址
						strcpy(s.clientIp[nextIndex], inet_ntoa(s.cliAddr[i].sin_addr));

						if (newSock != INVALID_SOCKET)
						{
							//设置发送和接收时限
							/*setsockopt(newSock, SOL_SOCKET, SO_SNDTIMEO, (const char*) & SEND_TIMEOUT, sizeof(SEND_TIMEOUT));
							setsockopt(newSock, SOL_SOCKET, SO_SNDTIMEO, (const char*) &RECV_TIMEOUT, sizeof(RECV_TIMEOUT));*/
							//给新客户端分配socket
							s.cliSock[nextIndex] = newSock;
							s.life++;//存活客户端增加
							s.isLife[nextIndex] = true;
							//新客户端的地址已经存在cliAddr[nextIndex]中了
							//为新客户端绑定事件对象,同时设置监听，close，read，write
							WSAEVENT newEvent = WSACreateEvent();
							WSAEventSelect(s.cliSock[nextIndex], newEvent, FD_CLOSE | FD_READ | FD_WRITE);
							s.cliEvent[nextIndex] = newEvent;
							//服务端显示
							cout << "#" << nextIndex << "游客 "<< nextIndex <<"（IP：" << inet_ntoa(s.cliAddr[nextIndex].sin_addr) << ")上线，当前连接数：" << s.life << endl;

							//告知客户端是几号
							char tellCli[128] = "客户端 ";
							char charTellCli[20];
							itoa(nextIndex, charTellCli, 10);
							strcat(tellCli, charTellCli);
							strcat(tellCli, " 号");

							send(s.cliSock[nextIndex], tellCli, sizeof(tellCli), 0);
							//memset(&buf, 0, MAXBYTE);
							// 
							//给所有客户端发送欢迎消息
							char buf[BUFFER_SIZE] = "[Server]welcome ";
							char charNextIndex[20];
							itoa(nextIndex, charNextIndex, 10);

							strcat(buf, "number"); strcat(buf, charNextIndex);
							strcat(buf, "（IP：");
							strcat(buf, inet_ntoa(s.cliAddr[nextIndex].sin_addr));
							strcat(buf, ")online");

							//s.sendToAll(buf);	//出错！！！！！！！！！不能正确传递buf-size
							for (int j = 1; j <= s.total; j++)
							{
								//cout << sizeof(buf) << endl;
								if (s.isLife[j])send(s.cliSock[j], buf, 1024, 0);
							}
						}


						//发送离线信息
							//\n
						if (tmp != -1){
							char offLine[512] = "您最近的一条消息：\n";
							strcat(offLine, s.offLineMsg[nextIndex]);
							/*send(s.cliSock[nextIndex], "您最近的一条消息：", sizeof("您最近的一条消息："), 0);
							send(s.cliSock[nextIndex], s.offLineMsg[nextIndex], sizeof(s.offLineMsg[nextIndex]), 0);*/
							send(s.cliSock[nextIndex], offLine, 1024, 0);
						}

					}

				}





				else if (networkEvents.lNetworkEvents & FD_CLOSE)//客户端被关闭，即断开连接
				{

					//i表示已关闭的客户端下标
					s.life--;
					cout << "#" << i << "游客（IP：" << inet_ntoa(s.cliAddr[i].sin_addr) << ")退出了聊天室,当前连接数：" << s.life << endl;
					//释放这个客户端的资源
					closesocket(s.cliSock[i]);
					WSACloseEvent(s.cliEvent[i]);

					////数组调整,用顺序表删除元素
					//for (int j = i; j < total; j++)
					//{
						//s.cliSock[j] = s.cliSock[j + 1];
						//s.cliEvent[j] = s.cliEvent[j + 1];
						//s.cliAddr[j] = s.cliAddr[j + 1];
					//}
					//给所有客户端发送退出聊天室的消息
					char buf[BUFFER_SIZE] = "[Server]（IP：";
					strcat(buf, inet_ntoa(s.cliAddr[i].sin_addr));
					strcat(buf, ")offline");
					s.isLife[i] = false;
					//s.sendToAll(buf);
					for (int j = 1; j <= s.total; j++)
					{
						cout << sizeof(buf) << endl;
						if (s.isLife[j])send(s.cliSock[j], buf, 1024, 0);
					}

				}





				else if (networkEvents.lNetworkEvents & FD_READ)//接收到消息
				{

					char buffer[BUFFER_SIZE] = { 0 };//字符缓冲区，用于接收和发送消息
					char buffer2[BUFFER_SIZE] = { 0 };

					//for (int j = 1; j <= s.total; j++)
					//{
					int nrecv = recv(s.cliSock[i], buffer, sizeof(buffer), 0);//nrecv是接收到的字节数
					if (nrecv > 0)//如果接收到的字符数大于0
					{
						sprintf(buffer2, "[#%d]%s", i, buffer);
						//在服务端显示
						cout << buffer2 << endl;
						if (strcmp(buffer, "#file")==0) {
							s.sendType[i] = 1;
							//出错
							send(s.cliSock[s.sendTo[i]], buffer, sizeof(buffer), 0);
							memset(&buffer, 0, BUFFER_SIZE);

							/*do {
								nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
								cout << "正在接收文件\n";
								if (strcmp(buffer, "#end")) break;
								fwrite(buffer, nCount, 1, fp);
								memset(&buffer, 0, BUFFER_SIZE);
							} while (BUFFER_SIZE == sizeof(nCount));*/

							do{
								nrecv = recv(s.cliSock[i], buffer, sizeof(buffer), 0);
								send(s.cliSock[s.sendTo[i]], buffer, BUFFER_SIZE, 0);
								printf("%s", buffer);
								cout << buffer;
								memset(&buffer, 0, BUFFER_SIZE);
								//memset(&buffer2, 0, MAXBYTE);
							}while (BUFFER_SIZE == nrecv);
							cout << "文件传输完成\n";
						}

						else if (strcmp(buffer, "#photo")==0) {
							s.sendType[i] = 2;
							//出错
							send(s.cliSock[s.sendTo[i]], buffer, sizeof(buffer), 0);
							memset(&buffer, 0, BUFFER_SIZE);

							/*do {
								nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
								cout << "正在接收文件\n";
								if (strcmp(buffer, "#end")) break;
								fwrite(buffer, nCount, 1, fp);
								memset(&buffer, 0, BUFFER_SIZE);
							} while (BUFFER_SIZE == sizeof(nCount));*/

							do{
								nrecv = recv(s.cliSock[i], buffer, sizeof(buffer), 0);
								send(s.cliSock[s.sendTo[i]], buffer, BUFFER_SIZE, 0);
								printf("%s", buffer);
								cout << buffer;
								memset(&buffer, 0, BUFFER_SIZE);
								//memset(&buffer2, 0, MAXBYTE);
							}while (BUFFER_SIZE == nrecv);
							cout << "文件传输完成\n";
						}


						else if (strcmp(buffer, "#people")==0) {
							memset(&buffer, 0, MAXBYTE);
							strcat(buffer, "在线的主机有：\n");
							send(s.cliSock[i], buffer, sizeof(buffer), 0);
							memset(&buffer, 0, MAXBYTE);
							for (int p = 1; p < s.total+1; p++) {
								char* tmp;
								if (s.isLife[p]) tmp = inet_ntoa(s.cliAddr[i].sin_addr);
								else continue;
								strcat(tmp, "    ");
								send(s.cliSock[i], tmp, BUFFER_SIZE, 0);
								//memset(&tmp, 0, MAXBYTE);
							}
							continue;
						}
						else if (buffer[0] == '#') {
							s.sendTo[i] = buffer[1] - '0';
						}
						//else {
							if (s.sendTo[i] != 0) {
								if (s.isLife[s.sendTo[i]])send(s.cliSock[s.sendTo[i]], buffer2, sizeof(buffer2), 0);
								//离线信息存储
								else {
									strcpy(s.offLineMsg[s.sendTo[i]], buffer2);
								}
							}
							else {
								for (int k = 1; k <= s.total; k++)
									send(s.cliSock[k], buffer2, sizeof(buffer2), 0);
							}
						//}

						////在其他客户端显示（'广'播给其他客户端）
						//for (int k = 1; k <= s.total; k++)
						//{
						//	send(s.cliSock[k], buffer2, sizeof(buffer2), 0);
						//}
					}

					//}



				}
			}
		}


	}
	return 0;
}