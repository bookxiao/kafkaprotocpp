#pragma once

#include "Packet.h"
#include "ApiConstants.h"
#include <iostream>

namespace kafkaprotocpp {

struct Broker : public Marshallable
{
    int32_t nodeid;
    std::string host;
    int32_t port;

    virtual void marshal(Pack &pk) const
    {
        pk << nodeid << host << port;
    }

    virtual void unmarshal(const Unpack &up)
    {
        up >> nodeid >> host >> port;
    }
};

struct PartitionMetadata : public Marshallable
{
    int16_t errcode;
    int32_t parid;
    int32_t leader;
    std::vector<int32_t> replicas;
    std::vector<int32_t> isr;

    virtual void marshal(Pack &pk) const
    {
        pk << errcode << parid << leader;
        marshal_container(pk, replicas);
        marshal_container(pk, isr);
    }

    virtual void unmarshal(const Unpack &up)
    {
        up >> errcode >> parid >> leader;
        unmarshal_container(up, std::back_inserter(replicas));
        unmarshal_container(up, std::back_inserter(isr));
    }
};

struct TopicMetadata : public Marshallable
{
    int16_t errcode;
    std::string strTopic;
    std::vector<PartitionMetadata> vecParMeta;

    virtual void marshal(Pack &pk) const
    {
        pk << errcode << strTopic << vecParMeta;
    }

    virtual void unmarshal(const Unpack &up)
    {
        up >> errcode >> strTopic >> vecParMeta;
    }
};

struct MetadataRequest : public Marshallable
{
    enum { apikey = ApiConstants::METADATA_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::vector<std::string> vecTopic;

    virtual void marshal(Pack &pk) const
    {
        pk << vecTopic;
    }
    virtual void unmarshal(const Unpack &up)
    {
        up >> vecTopic;
    }

};

struct MetadataResponse : public Marshallable
{
    std::vector<Broker> vecBroker;
    std::vector<TopicMetadata> vecTopicMeta;

    virtual void marshal(Pack &pk) const
    {
        pk << vecBroker << vecTopicMeta;
    }

    virtual void unmarshal(const Unpack &up)
    {
        up >> vecBroker >> vecTopicMeta;
    }
};

struct Message : public Marshallable
{
    int64_t offset;
    int32_t size; // do NOT contain size of 'offset' and 'size' field

    int32_t crc;
    int8_t magicByte; // version
    int8_t attr; // 0~3 bits - compression code
                 // 4th bit - timestamp type, 0 for CreateTime, 1 for LogAppendTime
                 // 5~7 bits - reserved, must be 0
    int64_t timestamp; // Unit is milliseconds since beginning of the epoch
    std::string key;
    std::string value;

    int version() const {
        return magicByte;
    }
    int comptype() const {
        return attr & 0x07;
    }
    void setComptype(int type) {
        attr = (attr & 0xF8) | (type & 0x07);
    }

    Message() : offset(0), size(0), timestamp(-1) {}
    Message(int8_t magic_, int8_t attr_, int64_t ts_, std::string&& key_, std::string&& value_) :
        offset(0), magicByte(magic_), attr(attr_), timestamp(ts_), key(key_), value(value_) {
            size = 8 + 4 + 4 + 1 + 1 + 8 + 4 + key.length() + 4 + value.length();
        }

    virtual void marshal(Pack &pk) const {
        pk << offset;
        size_t sizeHead = pk.size();
        pk.push_int32(0);
        pk.beginCRC32();
        pk << magicByte << attr;
        if(version() == 1) {
            pk << timestamp;
        }
        pk.push_bytes(key.data(), key.size());
        pk.push_bytes(value.data(), value.size());
        pk.endCRC32();
        pk.replace_int32(sizeHead, pk.size() - sizeHead - 4);
    }

