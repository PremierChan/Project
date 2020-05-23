#include "client.h"

void socket_init(char const* argv[]) {
    serverport = atoi(argv[2]);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Socket:");
        exit(0);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverport);

#if 1
    if (inet_pton(AF_INET, argv[1], (void*)(&serveraddr.sin_addr.s_addr)) < 0) {
        perror("Pton:");
        exit(0);
    }
#else
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
#endif

    if (connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("Connect:");
        exit(0);
    }

    puts("客户端准备就绪！按下回车继续");
    getchar();
}

void initUi(void) {
    //初始化界面
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("* 1: 注册      2: 登陆      3: 忘记密码     4: 退出 *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("请输入：");

    //输入错误向服务器发送退出信息，并关闭套接字退出程序
    scanf("%d", &n);
    getchar();

    //根据n值进入不同的函数
    switch (n) {
        case 1:  // send R  注册
            msg.tips[0] = 'R';
            send(socketfd, &msg, sizeof(MSG), 0);
            do_register(socketfd, &msg);
            break;

        case 2:  // send L  登录
            msg.tips[0] = 'L';
            send(socketfd, &msg, sizeof(MSG), 0);
            do_login(socketfd, &msg);
            break;

        case 3:  // send F  找回密码
            msg.tips[0] = 'F';
            send(socketfd, &msg, sizeof(MSG), 0);
            do_forget_password(socketfd, &msg);
            break;

        case 4:  // send Q  退出
            msg.tips[0] = 'Q';
            send(socketfd, &msg, sizeof(MSG), 0);
            close(socketfd);
            system("clear");
            exit(0);
        default:
            system("clear");
            printf("输入错误！\n");
            break;
    }
}

void do_register(int socketfd, MSG* msg) {
    char q;

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                       注册                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    msg->type = 1;
    printf("请输入密码：");
    scanf("%s", msg->passwd);
    getchar();
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                       注册                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("请输入密码：******\n");
    printf("请输入姓名：");
    scanf("%s", msg->name);
    getchar();
    printf("请输入年龄：");
    while (!scanf("%d", &(msg->age))) {
        getchar();
        printf("\n输入错误，请重新输入年龄:");
    }
    printf("请输入性别：");
    scanf("%s", msg->sex);
    getchar();
    printf("请输入手机号：");
    scanf("%s", msg->phone);
    getchar();
    printf("请输入地址：");
    scanf("%s", msg->addr);
    getchar();

    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                   注册信息                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("姓  名：%s\n", msg->name);
    printf("年  龄：%d\n", msg->age);
    printf("性  别：%s\n", msg->sex);
    printf("手机号：%s\n", msg->phone);
    printf("地  址：%s\n", msg->addr);

    puts("");
    printf("[该ID号码将作为登陆账号使用]\nID号为：%d\n", msg->id);
    printf("密  码：******\n");
    printf("\n请按下回车来返回上级页面：");

    getchar();

    system("clear");
};

int do_login(int socketfd, MSG* msg) {
#if 0
    strcpy(msg->name, "CQL");
    strcpy(msg->passwd, "123");
    msg->id = 1;
    msg->type = 1;
    msg->age = 11;
    strcpy(msg->tips, "asd");
    strcpy(msg->sex, "boy");
    strcpy(msg->phone, "15774512737");
    strcpy(msg->addr, "asfafs");
#endif

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                       登陆                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("请输入登陆ID：");
    while (!scanf("%d", &(msg->id))) {
        getchar();
        system("clear");
        printf("*****************************************************\n");
        printf("*                                                   *\n");
        printf("*                       登陆                        *\n");
        printf("*                                                   *\n");
        printf("*****************************************************\n");
        printf("输入错误！请重新输入登陆ID：\n");
    }
    getchar();
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                       登陆                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("请输入登陆密码：");
    scanf("%s", msg->passwd);
    getchar();
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                       登陆                        *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");

    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    if (msg->tips[0] == '#') {
        printf("登陆失败！将返会主页！\n输入回车确认！");
        getchar();
        system("clear");
        return 0;
    }

    if (msg->type == 1) {
        do_general_user(socketfd, msg);  //登录为普通用户
        return 1;
    } else if (msg->type == 0) {
        do_root_user(socketfd, msg);  //登录为root用户
        return 1;
    }
};
void do_forget_password(int socketfd, MSG* msg) {
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    忘记密码                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("请输入ID:");
    scanf("%d", msg->id);
    send(socketfd, msg, sizeof(MSG), 0);
};

void do_root_user(int socketfd, MSG* msg) {
    int n = 0;
    while (1) {
        system("clear");
        printf("*****************************************************\n");
        printf("*                                                   *\n");
        printf("*                    Root用户                       *\n");
        printf("*                                                   *\n");
        printf("*****************************************************\n");
        printf("你好！%s\n", msg->name);
        /*
        printf("年  龄：%d\n", msg->age);
        printf("性  别：%s\n", msg->sex);
        printf("手机号：%s\n", msg->phone);
        printf("地  址：%s\n", msg->addr);
        */
        printf("\n****************************************\n");
        printf("* 1: 添加用户  2: 删除用户  3:修改信息 *\n");
        printf("* 4: 查询信息  5: 退出                 *\n");
        printf("****************************************\n");
        printf("请输入:");
        scanf("%d", &n);
        getchar();

        switch (n) {
            case 1:
                msg->tips[0] = 'A';
                send(socketfd, msg, sizeof(MSG), 0);
                do_add_user(socketfd, msg);  //添加用户
                break;

            case 2:
                msg->tips[0] = 'D';
                send(socketfd, msg, sizeof(MSG), 0);
                do_delete_user(socketfd, msg);  //删除用户
                break;

            case 3:
                msg->tips[0] = 'U';
                send(socketfd, msg, sizeof(MSG), 0);
                do_update_root_user(socketfd, msg);  // root用户修改信息
                return;
                break;

            case 4:
                msg->tips[0] = 'S';
                send(socketfd, msg, sizeof(MSG), 0);
                do_search_root_user(socketfd, msg);  // root用户查询信息
                break;

            case 5:
                msg->tips[0] = 'Q';  //退出
                send(socketfd, msg, sizeof(MSG), 0);
                return;
            default:
                printf("输入错误！\n");
                break;
        }
    }
}

void do_general_user(int socketfd, MSG* msg) {
    int n = 0;
    while (1) {
        system("clear");
        printf("*****************************************************\n");
        printf("*                                                   *\n");
        printf("*                    普通用户                       *\n");
        printf("*                                                   *\n");
        printf("*****************************************************\n");
        printf("你好！%s\n", msg->name);
        printf("\n************************************\n");
        printf("* 1: 修改密码  2: 查询信息  3:退出 *\n");
        printf("************************************\n");
        printf("请输入:");
        scanf("%d", &n);
        getchar();

        switch (n) {
            case 1:
                msg->tips[0] = 'P';
                send(socketfd, msg, sizeof(MSG), 0);
                do_update_general_user(socketfd, msg);  //普通用户修改信息
                return;
                break;

            case 2:
                msg->tips[0] = 'S';
                send(socketfd, msg, sizeof(MSG), 0);
                do_search_general_user(socketfd, msg);  //普通用户查询信息
                break;

            case 3:
                msg->tips[0] = 'Q';
                send(socketfd, msg, sizeof(MSG), 0);  //退出
                return;
            default:
                printf("输入错误！\n");
                break;
        }
    }
}

void do_add_user(int socketfd, MSG* msg) {
    puts("按下回车跳转到注册页");
    getchar();
    do_register(socketfd, msg);
    return;
}
void do_delete_user(int socketfd, MSG* msg) {
    printf("删除\n");
    getchar();
}
void do_update_root_user(int socketfd, MSG* msg) {
    printf("root修改\n");
    getchar();
}
void do_search_root_user(int socketfd, MSG* msg) {
    printf("root查询\n");
    getchar();
}

void do_update_general_user(int socketfd, MSG* msg) {
    int number = 0;
    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);
    char oldPasswd[20];

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    密码修改                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("你好！%s\n\n", msg->name);
    printf("请输入原始密码：");
    scanf("%s", oldPasswd);
    getchar();
    while (strcmp(oldPasswd, msg->passwd)) {
        printf("\n错误！请重新输入：");
        scanf("%s", oldPasswd);
        getchar();
        number++;
        if (4 == number) {
            printf("\n您已连续输错5次!请重新登录！按下回车继续\n");
            getchar();
            return;
        }
    }
    printf("请输入新密码：");
    scanf("%s", msg->passwd);
    getchar();
    printf("\n修改成功！按下空格返回上一页\n");
    getchar();
}

void do_search_general_user(int socketfd, MSG* msg) {
    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    信息查询                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("你好！%s\n", msg->name);

    if (msg->tips[0] == '#') {
        printf("%s\n", msg->tips + 1);
    } else {
        printf("账号ID : %d\n", msg->id);
        printf("年  龄：%d\n", msg->age);
        printf("性  别：%s\n", msg->sex);
        printf("手机号：%s\n", msg->phone);
        printf("地  址：%s\n", msg->addr);
    }

    printf("\n请输入回车返回！");
    getchar();
    return;
}