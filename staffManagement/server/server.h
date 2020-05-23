#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sqlite3.h>
#include <time.h>
#include <pthread.h>

#define DATABASE "staff.db"
#define N 1024
#define BACKLOG 5

typedef struct {
    int id;           //用户id
    char name[20];    //姓名
    char passwd[20];  //密码
    char tips[50];    //消息
    int type;         //用户类型
    char sex[10];     //性别
    int age;          //年龄
    char phone[12];   //电话
    char addr[50];    //住址
} MSG;

int serverport;
struct sockaddr_in serveraddr, clientaddr;
sqlite3* db;
socklen_t client_len;
int listenfd;
MSG comMsg;

int sqliteCallback(void* para, int f_number, char** f_value, char** f_name);
void socket_init(const char* argv[]);
int create_database(void);
void* do_client(void* arg);
void do_register(MSG* userMsg, int clientfd);
void do_findPassword(MSG* userMsg, int clientfd);
void do_login(MSG* userMsg, int clientfd);
void do_quit(MSG* userMsg, int clientfd);

void do_login_success(MSG* userMsg, int clientfd);
void do_add(MSG* userMsg, int clientfd);
void do_delete(MSG* userMsg, int clientfd);
void do_select(MSG* userMsg, int clientfd);
void do_updata(MSG* userMsg, int clientfd);
void do_passwd(MSG* userMsg, int clientfd);

#endif