    virtual void unmarshal(const Unpack &up) {
        if(up.size() < 8 + 4)
            throw IncompletePacket("offset and size of this msg is missing");
        
        up >> offset >> size;
        if(up.size() < size) // incomplete message, just ignore it
            throw IncompletePacket("msg is incomplete");

        up >> crc >> magicByte >> attr;
        if(version() == 1) {
            up >> timestamp;
        } 
        key = up.pop_bytes();
        value = up.pop_bytes();
    }
};

struct MessageSet : public Marshallable
{
    MessageSet() : size(0) {}

    int32_t size;
    std::vector<Message> msgSet;

    void pushMessage(Message&& msg) {
        size += msg.size;
        msgSet.emplace_back(msg);
    }

    virtual void marshal(Pack &pk) const 
    {
        for(auto msg : msgSet) {
            msg.marshal(pk);
        }
    }

    virtual void unmarshal(const Unpack &up)
    {
        Unpack iup(up.data(), size);
        up.reset(up.data()+size, up.size() - size); // skip this messageset
        while(! iup.empty()) {
            Message msg;
            try {
                msg.unmarshal(iup);
                msgSet.push_back(msg);
            } catch(IncompletePacket& ipe) {
                iup.reset(iup.data()+iup.size(), 0);
            }
        }
    }
};

struct FetchPartitionRequestUnit : public Marshallable
{
    int32_t parn;
    int64_t offset;
    int32_t maxBytes;

    FetchPartitionRequestUnit(int32_t p, int64_t o, int32_t mb) : parn(p), offset(o), maxBytes(mb) {}

    void marshal(Pack &pk) const
    {
        pk << parn << offset << maxBytes;
    }
};

struct FetchTopicRequestUnit : public Marshallable
{
    std::string topicStr;
    std::vector<FetchPartitionRequestUnit> fetchParVec;

    FetchTopicRequestUnit(const std::string& t) : topicStr(t) {}

    void marshal(Pack &pk) const
    {
        pk << topicStr << fetchParVec;
    }
};

// FetchRequest有3个版本:v0/v1/v2，3个版本的请求格式一样，但是回包格式不一样
// 其中v0版本的回包没有ThrottleTime字段;
// 其中v0/v1只能拉回来v0版本的Message, v2可以拉回来v0/v1版本的Message
struct FetchRequest : public Marshallable
{
    int32_t replicaId;
    int32_t maxWaitTimeMs;
    int32_t minBytes;
    std::vector<FetchTopicRequestUnit> fetchTopicVec;

    void marshal(Pack &pk) const
    {
        pk << replicaId << maxWaitTimeMs << minBytes << fetchTopicVec;
    }
};

struct FetchRequestV0 : public FetchRequest
{
    enum { apikey = ApiConstants::FETCH_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};
};

struct FetchRequestV1 : public FetchRequest
{
    enum { apikey = ApiConstants::FETCH_REQUEST_KEY, apiver = ApiConstants::API_VERSION1};
};

struct FetchRequestV2 : public FetchRequest
{
    enum { apikey = ApiConstants::FETCH_REQUEST_KEY, apiver = ApiConstants::API_VERSION2};
};

struct FetchPartitionResponseUnit : public Marshallable
{
    int32_t parn;
    int16_t errcode;
    int64_t highWatherMarkOffset;
    MessageSet msgSet;

    void unmarshal(const Unpack &up)
    {
        up >> parn >> errcode >> highWatherMarkOffset;
        up >> msgSet.size;
        up >> msgSet;
    }
};

struct FetchTopicResponseUnit : public Marshallable
{
    std::string topic;
    std::vector<FetchPartitionResponseUnit> fetchParResult;

    void unmarshal(const Unpack &up)
    {
        up >> topic >> fetchParResult;
    }
};

struct FetchResponseV0 : public Marshallable
{
    std::vector<FetchTopicResponseUnit> result;

    virtual void unmarshal(const Unpack &up)
    {
        up >> result;
    }
};

struct FetchResponseV1 : public FetchResponseV0
{
    int32_t throttleTime;

    virtual void unmarshal(const Unpack &up)
    {
        up >> throttleTime;
        FetchResponseV0::unmarshal(up);
    }
};

struct FetchResponseV2 : public FetchResponseV0
{
    int32_t throttleTime;

