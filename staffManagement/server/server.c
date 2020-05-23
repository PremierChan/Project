#include "server.h"

int main(int argc, const char* argv[]) {
    pthread_t tid;

    if (argc < 3) {
        fprintf(stderr, "使用方法 :%s  <valid>  <valid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    create_database();  //创建数据库
    socket_init(argv);
    while (1) {
        int clientfd;

        if ((clientfd = accept(listenfd, (struct sockaddr*)&clientaddr,
                               &client_len)) > 0) {
            if (pthread_create(&tid, NULL, do_client, (void*)&clientfd))
                puts("线程创建失败！");
        }
    }

    close(listenfd);
    return 0;
}
