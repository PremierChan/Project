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
                "\"CAB\");");
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);

        sprintf(
            sql,
            " INSERT INTO user_login VALUES (1000,\"asdfghjkl\",0,\"CAB\");");
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

        printf("\n[%c]\n", userMsg->tips[0]);
        switch (userMsg->tips[0]) {
            case 'R':
                memset(userMsg, 0, sizeof(*userMsg));
                do_register(userMsg, clientfd);
                break;
            case 'F':
                memset(userMsg, 0, sizeof(*userMsg));
                do_findPassword(userMsg, clientfd);
                break;
            case 'L':
                memset(userMsg, 0, sizeof(*userMsg));
                do_login(userMsg, clientfd);
                break;
            case 'Q':
                memset(userMsg, 0, sizeof(*userMsg));
                do_quit(userMsg, clientfd);
                return NULL;
                break;
            default:
                printf("服务器错误\n");
                return NULL;
                break;
        }
    }

    return NULL;
}

void do_register(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];
    int users;
    int flag;

    printf("用户注册\n");

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
    users = comMsg.id;

    strcpy(userMsg->name, "");
    strcpy(userMsg->phone, "");
    strcpy(userMsg->addr, "");
    strcpy(userMsg->sex, "");

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    if ((!strlen(userMsg->name)) && (!strlen(userMsg->phone)) &&
        (!strlen(userMsg->sex)) && (!strlen(userMsg->addr))) {
        printf("\n注册失败，客户端退出\n");
        return;
    }

    userMsg->type = 1;

    for (int i = 1000; i <= users; i++) {
        sprintf(registerInfoSql, "select * from user_login where id = %d;", i);
        flag = sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
        if (!flag) {
            userMsg->id = i;
            break;
        }
        if (i == users) {
            userMsg->id = comMsg.id + 1;
            break;
        }
    }

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

    backInfo(userMsg, clientfd);

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

void do_findPassword(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];
    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    printf("用户 %d 正在找回密码\n", userMsg->id);

    sprintf(registerInfoSql, "select * from user_info where id =%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if (strcmp(comMsg.name, userMsg->name) ||
        strcmp(comMsg.phone, userMsg->phone) ||
        strcmp(comMsg.tips, userMsg->tips)) {
        puts("验证失败\n");
        strcpy(comMsg.tips, "#");

        send(clientfd, &comMsg, sizeof(comMsg), 0);
    } else {
        puts("验证成功\n");

        sprintf(registerInfoSql, "select * from user_login where id =%d;",
                userMsg->id);
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

        send(clientfd, &comMsg, sizeof(comMsg), 0);
    }

    memset(&comMsg, 0, sizeof(comMsg));
    return;
}

