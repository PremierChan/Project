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
    int number1 = 0, number2 = 0;

    sqlite3_open(DATABASE, &db);

    sprintf(sql,
            "create table user_login (ID int,passwd char,type int,tips char);");
    number1 = !sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    sprintf(sql,
            "create table user_info (ID int,name char,sex char, age "
            "int, phone "
            "char, addr char, type int,tips char) ");
    number2 = !sqlite3_exec(db, sql, NULL, NULL, &errmsg);

    if (number1 && number2) {
        sprintf(sql,
                "INSERT INTO user_info "
                "VALUES(1000,\"PremierChan\",\"男\",99,\"110\",\"NULL\",0,"
                "\"A\");");
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);

        sprintf(sql,
                " INSERT INTO user_login VALUES (1000,\"asdfghjkl\",0,\"a\");");
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);

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

void do_register(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    printf("用户注册\n");

    recv(clientfd, userMsg, sizeof(*userMsg), 0);
    userMsg->id = comMsg.id + 1;
    userMsg->type = 1;
    strcpy(userMsg->tips, "A");

    sprintf(registerInfoSql,  //登录信息录入
            "insert into user_login values (%d,\"%s\",%d,\"%s\");", userMsg->id,
            userMsg->passwd, userMsg->type, userMsg->tips);
    sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

    sprintf(registerInfoSql,  //个人信息录入
            "insert into user_info values "
            "(%d,\"%s\",\"%s\",%d,\"%s\",\"%s\",%d,\"%s\");",
            userMsg->id, userMsg->name, userMsg->sex, userMsg->age,
            userMsg->phone, userMsg->addr, userMsg->type, userMsg->tips);
    sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

    send(clientfd, userMsg, sizeof(*userMsg), 0);

    printf("注册完成\n");
    memset(&comMsg, 0, sizeof(comMsg));

    return;
}

int sqliteCallback(void* para, int f_number, char* f_value[], char* f_name[]) {
    int i = 0;
    if (f_number == 4) {
        comMsg.id = atoi(f_value[i++]);
        strcpy(comMsg.passwd, f_value[i++]);
        comMsg.type = atoi(f_value[i++]);
        strcpy(comMsg.tips, f_value[i]);
    } else {
        comMsg.id = atoi(f_value[i++]);
        strcpy(comMsg.name, f_value[i++]);
        stpcpy(comMsg.sex, f_value[i++]);
        comMsg.age = atoi(f_value[i++]);
        strcpy(comMsg.phone, f_value[i++]);
        strcpy(comMsg.addr, f_value[i++]);
        comMsg.type = atoi(f_value[i++]);
        strcpy(comMsg.tips, f_value[i]);
    }
}

/**
 *Name 		  : 	do_findPassword
 *Description : 	在用户忘记密码时提供密保找回密码功能
 *Input 	  : 	用户id,密保问题
 *Output 	  :
 */
void do_findPassword(MSG* userMsg, int clientfd) {
    recv(clientfd, userMsg, sizeof(*userMsg), 0);
    printf("%d\n", userMsg->id);
    getchar();
    return;
}

void do_login(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if (userMsg->id > comMsg.id)
        goto ERROR;

    sprintf(registerInfoSql, "select * from user_login where id=%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if (strcmp(userMsg->passwd, comMsg.passwd))
        goto ERROR;

    sprintf(registerInfoSql, "select * from user_info where id=%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    strcpy(userMsg->tips, "A");
    send(clientfd, &comMsg, sizeof(comMsg), 0);

    memset(&comMsg, 0, sizeof(comMsg));

    do_login_success(userMsg, clientfd);

    return;

ERROR:
    strcpy(userMsg->tips, "#");
    send(clientfd, userMsg, sizeof(*userMsg), 0);
    memset(&comMsg, 0, sizeof(comMsg));
    return;
}

void do_quit(MSG* userMsg, int clientfd) {
    free(userMsg);
    close(clientfd);
    printf("用户退出\n");

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
    puts("do_sele");
    return;
}

/**
 *Name 		  : 	do_updata
 *Description : 	管理员根据用户id修改用户信息
 *Input 	  : 	用户id
 *Output 	  :
 */
void do_updata(MSG* userMsg, int clientfd) {
    puts("do_up");
    return;
}

void do_passwd(MSG* userMsg, int clientfd) {
    recv(clientfd, userMsg, sizeof(*userMsg), 0);
    printf("%d\n", userMsg->id);
    getchar();
    return;
}
