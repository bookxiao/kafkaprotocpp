#pragma once

#include "Packet.h"

namespace kafkaprotocpp {

struct ResponseHeader
{
    int32_t m_length;
    int32_t m_ctxid;
    int apikey;
};

struct Response : public ResponseHeader
{
public:
    Unpack up;
    const char* od;
    size_t os;

public:
    Response(const char* buffer, size_t size);
    void head();

    static int32_t peeklen(const char* data);
};

}
