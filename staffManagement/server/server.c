
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

typedef struct {
    int id;           //用户id
    char name[20];    //姓名
    char passwd[20];  //密码
    char tips[50];    //消息
    int type;         //用户类型
    char sex[10];     //性别
    int age;          //年龄
    char phone[11];   //电话
    char addr[50];    //住址
} MSG;

struct sockaddr_in serveraddr, clientaddr;
sqlite3* db;
socklen_t client_len;
int listenfd;

/**
 *Name 		  : 	socket_init
 *Description : 	初始化网络，为客户端连接做准备, 初始化监听套接字
 *listenfd Input 	  : Output 	  :
 */
void socket_init(const char* argv[]) {
        listenfd = socket(AF_INET, SOCK_ST

	serveraddr.sin_family = AF_INET;
	
	//绑定服务器端口

	//服务器开启监听，等待客服段连接

	return;
}

/**
 *Name 		  : 	create_database
 *Description : 	创建数据库
 *Input 	  :
 *Output 	  :
 */
int create_database() {
    char** dbResult;
    int nRow, nColumn;
    char* errmsg;
    char sql[1024];
    printf("create_database!\n");

    //打开数据库，如果数据库不存在则创建
    sqlite3_open(...);

    sprintf(sql,
            "create table user_login ....));");  // 为数据库创建表 user_login
    printf("create_database!\n");
    sqlite3_exec(db, sql, NULL, NULL, &errmsg)  // 执行sql 语句

        printf("create_database!\n");
    sprintf(sql, "create table user_info ....");  // 为数据库创建表 user_info
    sqlite3_exec(db, sql, NULL, NULL, &errmsg)  // 执行sql 语句

        // 确保 user_login 表中有 root 用户  密码 root
        // ..
        sprintf(sql, "select * from user_login where name = 'root';");
    // ..
    sprintf(sql,
            "insert into user_login values(null,'root','root',0, 'root');");

    return 1;
}

void do_quit(MSG* userMsg, int clientfd) {
    free(userMsg);
    close(clientfd);
    printf("user quit\n");

    return;
}

/**
 *Name 		  : 	do_register
 *Description : 	注册用户信息，将用户登录信息添加到user_login表中，将
                                        用户信息添加到user_info表中
 *Input 	  :     用户信息（MSG）
 *Output 	  :
 */
void do_register(MSG* userMsg, int clientfd) {
    return;
}

void do_add(MSG* userMsg, int clientfd) {
    do_register(userMsg, clientfd);
}

/**
 *Name 		  : 	do_delete
 *Description :     根据用户的id删除用户记录
 *Input 	  : 	用户id(msg->id)
 *Output 	  :
 */
void do_delete(MSG* userMsg, int clientfd) {
    return;
}

/**
 *Name 		  : 	do_select
 *Description : 	根据用户id查询某个用户的信息
 *Input 	  : 	用户id
 *Output 	  :
 */
void do_select(MSG* userMsg, int clientfd) {
    return;
}

/**
 *Name 		  : 	do_updata
 *Description : 	管理员根据用户id修改用户信息
 *Input 	  : 	用户id
 *Output 	  :
 */
void do_updata(MSG* userMsg, int clientfd) {
    return;
}

void do_passwd(MSG* userMsg, int clientfd) {
    return;
}

void do_login_success(MSG* userMsg, int clientfd) {
    while (1) {
        if (recv(clientfd, userMsg, sizeof(*userMsg), 0) < 0) {
        }

        switch (userMsg->tips[0]) {
            case 'A':
                do_add(userMsg, clientfd);
                break;
            case 'D':
                do_delete(userMsg, clientfd);
                break;
            case 'U':
                do_updata(userMsg, clientfd);
                break;
            case 'S':
                do_select(userMsg, clientfd);
                break;
            case 'P':
                do_passwd(userMsg, clientfd);
                return;
                break;
            case 'Q':
                return;
                break;
            default:
                break;
        }
    }

    return;
}

/**
 *Name 		  : 	do_login
 *Description : 	判断用户输入的帐号与密码是否正确
 *Input 	  : 	用户id,密码
 *Output 	  :
 */
void do_login(MSG* userMsg, int clientfd) {
    return;
}

/**
 *Name 		  : 	do_findPassword
 *Description : 	在用户忘记密码时提供密保找回密码功能
 *Input 	  : 	用户id,密保问题
 *Output 	  :
 */
void do_findPassword(MSG* userMsg, int clientfd) {
    return;
}

void* do_client(void* arg) {
    int length;
    int clientfd = *(int*)arg;
    MSG* userMsg = (MSG*)malloc(sizeof(MSG));

    while (1) {
        if ((length = recv(clientfd, userMsg, sizeof(*userMsg), 0)) < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
        }

        printf("%c\n", userMsg->tips[0]);
        switch (userMsg->tips[0]) {
            case 'R':
                do_register(userMsg, clientfd);
                break;
            case 'F':
                do_findPassword(userMsg, clientfd);
                break;
            case 'L':
                do_login(userMsg, clientfd);
                break;
            case 'Q':
                do_quit(userMsg, clientfd);
                return NULL;
                break;
            default:
                printf("Server Error\n");
                break;
        }
    }

    return NULL;
}

int main(int argc, const char* argv[]) {
    pthread_t tid;  //线程id

    if (argc < 3) {
        fprintf(stderr, "Usage :%s  <valid>  <valid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    create_database();  //创建数据库
    socket_init(argv);
    while (1) {
        int clientfd;

        //连接客服端
                if ((clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &client_len)) < 0
		
		// 为每个 链接进来的 客户端创建一个 线程 
		if (pthread_create(&tid, NULL, do_client, (void *)&clientfd))
    }

    close(listenfd);

    return 0;
}
