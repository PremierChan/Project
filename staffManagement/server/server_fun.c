#include "server.h"

void socket_init(const char* argv[]) {
    serverport = atoi(argv[2]);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ((-1) == listenfd) {
        perror("Socket");
        exit(0);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverport);
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Bind");
        exit(0);
    }

    if (listen(listenfd, BACKLOG) < 0) {
        perror("Listen");
        exit(0);
    }

    puts("服务器准备完毕！");

    return;
}

int create_database(void) {
    char** dbResult;
    int nRow, nColumn;
    char* errmsg;
    char sql[1024];
    sqlite3* db;
    int number1 = 0, number2 = 0;

    if (!sqlite3_open(DATABASE, &db))

        sprintf(
            sql,
            "create table user_login (ID int,passwd char,type int,tips char);");
    number1 = !sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    sprintf(sql,
            "create table user_info (ID int,name char,sex char, age "
            "int, phone "
            "char, addr char, type int,tips char) ");
    number2 = !sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    if (number1 && number2) {
        puts("数据库表格初始化成功！");
    }

    return 1;
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
                printf("服务器错误！\n");
                break;
        }
    }

    return NULL;
}

/**
 *Name 		  : 	do_register
 *Description : 	注册用户信息，将用户登录信息添加到user_login表中，将
                                        用户信息添加到user_info表中
 *Input 	  :     用户信息（MSG）
 *Output 	  :
 */
void do_register(MSG* userMsg, int clientfd) {
    printf("do_reg\n");
    return;
}

/**
 *Name 		  : 	do_findPassword
 *Description : 	在用户忘记密码时提供密保找回密码功能
 *Input 	  : 	用户id,密保问题
 *Output 	  :
 */
void do_findPassword(MSG* userMsg, int clientfd) {
    puts("do_fnd");
    return;
}

/**
 *Name 		  : 	do_login
 *Description : 	判断用户输入的帐号与密码是否正确
 *Input 	  : 	用户id,密码
 *Output 	  :
 */
void do_login(MSG* userMsg, int clientfd) {
    puts("do_login");
    return;
}

void do_quit(MSG* userMsg, int clientfd) {
    free(userMsg);
    close(clientfd);
    printf("user quit\n");

    return;
}

#if 0

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

#endif