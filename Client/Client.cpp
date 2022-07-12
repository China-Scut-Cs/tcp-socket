#include "Client.h"
#include"getFileSize.cpp"

void Client::Init() {
	setHostnameIp();

	//1、初始化socket库
	WSADATA wsaData;//获取版本信息，说明要使用的版本
	WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(主版本号, 副版本号)

	//2、创建socket
	cliSock = socket(AF_INET, SOCK_STREAM, 0);//面向网路的流式套接字,第三个参数代表自动选择协议

	//3、打包地址
	//客户端
	cliAddr = { 0 };
	cliAddr.sin_family = AF_INET;
	auto k = inet_addr("127.0.0.1");
	//cliAddr.sin_addr.s_addr = inet_addr("127.0.0.3");//IP地址
	cliAddr.sin_port = htons(12345);//端口号
	//服务端
	servAddr = { 0 };
	servAddr.sin_family = AF_INET;//和服务器的socket一样，sin_family表示协议簇，一般用AF_INET表示TCP/IP协议。
	servAddr.sin_addr.S_un.S_addr = inet_addr("172.20.127.28");//服务端地址设置为真实地址172.20.127.28  192.168.125.155
	servAddr.sin_port = htons(12345);//host to net short 端口号设置为12345

	if (connect(cliSock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "链接出现错误，错误代码" << WSAGetLastError() << endl;
	}

}

void Client::sendMsg() {
	//while (1)
	//{
	//	if (sendType == 0) {
	//		char buf[BUFFER_SIZE] = { 0 };
	//		cin.getline(buf, sizeof(buf));
	//		//if (strcmp(buf, "quit") == 0)//若输入“quit”，则退出聊天室
	//		//{
	//		//	break;
	//		//}
	//		//前几个带'#'号的都是命令消息
	//		if (strcmp(buf, "#file") == 0) {
	//			sendType = 1;
	//			send(cliSock, buf, sizeof(buf), 0);
	//			continue;
	//		}
	//		else if (strcmp(buf, "#people") == 0) {
	//			send(cliSock, buf, sizeof(buf), 0);
	//			continue;
	//		}
	//		else {
	//			strcat(buf, "     --by ");
	//			strcat(buf, ip);
	//			send(cliSock, buf, sizeof(buf), 0);
	//		}
	//	}
	//	//发文件-图片
	//	else {
	//		char buf[BUFFER_SIZE] = { 0 };
	//		int nCount;
	//		FILE* fp = fopen(filename, "rb");
	//		while ((nCount = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
	//			send(cliSock, buf, nCount, 0);
	//		}
	//		sendType = 0;
	//	}
	//}
}



DWORD WINAPI recvMsgThread(LPVOID IpParameter)//接收消息的线程
{
	Client c = *(Client*)IpParameter;//获取客户端的SOCKET参数

	while (1)
	{
		char buffer[BUFFER_SIZE] = { 0 };//字符缓冲区，用于接收和发送消息
		if (c.recvType == 0) {
			int nrecv = recv(c.cliSock, buffer, sizeof(buffer), 0);//nrecv是接收到的字节数
			if (strcmp(buffer, "#file") == 0) {
				c.recvType = 1;
				//continue;
			//收文件
			//else {
				int nCount;
				memset(&buffer, 0, BUFFER_SIZE);
				FILE* fp = fopen(c.filename, "wt");
				do {
					char buffer[BUFFER_SIZE] = { 0 };
					nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
					cout << "正在接收文件\n";
					//if (strcmp(buffer, "#end")) break;
					cout << buffer;
					fwrite(buffer, nCount, 1, fp);
					memset(&buffer, 0, BUFFER_SIZE);
				} while (BUFFER_SIZE == sizeof(nCount));
				fclose(fp);
				cout << "已收完文件\n";
				c.recvType = 0;
				//}
			}
			else if (strcmp(buffer, "#photo") == 0) {
				c.recvType = 1;
				//continue;
			//收文件
			//else {
				int nCount;
				memset(&buffer, 0, BUFFER_SIZE);
				FILE* fp = fopen(c.photoname, "wb+");
				do {
					char buffer[BUFFER_SIZE] = { 0 };
					nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
					cout << "正在接收文件\n";
					//if (strcmp(buffer, "#end")) break;
					cout << buffer;
					fwrite(buffer, nCount, 1, fp);
					memset(&buffer, 0, BUFFER_SIZE);
				} while (BUFFER_SIZE == sizeof(nCount));
				fclose(fp);
				//int filesize = file_size2(c.photoname);
				cout << "已收完文件，文件大小：";// << filesize << endl;
				c.recvType = 0;
				//}
			}
			if (nrecv > 0)//如果接收到的字符数大于0
			{
				cout << buffer << endl;
			}
			else if (nrecv < 0)//如果接收到的字符数小于0就说明断开连接
			{
				cout << "与服务器断开连接" << endl;
				break;
			}
		}



	}
	return 0;
}
void Client::setHostnameIp() {
	WORD wVersionRequested;//WORD类型变量，用于存放Winsock版本的值
	WSADATA wsaData;

	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);
	//调用MAKEWORD()函数获得Winsock的版本，用于加载Winsock库
	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		//加载Winsock库，如果WSAStartup()函数的返回值为0，说明加载成功
		if (gethostname(name, sizeof(name)) == 0)
		{
			//判断是否成功的将本地主机名存放入由name参数指定的缓冲区中
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				//如果获得主机名成功的话，调用inet_ntoa()函数取得IP地址
				ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				printf("本机的IP地址是：%s\n", ip);//输出IP地址 
				printf("本机的名称是：%s\n", name);
			}
		}
		WSACleanup();//卸载Winsock库，并释放所有资源
	}

}


