#pragma once
struct Message
{
    char sendIp[32];//������ip

    char info_to[1]; //������ID    0-��������1-1������...

    char type[1];//���͵���Ϣ����  0-���֣�1-ͼƬ��2-�ļ�

    char info_content[1024]; //��Ϣ����
};


