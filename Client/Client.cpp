#include "Client.h"
#include"getFileSize.cpp"

void Client::Init() {
	setHostnameIp();

	//1����ʼ��socket��
	WSADATA wsaData;//��ȡ�汾��Ϣ��˵��Ҫʹ�õİ汾
	WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(���汾��, ���汾��)

	//2������socket
	cliSock = socket(AF_INET, SOCK_STREAM, 0);//������·����ʽ�׽���,���������������Զ�ѡ��Э��

	//3�������ַ
	//�ͻ���
	cliAddr = { 0 };
	cliAddr.sin_family = AF_INET;
	auto k = inet_addr("127.0.0.1");
	//cliAddr.sin_addr.s_addr = inet_addr("127.0.0.3");//IP��ַ
	cliAddr.sin_port = htons(12345);//�˿ں�
	//�����
	servAddr = { 0 };
	servAddr.sin_family = AF_INET;//�ͷ�������socketһ����sin_family��ʾЭ��أ�һ����AF_INET��ʾTCP/IPЭ�顣
	servAddr.sin_addr.S_un.S_addr = inet_addr("172.20.127.28");//����˵�ַ����Ϊ��ʵ��ַ172.20.127.28  192.168.125.155
	servAddr.sin_port = htons(12345);//host to net short �˿ں�����Ϊ12345

	if (connect(cliSock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "���ӳ��ִ��󣬴������" << WSAGetLastError() << endl;
	}

}

void Client::sendMsg() {
	//while (1)
	//{
	//	if (sendType == 0) {
	//		char buf[BUFFER_SIZE] = { 0 };
	//		cin.getline(buf, sizeof(buf));
	//		//if (strcmp(buf, "quit") == 0)//�����롰quit�������˳�������
	//		//{
	//		//	break;
	//		//}
	//		//ǰ������'#'�ŵĶ���������Ϣ
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
	//	//���ļ�-ͼƬ
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



DWORD WINAPI recvMsgThread(LPVOID IpParameter)//������Ϣ���߳�
{
	Client c = *(Client*)IpParameter;//��ȡ�ͻ��˵�SOCKET����

	while (1)
	{
		char buffer[BUFFER_SIZE] = { 0 };//�ַ������������ڽ��պͷ�����Ϣ
		if (c.recvType == 0) {
			int nrecv = recv(c.cliSock, buffer, sizeof(buffer), 0);//nrecv�ǽ��յ����ֽ���
			if (strcmp(buffer, "#file") == 0) {
				c.recvType = 1;
				//continue;
			//���ļ�
			//else {
				int nCount;
				memset(&buffer, 0, BUFFER_SIZE);
				FILE* fp = fopen(c.filename, "wt");
				do {
					char buffer[BUFFER_SIZE] = { 0 };
					nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
					cout << "���ڽ����ļ�\n";
					//if (strcmp(buffer, "#end")) break;
					cout << buffer;
					fwrite(buffer, nCount, 1, fp);
					memset(&buffer, 0, BUFFER_SIZE);
				} while (BUFFER_SIZE == sizeof(nCount));
				fclose(fp);
				cout << "�������ļ�\n";
				c.recvType = 0;
				//}
			}
			else if (strcmp(buffer, "#photo") == 0) {
				c.recvType = 1;
				//continue;
			//���ļ�
			//else {
				int nCount;
				memset(&buffer, 0, BUFFER_SIZE);
				FILE* fp = fopen(c.photoname, "wb+");
				do {
					char buffer[BUFFER_SIZE] = { 0 };
					nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
					cout << "���ڽ����ļ�\n";
					//if (strcmp(buffer, "#end")) break;
					cout << buffer;
					fwrite(buffer, nCount, 1, fp);
					memset(&buffer, 0, BUFFER_SIZE);
				} while (BUFFER_SIZE == sizeof(nCount));
				fclose(fp);
				//int filesize = file_size2(c.photoname);
				cout << "�������ļ����ļ���С��";// << filesize << endl;
				c.recvType = 0;
				//}
			}
			if (nrecv > 0)//������յ����ַ�������0
			{
				cout << buffer << endl;
			}
			else if (nrecv < 0)//������յ����ַ���С��0��˵���Ͽ�����
			{
				cout << "��������Ͽ�����" << endl;
				break;
			}
		}



	}
	return 0;
}
void Client::setHostnameIp() {
	WORD wVersionRequested;//WORD���ͱ��������ڴ��Winsock�汾��ֵ
	WSADATA wsaData;

	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);
	//����MAKEWORD()�������Winsock�İ汾�����ڼ���Winsock��
	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		//����Winsock�⣬���WSAStartup()�����ķ���ֵΪ0��˵�����سɹ�
		if (gethostname(name, sizeof(name)) == 0)
		{
			//�ж��Ƿ�ɹ��Ľ������������������name����ָ���Ļ�������
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				//�������������ɹ��Ļ�������inet_ntoa()����ȡ��IP��ַ
				ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				printf("������IP��ַ�ǣ�%s\n", ip);//���IP��ַ 
				printf("�����������ǣ�%s\n", name);
			}
		}
		WSACleanup();//ж��Winsock�⣬���ͷ�������Դ
	}

}


