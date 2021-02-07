#include "Connection.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

using namespace kafkaprotocpp;

static int send_request(int skfd, const char* data, size_t size)
{
    size_t n = 0;
    while(n < size) {
        int len = write(skfd, data+n, size-n);
        if(len < 0) {
            if(errno == EAGAIN)
                continue;
            return -1;
        }
        n += len;
    }

    return 0;
}

static char* read_response(int skfd)
{
    // read response
    int size = 0;
    int ret = read(skfd, &size, 4);
    if(ret != 4) {
        printf("not enough length field:%d\n", ret);
        return NULL;
    }
    size = htonl(size);

    int n = 0;
    char* buffer = (char*)malloc(size+4);
    *(int32_t*)buffer = ntohl(size);
    while(n < size) {
        int len = read(skfd, buffer+4+n, size-n);
        if(len < 0) {
            if(errno == EAGAIN)
                continue;
            goto err;
        }
        n += len;
    }
    return buffer;

err:
    free(buffer);
    return NULL;
}

int Connection::Connect(const std::string& host, int port)
{
    if(sockfd > 0) {
        close(sockfd);
        sockfd = 0;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("connect %s:%d failed\n", host.c_str(), port);
        return -1;
    }
    printf("connect %s:%d success\n", host.c_str(), port);

    return 0;
}

int Connection::SendRequest(int apikey, int apiver, kafkaprotocpp::Marshallable& req, kafkaprotocpp::Marshallable& res)
{
    if(sockfd <= 0) {
        return -1;
    }

    kafkaprotocpp::Request outreq(1, "inner_test", apikey, apiver, req);
    if(send_request(sockfd, outreq.data(), outreq.size()) < 0)
        return -1;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    int ret = select(sockfd+ 1, &rfds, NULL, NULL, &tv);
    if(ret <= 0) {
        printf("read resp timeout\n");
        close(sockfd);
        sockfd = 0;
        return -1;
    }

    auto buf = read_response(sockfd);
    auto len = kafkaprotocpp::Response::peeklen(buf);
    if(buf == NULL)
        return -1;

    kafkaprotocpp::Response resp(buf, len);
    resp.head();
    res.unmarshal(resp.up);
    free(buf);

    return 0;
}

Connection::~Connection()
{
    if(sockfd > 0) {
        close(sockfd);
    }
}