    virtual void unmarshal(const Unpack &up)
    {
        up >> throttleTime;
        FetchResponseV0::unmarshal(up);
    }
};


struct MessageExtraInfo {
    void* opaque;
};

struct ProducePartitionReqUnit : public Marshallable
{
    int32_t parn;
    MessageSet msgSet;
    std::vector<MessageExtraInfo> extInfo; // do NOT marshal/unmarshal

    virtual void marshal(Pack &pk) const
    {
        pk << parn ;
        size_t sizeHead = pk.size();
        pk.push_int32(0);
        pk << msgSet;
        pk.replace_int32(sizeHead, pk.size() - sizeHead - 4);
    }
};

struct ProduceTopicReqUnit : public Marshallable
{
    std::string topic;
    std::vector<ProducePartitionReqUnit> parMsgSets;

    void marshal(Pack &pk) const
    {
        pk << topic << parMsgSets;
    }
};

struct ProduceRequest : public Marshallable
{
    enum { apikey = ApiConstants::PRODUCE_REQUEST_KEY, apiver = ApiConstants::API_VERSION2};

    int16_t ack; // if it is 1, the server will wait the data is written to the local log before sending a response;
                // if it is -1, the server will block until the message is commited by all in sync replicas before sending a response
    int32_t timeout; // max time in milliseconds the server can await the receipt of the number of ack.
    std::vector<ProduceTopicReqUnit> topicMsgSets;

    void marshal(Pack &pk) const
    {
        pk << ack << timeout << topicMsgSets;
    };
};

struct ProducePartitionResUnit : public Marshallable
{
    int32_t parn;
    int16_t errcode;
    int64_t offset;
    int64_t timestamp; // if LogAppendTime is used on Broker, this field return the LogAppendTime of the fir message in msgset

    void unmarshal(const Unpack &up)
    {
        up >> parn >> errcode >> offset >> timestamp;
    }
};

struct ProduceTopicResUnit : public Marshallable
{
    std::string topic;
    std::vector<ProducePartitionResUnit> parRespVec;

    void unmarshal(const Unpack &up)
    {
        up >> topic >> parRespVec;
    }
};

struct ProduceResponseV2 : public Marshallable
{
    std::vector<ProduceTopicResUnit> topicRespVec;
    int32_t throttleTime;

    void unmarshal(const Unpack& up)
    {
        up >> topicRespVec >> throttleTime;
    }
};

struct ListOffsetReqPartitionUnit : public Marshallable
{
    int32_t parn;
    int64_t time_before; // req the offset before a certain time(ms), -1 means the latest offset, -2 means the oldest offset

    void marshal(Pack &pk) const
    {
        pk << parn << time_before;
    }
};

struct ListOffsetReqTopicUnit : public Marshallable
{
    std::string topic;
    std::vector<ListOffsetReqPartitionUnit> parReqVec;

    void marshal(Pack & pk) const
    {
        pk << topic << parReqVec;
    }
};

struct ListOffsetRequest : public Marshallable
{
    enum { apikey = ApiConstants::LIST_OFFSET_REQUEST_KEY, apiver = ApiConstants::API_VERSION1};

    int32_t replicaId;
    std::vector<ListOffsetReqTopicUnit> topicReqVec;

    void marshal(Pack &pk) const
    {
        pk << replicaId << topicReqVec;
    }
};

struct PartitionOffsets : public Marshallable
{
    int32_t parn;
    int16_t errcode;
    int64_t timestamp;
    int64_t offset;

    void unmarshal(const Unpack &up)
    {
        up >> parn >> errcode >> timestamp >> offset;
    }
};

struct TopicOffsets : public Marshallable
{
    std::string topic;
    std::vector<PartitionOffsets> parOffsets;

    void unmarshal(const Unpack &up)
    {
        up >> topic >> parOffsets;
    }
};

struct ListOffsetResponse : public Marshallable
{
    std::vector<TopicOffsets> offsets;

    void unmarshal(const Unpack & up)
    {
        up >> offsets;
    }
};

}
