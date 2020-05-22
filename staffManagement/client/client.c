#include "client.h"

int main(int argc, char const* argv[]) {
    //判断输入的参数
    if (argc < 3) {
        printf("使用方法:%s <服务器IP> <服务器端口>\n", argv[0]);
        exit(0);
    }

    socket_init(argv);

    while (1) {
        initUi();
    }

    return 0;
}
