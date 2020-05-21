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

    puts("客户端准备就绪！");
}