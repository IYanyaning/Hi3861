 #ifndef TCP_SERVER_H
 #define TCP_SERVER_H
 #include <stdio.h>
 #include <unistd.h>
 #include "ohos_init.h"
 #include "cmsis_os2.h"
 #include "hi_wifi_api.h"
 #include "lwip/ip_addr.h"
 #include "lwip/netifapi.h"
 #include "lwip/sockets.h"

void tcp_server(unsigned short port)
 {
    ssize_t retval = 0;
    int backlog = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    int connfd = -1;
    char request[128] = "";
    struct sockaddr_in clientAddr = {0};
    socklen_t clientAddrLen = sizeof(clientAddr);
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);              // 端⼝号，从主机字节序转为⽹络字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 允许任意主机接⼊， 0.0.0.0
    retval = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // 绑定端⼝
    if (retval < 0)
    {
        printf("bind failed, %ld!\r\n", retval);
        goto do_cleanup;
    }
    printf("bind to port %d success!\r\n", port);
    retval = listen(sockfd, backlog); // 开始监听
    if (retval < 0)
    {
        printf("listen failed!\r\n");
        goto do_cleanup;
    }
    printf("listen with %d backlog success!\r\n", backlog);
    // 接受客户端连接，成功会返回⼀个表示连接的 socket ， clientAddr 参数将会携带客户端主机和端⼝信息 ；失败返回 -1
    // 此后的 收、发 都在 表示连接的 socket 上进⾏；之后 sockfd 依然可以继续接受其他客户端的连接，
    //  UNIX系统上经典的并发模型是“每个连接⼀个进程”——创建⼦进程处理连接，⽗进程继续接受其他客户端的连接
    //  鸿蒙liteos-a内核之上，可以使⽤UNIX的“每个连接⼀个进程”的并发模型
    //     liteos-m内核之上，可以使⽤“每个连接⼀个线程”的并发模型
    connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (connfd < 0)
    {
        printf("accept failed, %d, %d\r\n", connfd, errno);
        goto do_cleanup;
    }
    printf("accept success, connfd = %d!\r\n", connfd);
    printf("client addr info: host = %s, port = %d\r\n", 
inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    // 后续 收、发 都在 表示连接的 socket 上进⾏；
    retval = recv(connfd, request, sizeof(request), 0);
    if (retval < 0)
    {
        printf("recv request failed, %ld!\r\n", retval);
        goto do_disconnect;
    }
 
    printf("recv request{%s} from client done!\r\n", request);
    retval = send(connfd, request, strlen(request), 0);
    if (retval <= 0)
    {
        printf("send response failed, %ld!\r\n", retval);
        goto do_disconnect;
    }
    printf("send response{%s} to client done!\r\n", request);
 do_disconnect:
    closesocket(connfd);
 do_cleanup:
    printf("do_cleanup...\r\n");
    closesocket(sockfd);
 }
 #endif