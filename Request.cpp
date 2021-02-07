#include "Request.h"

using namespace std;
using namespace kafkaprotocpp;

Request::Request(int32_t ctxid, const std::string& clientid, int16_t apikey, int16_t apiver, Marshallable& m) :
    RequestHeader(ctxid, clientid, apikey, apiver),
    pb(),
    pk(pb)
{
    pk.push_int32(0); // reserved length field
    pk.push_int16(apikey);
    pk.push_int16(apiver);
    pk.push_int32(ctxid);
    pk.push_string(clientid);

    m.marshal(pk);

    // update length
    pk.replace_int32(0, pk.size()-4);
}

void Request::setCtxid(int32_t ctxid)
{
    m_ctxid = ctxid;
    pk.replace_int32(8, m_ctxid);
}

const char* Request::data()
{
    return pk.data();
}

size_t Request::size()
{
    return pk.size();
}
