#include "Server.h"
#pragma warning(disable:4996)

void Server::Init() {

	RECV_TIMEOUT = 10;//������Ϣ��ʱ
	SEND_TIMEOUT = 10;//������Ϣ��ʱ
	total = 0;//��ǰ�Ѿ����ӵĿͷ��˷�����
	isLife[0] = true;//��ʾ����˴��
	//isLife.length()Ϊ�α���
	for (int i = 1; i < 10; i++) isLife[i] = false;
	setHostnameIp();

	//1����ʼ��socket��
	WSAStartup(MAKEWORD(2, 2), &wsaData);//MAKEWORD(���汾��, ���汾��)
	//2������socket
	servSock = socket(AF_INET, SOCK_STREAM, 0);//������·����ʽ�׽���

	//3������������ַ�����һ���ṹ������
	servAddr.sin_family = AF_INET;//�ͷ�������socketһ����sin_family��ʾЭ��أ�һ����AF_INET��ʾTCP/IPЭ�顣
	servAddr.sin_addr.S_un.S_addr = inet_addr(serverIp);//����˵�ַ����Ϊ���ػػ���ַ,192.168.125.155
	servAddr.sin_port = htons(12345);//host to net short �˿ں�����Ϊ12345
	//4���󶨷���˵�socket�ʹ���õĵ�ַ
	bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
	//4.5�������sokect��һ���¼������������տͻ������ӵ��¼�
	WSAEVENT servEvent = WSACreateEvent();//����һ���˹�����Ϊ���ŵ��¼�����
	WSAEventSelect(servSock, servEvent, FD_ALL_EVENTS);//���¼����󣬲��Ҽ��������¼�
	cliSock[0] = servSock;
	cliEvent[0] = servEvent;

	//5����������
	listen(servSock, 10);//�������г���Ϊ10

	//6��������ʼ��
	for (int i = 0; i < 10; i++) {
		sendTo[i] = 0;
		sendType[i] = 0;
	}

	//test
	sendTo[1] = 2;
}

//�ؼ����ġ������ܳ���
int Server::serchIp(char* ip) {
	for (int i = 2; i < total + 1; i++) {
		//if (!isLife[i]) continue;
		if (strcmp(inet_ntoa(cliAddr[i].sin_addr), ip) == 0) return i;
	}
	return -1;
}

//������������ip��
void Server::setHostnameIp() {
	WORD wVersionRequested;//WORD���ͱ��������ڴ��Winsock�汾��ֵ
	WSADATA wsaData;

	PHOSTENT hostinfo;
	wVersionRequested = MAKEWORD(2, 0);
	//����MAKEWORD()�������Winsock�İ汾�����ڼ���Winsock��
	if (WSAStartup(wVersionRequested, &wsaData) == 0)
	{
		//����Winsock�⣬���WSAStartup()�����ķ���ֵΪ0��˵�����سɹ�
		if (gethostname(serverName, sizeof(serverName)) == 0)
		{
			//�ж��Ƿ�ɹ��Ľ������������������name����ָ���Ļ�������
			if ((hostinfo = gethostbyname(serverName)) != NULL)
			{
				//�������������ɹ��Ļ�������inet_ntoa()����ȡ��IP��ַ
				serverIp = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);
				cout << "���Ƿ����\n";
				printf("������IP��ַ�ǣ�%s\n", serverIp);//���IP��ַ 
				printf("�����������ǣ�%s\n\n\n", serverName);
			}
		}
		WSACleanup();//ж��Winsock�⣬���ͷ�������Դ
	}

}

//Ⱥ����Ϣ����
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

