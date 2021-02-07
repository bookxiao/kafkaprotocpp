#include "Response.h"

using namespace kafkaprotocpp;

Response::Response(const char* data, size_t len) : up(data, len), od(data), os(len) 
{

}

void Response::head()
{
    m_length = up.pop_int32();
    m_ctxid = up.pop_int32();
}

int32_t Response::peeklen(const char* data)
{
    int32_t i = *((int32_t*)data);
    return htonl(i) + 4;
}