//发送文件线程
//DWORD WINAPI transmmit(const LPVOID arg)
//{
//	//实际上这里也可以不加锁，上锁是为了方便看输出
//	m.lock();
//
//	F* temp = (F*)arg;
//	//获取文件的序号
//	//int file_id = temp->id;
//	//获取客户机的端口号
//	//ntohs(temp -> clientAddr.sin_port); 
//	cout << "测试开始,等待客户端发送消息..." << endl;
//	//从客户端处接受数据
//	/*
//	char Buffer[MAXBYTE] = { 0 }; //缓冲区
//	recv(temp->clientSocket, Buffer, MAXBYTE, 0); //recv方法 从客户端通过clientScocket接收
//	cout << "线程" << temp->id << "从客户端的" << ntohs(temp->clientAddr.sin_port) << "号端口收到:" << Buffer << endl;
//	*/
//	char file_name[100] = { 0 }; //文件路径
//	cout << "输入传输文件路径: ";
//	//cin >> file_name;
//	FILE* fp = fopen(file_name, "rb"); //将文件按二进制读取
//	if (fp == NULL)
//	{
//		cout << "文件" << file_name << "出错或不存在" << endl;
//	}
//	else
//	{
//		char Buffer[MAXBYTE] = { 0 }; //文件缓冲区
//		int size = 0; //读取的文件长度
//		//每次读取完之后清空缓存区，以便下一块文件读入
//		while ((size = fread(Buffer, sizeof(char), MAXBYTE, fp)) > 0)
//		{
//			//返回非0值表示send错误
//			if (send(temp->clientSocket, Buffer, size, NULL) < 0)
//			{
//				cout << "传输出错，请检查网络配置。" << endl;
//			}
//			memset(&Buffer, 0, MAXBYTE);
//		}
//		cout << temp->id << "线程已成功发送" << file_name << endl;
//		fclose(fp);
//	}
//
//
//	/*
//	//发送简单的字符串到客户端
//	const char* s = "Server file";
//	send(temp->clientSocket, s, strlen(s)*sizeof(char)+1, NULL);
//	cout << "线程" << temp->id << "通过客户端的" << ntohs(temp->clientAddr.sin_port) << "号端口发送:" << s << endl;
//	*/
//
//	m.unlock();
//
//	return 0;
//}
