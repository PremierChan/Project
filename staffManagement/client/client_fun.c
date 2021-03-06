#include "client.h"

void socket_init(char const* argv[]) {
    serverport = atoi(argv[2]);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Socket");
        exit(0);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverport);

#if 1
    if (inet_pton(AF_INET, argv[1], (void*)(&serveraddr.sin_addr.s_addr)) < 0) {
        perror("Pton");
        exit(0);
    }
#else
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
#endif

    if (connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) <
        0) {
        perror("Connect");
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
    memset(msg, 0, sizeof(MSG));
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

    printf("请输入效验码：");
    scanf("%s", msg->tips);
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
    printf("效验码：%s\n", msg->tips);
    printf("\n[请按下回车来返回上级页面]");

    recv(socketfd, msg, sizeof(MSG), 0);

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
        printf("登陆失败！将返会主页！\n\n[输入回车确认]");
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
    scanf("%d", &(msg->id));
    getchar();
    printf("\n请输入该账号的姓名：");
    scanf("%s", msg->name);
    getchar();
    printf("请输入该账号的手机号：");
    scanf("%s", msg->phone);
    getchar();
    printf("请输入该账号的效验码：");
    scanf("%s", msg->tips);
    getchar();

    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    找回密码                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");

    if (msg->tips[0] == '#') {
        printf("\n[抱歉，您输入的信息有误，按下回车继续]");
        getchar();
        return;
    }

    printf("验证成功！您的密码为  %s\n", msg->passwd);
    printf("\n[按下回车返回主页]");
    getchar();
    return;
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
                system("clear");
                printf(
                    "*****************************************************\n");
                printf(
                    "*                                                   *\n");
                printf(
                    "*                    修改信息                       *\n");
                printf(
                    "*                                                   *\n");
                printf(
                    "*****************************************************\n");
                printf("你好！%s\n\n", msg->name);
                printf("****************\n");
                printf("*  1.个人信息  *\n");
                printf("*  2.密    码  *\n");
                printf("****************\n");
                printf("请输入要修改的内容：");
                scanf("%d", &flagNumber);
                getchar();

                if ((1 == flagNumber) || (2 == flagNumber)) {
                    msg->tips[0] = 'U';
                    do_update_root_user(socketfd, msg);  // root用户修改信息
                    return;
                }

                printf("\n[输入有误，按下回车返回上一层]");
                getchar();
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
    puts("\n[按下回车跳转到注册页]");
    getchar();
    do_register(socketfd, msg);
    return;
}

void do_delete_user(int socketfd, MSG* msg) {
    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    用户删除                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("你好！%s\n\n请输入您要删除用户的ID：", msg->name);
    scanf("%d", &(msg->id));
    getchar();
    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    if (msg->tips[0] == '@') {
        printf("\n\n[您输入的ID有误,按下回车继续]");
        getchar();
        system("clear");
        return;
    }

    printf("\n\n账号ID：%d\n", msg->id);
    printf("用户名：%s\n\n", msg->name);
    printf("如果您确定要删除该用户请在此输入该用户ID：");
    scanf("%d", &(msg->id));

    send(socketfd, msg, sizeof(MSG), 0);

    printf("\n\n[删除成功按下回车返回]");
    getchar();

    recv(socketfd, msg, sizeof(MSG), 0);

    system("clear");

    return;
}

void do_update_root_user(int socketfd, MSG* msg) {
    send(socketfd, msg, sizeof(MSG), 0);
    if (1 == flagNumber) {
        strcpy(msg->tips, "@");
        send(socketfd, msg, sizeof(MSG), 0);

        recv(socketfd, msg, sizeof(MSG), 0);

        system("clear");
        printf("*****************************************************\n");
        printf("*                                                   *\n");
        printf("*                    修改信息                       *\n");
        printf("*                                                   *\n");
        printf("*****************************************************\n");

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

        printf("请输入效验码：");
        scanf("%s", msg->tips);
        getchar();

        send(socketfd, msg, sizeof(MSG), 0);
        recv(socketfd, msg, sizeof(MSG), 0);

        system("clear");
        printf("*****************************************************\n");
        printf("*                                                   *\n");
        printf("*                   个人信息                        *\n");
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
        printf("效验码：%s\n", msg->tips);
        printf("\n[请按下回车来返回上级页面]");

        recv(socketfd, msg, sizeof(MSG), 0);

        getchar();

    } else if (2 == flagNumber) {
        strcpy(msg->tips, "#");
        send(socketfd, msg, sizeof(MSG), 0);
        system("clear");
        do_update_general_user(socketfd, msg);
    }
    return;
}

void do_search_root_user(int socketfd, MSG* msg) {
    int number;
    int allUser = 1;

    recv(socketfd, &number, sizeof(number), 0);

    int voidUser[number + 1];
    recv(socketfd, voidUser, sizeof(voidUser), 0);

    if (0 > voidUser[number])
        allUser = voidUser[number];

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    信息查询                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("你好！%s\n\n", msg->name);
    printf("*****************************************************\n");
    printf("当前系统中共有 [%d] 位用户第一位用户编号为 [1000]  \n", allUser);
    printf("以下为空ID号：");
    for (int i = 0; i < number; i++) {
        printf("%d\t", voidUser[i]);
    }
    printf("\n*****************************************************\n");
    printf("\n请输入要查询的用户ID:");
    scanf("%d", &msg->id);
    getchar();

    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    if (msg->tips[0] == '@') {
        printf("\n\n[您输入的ID有误,按下回车继续]");
        getchar();
        system("clear");
        return;
    }

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    信息查询                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("用  户：%s\n", msg->name);

    printf("账号ID: %d\n", msg->id);
    printf("年  龄：%d\n", msg->age);
    printf("性  别：%s\n", msg->sex);
    printf("手机号：%s\n", msg->phone);
    printf("地  址：%s\n", msg->addr);

    printf("\n[请输入回车返回]");

    getchar();

    recv(socketfd, msg, sizeof(MSG), 0);

    system("clear");
    return;
}

void do_update_general_user(int socketfd, MSG* msg) {
    int number = 0;
    // recv(socketfd, msg, sizeof(MSG), 0);
    char oldPasswd[20];

    system("clear");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*                    密码修改                       *\n");
    printf("*                                                   *\n");
    printf("*****************************************************\n");
    printf("你好！%s\n\n", msg->name);
    printf("请输入原始密码：");
    scanf("%s", msg->passwd);
    getchar();

    send(socketfd, msg, sizeof(MSG), 0);
    recv(socketfd, msg, sizeof(MSG), 0);

    while (msg->tips[0] == '#') {
        printf("\n错误！请重新输入：");
        scanf("%s", msg->passwd);
        getchar();

        send(socketfd, msg, sizeof(MSG), 0);
        recv(socketfd, msg, sizeof(MSG), 0);

        number++;
        if (4 == number) {
            printf("\n您已连续输错5次!请重新登录！按下回车继续\n");
            strcpy(msg->tips, "@");
            send(socketfd, msg, sizeof(MSG), 0);
            getchar();
            return;
        }
    }
    printf("请输入新密码：");
    scanf("%s", msg->passwd);
    getchar();

    send(socketfd, msg, sizeof(MSG), 0);

    printf("\n[修改成功！按下空格返回主页]");
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

    printf("\n[请输入回车返回]");
    getchar();
    return;
}