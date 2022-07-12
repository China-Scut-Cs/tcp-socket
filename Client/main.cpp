#include"Client.h"
#include <sys/stat.h> // struct stat
int main() {
	Client c;
	c.Init();

	//创建接受消息线程
	CloseHandle(CreateThread(NULL, 0, recvMsgThread, (LPVOID)&c, 0, 0));
	//主线程用于输入要发送的消息
	while (1)
	{
		if (c.sendType == 0) {
			char buf[BUFFER_SIZE] = { 0 };
			cin.getline(buf, sizeof(buf));
			//if (strcmp(buf, "quit") == 0)//若输入“quit”，则退出聊天室
			//{
			//	break;
			//}
			//发送文件
			if (strcmp(buf, "#file") == 0) {
				c.sendType = 1;
				send(c.cliSock, buf, BUFFER_SIZE, 0);

				char buf[BUFFER_SIZE] = { 0 };
				int nCount;

				FILE* fp = fopen(c.filename, "rt");
				struct _stat info;

				_stat(c.filename, &info);

				int size = info.st_size;

				/*cout << "文件大小："<< size << endl;
				char charSize[1024];
				snprintf(charSize, sizeof(charSize), "%d", size);
				send(c.cliSock, charSize, 1024, 0);*/

				while ((nCount = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
					cout << "正在发送文件" << endl;
					cout << buf;
					send(c.cliSock, buf, BUFFER_SIZE, 0);
				}
				cout << "传输完毕\n";
				//send(c.cliSock, "#end", sizeof("#end"), 0);
				c.sendType = 0;
				continue;
			}
			//发送图片
			else if (strcmp(buf, "#photo") == 0) {
				c.sendType = 1;
				send(c.cliSock, buf, BUFFER_SIZE, 0);

				char buf[BUFFER_SIZE] = { 0 };
				int nCount;

				//计算文件大小
				FILE* fp = fopen(c.photoname, "rb");
				struct _stat info;

				_stat(c.photoname, &info);

				int size = info.st_size;

				/*cout << "文件大小："<< size << endl;
				char charSize[1024];
				snprintf(charSize, sizeof(charSize), "%d", size);
				send(c.cliSock, charSize, 1024, 0);*/

				while ((nCount = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
					cout << "正在发送文件" << endl;
					cout << buf;
					send(c.cliSock, buf, BUFFER_SIZE, 0);
				}
				cout << "传输完毕\n";
				//send(c.cliSock, "#end", sizeof("#end"), 0);
				c.sendType = 0;
				continue;
			}

			else if (strcmp(buf, "#people") == 0) {
				send(c.cliSock, buf, BUFFER_SIZE, 0);
			}
			else {
				strcat(buf, "     --by ");
				strcat(buf, c.ip);
				send(c.cliSock, buf, BUFFER_SIZE, 0);
			}
		}
		//发文件

	}
	closesocket(c.cliSock);
	WSACleanup();
	return 0;
}