DWORD WINAPI servEventThread( LPVOID tmp) //���������߳�
{
	Server s =*(Server*) tmp;
	//���̸߳��������˺͸����ͻ��˷������¼�
	//������Ĳ�����ʼ��
	SOCKET servSock = s.servSock;//LPVOIDΪ��ָ�����ͣ���Ҫ��ת��SOCKET���������ã�����ʹ�ô����SOCKET
	while (1) //��ִͣ��
	{
		for (int i = 0; i < s.total + 1; i++)//i�����������ڼ����¼����ն�
		{
			if (!s.isLife[i]) continue;

			//����һ���ͻ������ӣ�total==1��ѭ�����Σ������ͻ��˺ͷ����
			//��ÿһ���նˣ��ͻ��˺ͷ���ˣ����鿴�Ƿ����¼����ȴ�WAIT_TIME����
			int index = WSAWaitForMultipleEvents(1, &s.cliEvent[i], false, s.WAIT_TIME, 0);

			index -= WSA_WAIT_EVENT_0;//��ʱindexΪ�����¼����ն��±�

			if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
			{
				continue;//���������߳�ʱ�����������ն�
			}



			else if (index == 0)
			{
				WSANETWORKEVENTS networkEvents;
				WSAEnumNetworkEvents(s.cliSock[i], s.cliEvent[i], &networkEvents);//�鿴��ʲô�¼�

				//�¼�ѡ��
				if (networkEvents.lNetworkEvents & FD_ACCEPT)//������accept�¼����˴���λ�������룩
				{
					if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						cout << "����ʱ�������󣬴������" << networkEvents.iErrorCode[FD_ACCEPT_BIT] << endl;
						continue;
					}
					//��������
					if (s.total + 1 < s.MAX_LINK_NUM)//������һ���ͻ�����ȻС�����������������ܸ�����
					{

						//totalΪ���������ܿͻ�������
						int nextIndex = s.total + 1;//������¿ͻ��˵��±�
						int addrLen = sizeof(SOCKADDR);
						SOCKET newSock = accept(servSock, (SOCKADDR*)&s.cliAddr[nextIndex], &addrLen);

						//auto client = inet_ntoa(s.cliAddr[nextIndex].sin_addr);
						//��ȡ�ͻ���IP
						int tmp = s.serchIp(inet_ntoa(s.cliAddr[nextIndex].sin_addr));
						if (tmp != -1) {
							int tmp1 = nextIndex;
							nextIndex = tmp;
							s.cliAddr[nextIndex] = s.cliAddr[tmp1];
							s.cliAddr[tmp1] = s.cliAddr[tmp1 + 1];
							//����������Ϣ
							//\n
							char offLineMsg[512] = "�������һ����Ϣ��";
							strcat(offLineMsg, s.offLineMsg[nextIndex]);
							/*send(s.cliSock[nextIndex], "�������һ����Ϣ��", sizeof("�������һ����Ϣ��"), 0);
							send(s.cliSock[nextIndex], s.offLineMsg[nextIndex], sizeof(s.offLineMsg[nextIndex]), 0);*/
							send(s.cliSock[nextIndex], offLineMsg, 1024,0);
						}
						else s.total++;//�ͻ�������������

						//�洢ip��ַ
						strcpy(s.clientIp[nextIndex], inet_ntoa(s.cliAddr[i].sin_addr));

						if (newSock != INVALID_SOCKET)
						{
							//���÷��ͺͽ���ʱ��
							/*setsockopt(newSock, SOL_SOCKET, SO_SNDTIMEO, (const char*) & SEND_TIMEOUT, sizeof(SEND_TIMEOUT));
							setsockopt(newSock, SOL_SOCKET, SO_SNDTIMEO, (const char*) &RECV_TIMEOUT, sizeof(RECV_TIMEOUT));*/
							//���¿ͻ��˷���socket
							s.cliSock[nextIndex] = newSock;
							s.life++;//���ͻ�������
							s.isLife[nextIndex] = true;
							//�¿ͻ��˵ĵ�ַ�Ѿ�����cliAddr[nextIndex]����
							//Ϊ�¿ͻ��˰��¼�����,ͬʱ���ü�����close��read��write
							WSAEVENT newEvent = WSACreateEvent();
							WSAEventSelect(s.cliSock[nextIndex], newEvent, FD_CLOSE | FD_READ | FD_WRITE);
							s.cliEvent[nextIndex] = newEvent;
							//�������ʾ
							cout << "#" << nextIndex << "�ο� "<< nextIndex <<"��IP��" << inet_ntoa(s.cliAddr[nextIndex].sin_addr) << ")���ߣ���ǰ��������" << s.life << endl;

							//��֪�ͻ����Ǽ���
							char tellCli[128] = "�ͻ��� ";
							char charTellCli[20];
							itoa(nextIndex, charTellCli, 10);
							strcat(tellCli, charTellCli);
							strcat(tellCli, " ��");

							send(s.cliSock[nextIndex], tellCli, sizeof(tellCli), 0);
							//memset(&buf, 0, MAXBYTE);
							// 
							//�����пͻ��˷��ͻ�ӭ��Ϣ
							char buf[BUFFER_SIZE] = "[Server]welcome ";
							char charNextIndex[20];
							itoa(nextIndex, charNextIndex, 10);

							strcat(buf, "number"); strcat(buf, charNextIndex);
							strcat(buf, "��IP��");
							strcat(buf, inet_ntoa(s.cliAddr[nextIndex].sin_addr));
							strcat(buf, ")online");

							//s.sendToAll(buf);	//��������������������������ȷ����buf-size
							for (int j = 1; j <= s.total; j++)
							{
								//cout << sizeof(buf) << endl;
								if (s.isLife[j])send(s.cliSock[j], buf, 1024, 0);
							}
						}


						//����������Ϣ
							//\n
						if (tmp != -1){
							char offLine[512] = "�������һ����Ϣ��\n";
							strcat(offLine, s.offLineMsg[nextIndex]);
							/*send(s.cliSock[nextIndex], "�������һ����Ϣ��", sizeof("�������һ����Ϣ��"), 0);
							send(s.cliSock[nextIndex], s.offLineMsg[nextIndex], sizeof(s.offLineMsg[nextIndex]), 0);*/
							send(s.cliSock[nextIndex], offLine, 1024, 0);
						}

					}

				}





				else if (networkEvents.lNetworkEvents & FD_CLOSE)//�ͻ��˱��رգ����Ͽ�����
				{

					//i��ʾ�ѹرյĿͻ����±�
					s.life--;
					cout << "#" << i << "�οͣ�IP��" << inet_ntoa(s.cliAddr[i].sin_addr) << ")�˳���������,��ǰ��������" << s.life << endl;
					//�ͷ�����ͻ��˵���Դ
					closesocket(s.cliSock[i]);
					WSACloseEvent(s.cliEvent[i]);

					////�������,��˳���ɾ��Ԫ��
					//for (int j = i; j < total; j++)
					//{
						//s.cliSock[j] = s.cliSock[j + 1];
						//s.cliEvent[j] = s.cliEvent[j + 1];
						//s.cliAddr[j] = s.cliAddr[j + 1];
					//}
					//�����пͻ��˷����˳������ҵ���Ϣ
					char buf[BUFFER_SIZE] = "[Server]��IP��";
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





				else if (networkEvents.lNetworkEvents & FD_READ)//���յ���Ϣ
				{

					char buffer[BUFFER_SIZE] = { 0 };//�ַ������������ڽ��պͷ�����Ϣ
					char buffer2[BUFFER_SIZE] = { 0 };

					//for (int j = 1; j <= s.total; j++)
					//{
					int nrecv = recv(s.cliSock[i], buffer, sizeof(buffer), 0);//nrecv�ǽ��յ����ֽ���
					if (nrecv > 0)//������յ����ַ�������0
					{
						sprintf(buffer2, "[#%d]%s", i, buffer);
						//�ڷ������ʾ
						cout << buffer2 << endl;
						if (strcmp(buffer, "#file")==0) {
							s.sendType[i] = 1;
							//����
							send(s.cliSock[s.sendTo[i]], buffer, sizeof(buffer), 0);
							memset(&buffer, 0, BUFFER_SIZE);

							/*do {
								nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
								cout << "���ڽ����ļ�\n";
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
							cout << "�ļ��������\n";
						}

						else if (strcmp(buffer, "#photo")==0) {
							s.sendType[i] = 2;
							//����
							send(s.cliSock[s.sendTo[i]], buffer, sizeof(buffer), 0);
							memset(&buffer, 0, BUFFER_SIZE);

							/*do {
								nCount = recv(c.cliSock, buffer, BUFFER_SIZE, 0);
								cout << "���ڽ����ļ�\n";
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
							cout << "�ļ��������\n";
						}


						else if (strcmp(buffer, "#people")==0) {
							memset(&buffer, 0, MAXBYTE);
							strcat(buffer, "���ߵ������У�\n");
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
								//������Ϣ�洢
								else {
									strcpy(s.offLineMsg[s.sendTo[i]], buffer2);
								}
							}
							else {
								for (int k = 1; k <= s.total; k++)
									send(s.cliSock[k], buffer2, sizeof(buffer2), 0);
							}
						//}

						////�������ͻ�����ʾ��'��'���������ͻ��ˣ�
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