void do_login(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];
    int number_error;

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if (userMsg->id > comMsg.id)
        goto ERROR;

    sprintf(registerInfoSql, "select * from user_login where id=%d;",
            userMsg->id);
    number_error =
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if (strcmp(userMsg->passwd, comMsg.passwd) || (!number_error))
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
    userNumber = userMsg->id;
    while (1) {
        if (recv(clientfd, userMsg, sizeof(*userMsg), 0) < 0) {
        }

        printf("\n%d : [L]--->[%C]\n", userNumber, userMsg->tips[0]);
        time(&t);
        printf("%s\n", ctime(&t));
        switch (userMsg->tips[0]) {
            case 'A':
                do_add(userMsg, clientfd);
                break;
            case 'D':
                do_delete(userMsg, clientfd);
                break;
            case 'U':
                do_updata(userMsg, clientfd);
                return;
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

void do_delete(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    printf("删除用户\n");

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    if ((userMsg->id > comMsg.id) || (userMsg->id < 1000)) {
        printf("删除失败\n");
        strcpy(userMsg->tips, "@");
        send(clientfd, userMsg, sizeof(*userMsg), 0);
        return;
    }

    sprintf(registerInfoSql, "select * from user_info where id =%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    send(clientfd, &comMsg, sizeof(comMsg), 0);

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    printf("user id = %d\n", userMsg->id);

    sprintf(registerInfoSql, "delete from user_info where id =%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

    sprintf(registerInfoSql, "delete from user_login where id =%d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

    printf("删除结束");

    backInfo(userMsg, clientfd);

    return;
}

void do_select(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];
    char* errmsg;
    int number_erro;
    int allUser;
    int allVoidNumber = 0;

    sprintf(registerInfoSql,
            "select * from user_login where id order by id desc limit 0,1;");
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
    allUser = comMsg.id;

    for (int i = 1000; i <= allUser; i++) {
        sprintf(registerInfoSql, "select * from user_login where id = %d;", i);
        number_erro =
            sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
        if (!number_erro) {
            allVoidNumber++;
        }
    }

    int voidUser[allVoidNumber + 1];

    for (int i = 1000, j = 0; (i <= allUser) && (j <= allVoidNumber); i++) {
        sprintf(registerInfoSql, "select * from user_login where id = %d;", i);
        number_erro =
            sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
        if (!number_erro) {
            voidUser[j++] = i;
        }
    }

    voidUser[allVoidNumber] = comMsg.id - 1000 - allVoidNumber;

    send(clientfd, &allVoidNumber, sizeof(allVoidNumber), 0);
    send(clientfd, voidUser, sizeof(voidUser), 0);

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    sprintf(registerInfoSql, "select * from user_info where id = %d;",
            userMsg->id);
    number_erro =
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, &errmsg);

    if ((userMsg->id > comMsg.id) || (userMsg->id < 1000) || (!number_erro)) {
        printf("查询错误\n");
        strcpy(userMsg->tips, "@");
        send(clientfd, userMsg, sizeof(*userMsg), 0);
        return;
    }

    send(clientfd, &comMsg, sizeof(comMsg), 0);

    backInfo(userMsg, clientfd);

    return;
}

void do_updata(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    if ('#' == userMsg->tips[0]) {
        printf("密码修改\n");
        do_passwd(userMsg, clientfd);
    } else if ('@' == userMsg->tips[0]) {
        printf("信息修改\n");

        sprintf(registerInfoSql, "select * from user_login where id = %d;",
                userMsg->id);
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

        send(clientfd, &comMsg, sizeof(comMsg), 0);

        strcpy(userMsg->name, "");
        strcpy(userMsg->phone, "");
        strcpy(userMsg->addr, "");
        strcpy(userMsg->sex, "");

        recv(clientfd, userMsg, sizeof(*userMsg), 0);

        if ((!strlen(userMsg->name)) && (!strlen(userMsg->phone)) &&
            (!strlen(userMsg->sex)) && (!strlen(userMsg->addr))) {
            printf("\n修改失败，客户端退出\n");
            return;
        }

        sprintf(registerInfoSql,  //登录信息修改
                "update user_login set tips = \"%s\" where id = %d;",
                userMsg->tips, userMsg->id);
        sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

        sprintf(registerInfoSql, "delete from user_info where id =%d;",
                userMsg->id);
        sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

        sprintf(registerInfoSql,  //个人信息修改
                "insert into user_info values "
                "(%d,\"%s\",\"%s\",%d,\"%s\",\"%s\",%d,\"%s\");",
                userMsg->id, userMsg->name, userMsg->sex, userMsg->age,
                userMsg->phone, userMsg->addr, userMsg->type, userMsg->tips);
        sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

        sprintf(registerInfoSql, "select * from user_info where id = %d;",
                userMsg->id);
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

        send(clientfd, &comMsg, sizeof(comMsg), 0);

        printf("修改完成\n");

        backInfo(userMsg, clientfd);

        return;
    }

    return;
}

void do_passwd(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    recv(clientfd, userMsg, sizeof(*userMsg), 0);
    sprintf(registerInfoSql, "select * from user_login where id = %d;",
            userMsg->id);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    while (strcmp(comMsg.passwd, userMsg->passwd)) {
        strcpy(comMsg.tips, "#");
        send(clientfd, &comMsg, sizeof(comMsg), 0);

        recv(clientfd, userMsg, sizeof(*userMsg), 0);

        if (userMsg->tips[0] == '@') {
            printf("修改失败");
            return;
        }

        sprintf(registerInfoSql, "select * from user_login where id = %d;",
                userMsg->id);
        sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);
    }
    strcpy(comMsg.tips, "A");
    send(clientfd, &comMsg, sizeof(comMsg), 0);

    recv(clientfd, userMsg, sizeof(*userMsg), 0);

    sprintf(registerInfoSql,
            "update user_login set passwd = \"%s\" where id = %d;",
            userMsg->passwd, userMsg->id);
    sqlite3_exec(db, registerInfoSql, NULL, NULL, NULL);

    printf("修改成功");

    memset(&comMsg, 0, sizeof(comMsg));
    return;
}

void backInfo(MSG* userMsg, int clientfd) {
    char registerInfoSql[1024];

    sprintf(registerInfoSql, "select * from user_info where id =%d;",
            userNumber);
    sqlite3_exec(db, registerInfoSql, sqliteCallback, NULL, NULL);

    send(clientfd, &comMsg, sizeof(comMsg), 0);
    memset(&comMsg, 0, sizeof(comMsg));
    return;
}