#pragma once
struct send_info
{
    char sendIp[32];//������ip

    int info_to; //������ID    0-��������1-1������...

    int type;//���͵���Ϣ����  0-���֣�1-ͼƬ��2-�ļ�

    char info_content[1024]; //��Ϣ����
};

