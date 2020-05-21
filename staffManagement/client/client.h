#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int id;  //员工的编号
    char name[20];
    char passwd[20];
    char tips[50];  //用于传送信息内容
    int type;       // 0位root用户，1为普通用户
    char sex[10];
    int age;
    char phone[11];
    char addr[50];
} MSG;

int serverport;
int socketfd;
struct sockaddr_in serveraddr;
MSG msg;
int n;

void socket_init(char const* argv[]);

#endif