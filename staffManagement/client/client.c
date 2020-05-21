#include "client.h"

int main(int argc, char const* argv[]) {
    //判断输入的参数
    if (argc < 3) {
        printf("Usage:%s <server_ip> <server_port>\n", argv[0]);
        exit(0);
    }

    socket_init(argv);

    
    return 0;
}
