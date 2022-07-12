#pragma once
struct Message
{
    char sendIp[32];//发送者ip

    char info_to[1]; //接收者ID    0-服务器，1-1号主机...

    char type[1];//发送的消息类型  0-文字，1-图片，2-文件

    char info_content[1024]; //消息主体
};


