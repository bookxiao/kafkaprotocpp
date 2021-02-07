#pragma once

#include "Packet.h"
#include <string>

namespace kafkaprotocpp {

struct RequestHeader {
    int16_t m_apikey;
    int16_t m_version;
    int32_t m_ctxid;
    std::string m_clientid;

    RequestHeader(int32_t ctxid, const std::string& clientid, int16_t apikey, int16_t apiver) :
        m_apikey(apikey), m_version(apiver), m_ctxid(ctxid), m_clientid(clientid) {}
};

struct Request : public RequestHeader
{
protected:
    PackBuffer pb;
    //Pack hpk;
    Pack pk;

public:
    Request();

    Request(int32_t ctxid, const std::string& clientid, int16_t apikey, int16_t apiver, Marshallable& m);
    void setCtxid(int32_t ctxid);

    const char* data();
    size_t size();
};

}
