#include"Client.h"
#include <sys/stat.h> // struct stat
int main() {
	Client c;
	c.Init();

	//����������Ϣ�߳�
	CloseHandle(CreateThread(NULL, 0, recvMsgThread, (LPVOID)&c, 0, 0));
	//���߳���������Ҫ���͵���Ϣ
	while (1)
	{
		if (c.sendType == 0) {
			char buf[BUFFER_SIZE] = { 0 };
			cin.getline(buf, sizeof(buf));
			//if (strcmp(buf, "quit") == 0)//�����롰quit�������˳�������
			//{
			//	break;
			//}
			//�����ļ�
			if (strcmp(buf, "#file") == 0) {
				c.sendType = 1;
				send(c.cliSock, buf, BUFFER_SIZE, 0);

				char buf[BUFFER_SIZE] = { 0 };
				int nCount;

				FILE* fp = fopen(c.filename, "rt");
				struct _stat info;

				_stat(c.filename, &info);

				int size = info.st_size;

				/*cout << "�ļ���С��"<< size << endl;
				char charSize[1024];
				snprintf(charSize, sizeof(charSize), "%d", size);
				send(c.cliSock, charSize, 1024, 0);*/

				while ((nCount = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
					cout << "���ڷ����ļ�" << endl;
					cout << buf;
					send(c.cliSock, buf, BUFFER_SIZE, 0);
				}
				cout << "�������\n";
				//send(c.cliSock, "#end", sizeof("#end"), 0);
				c.sendType = 0;
				continue;
			}
			//����ͼƬ
			else if (strcmp(buf, "#photo") == 0) {
				c.sendType = 1;
				send(c.cliSock, buf, BUFFER_SIZE, 0);

				char buf[BUFFER_SIZE] = { 0 };
				int nCount;

				//�����ļ���С
				FILE* fp = fopen(c.photoname, "rb");
				struct _stat info;

				_stat(c.photoname, &info);

				int size = info.st_size;

				/*cout << "�ļ���С��"<< size << endl;
				char charSize[1024];
				snprintf(charSize, sizeof(charSize), "%d", size);
				send(c.cliSock, charSize, 1024, 0);*/

				while ((nCount = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
					cout << "���ڷ����ļ�" << endl;
					cout << buf;
					send(c.cliSock, buf, BUFFER_SIZE, 0);
				}
				cout << "�������\n";
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
		//���ļ�

	}
	closesocket(c.cliSock);
	WSACleanup();
	return 0;
}