//�����ļ��߳�
//DWORD WINAPI transmmit(const LPVOID arg)
//{
//	//ʵ��������Ҳ���Բ�������������Ϊ�˷��㿴���
//	m.lock();
//
//	F* temp = (F*)arg;
//	//��ȡ�ļ������
//	//int file_id = temp->id;
//	//��ȡ�ͻ����Ķ˿ں�
//	//ntohs(temp -> clientAddr.sin_port); 
//	cout << "���Կ�ʼ,�ȴ��ͻ��˷�����Ϣ..." << endl;
//	//�ӿͻ��˴���������
//	/*
//	char Buffer[MAXBYTE] = { 0 }; //������
//	recv(temp->clientSocket, Buffer, MAXBYTE, 0); //recv���� �ӿͻ���ͨ��clientScocket����
//	cout << "�߳�" << temp->id << "�ӿͻ��˵�" << ntohs(temp->clientAddr.sin_port) << "�Ŷ˿��յ�:" << Buffer << endl;
//	*/
//	char file_name[100] = { 0 }; //�ļ�·��
//	cout << "���봫���ļ�·��: ";
//	//cin >> file_name;
//	FILE* fp = fopen(file_name, "rb"); //���ļ��������ƶ�ȡ
//	if (fp == NULL)
//	{
//		cout << "�ļ�" << file_name << "����򲻴���" << endl;
//	}
//	else
//	{
//		char Buffer[MAXBYTE] = { 0 }; //�ļ�������
//		int size = 0; //��ȡ���ļ�����
//		//ÿ�ζ�ȡ��֮����ջ��������Ա���һ���ļ�����
//		while ((size = fread(Buffer, sizeof(char), MAXBYTE, fp)) > 0)
//		{
//			//���ط�0ֵ��ʾsend����
//			if (send(temp->clientSocket, Buffer, size, NULL) < 0)
//			{
//				cout << "������������������á�" << endl;
//			}
//			memset(&Buffer, 0, MAXBYTE);
//		}
//		cout << temp->id << "�߳��ѳɹ�����" << file_name << endl;
//		fclose(fp);
//	}
//
//
//	/*
//	//���ͼ򵥵��ַ������ͻ���
//	const char* s = "Server file";
//	send(temp->clientSocket, s, strlen(s)*sizeof(char)+1, NULL);
//	cout << "�߳�" << temp->id << "ͨ���ͻ��˵�" << ntohs(temp->clientAddr.sin_port) << "�Ŷ˿ڷ���:" << s << endl;
//	*/
//
//	m.unlock();
//
//	return 0;
//}
