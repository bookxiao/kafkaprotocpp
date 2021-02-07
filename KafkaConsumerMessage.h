#pragma once

#include "Packet.h"
#include "ApiConstants.h"
#include <iostream>
#include <map>
#include <set>

namespace kafkaprotocpp {

struct QueryGroupCoordinator : public Marshallable
{
    enum { apikey = ApiConstants::GROUP_COORDINATOR_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::string groupid;

    void marshal(Pack &pk) const
    {
        pk << groupid;
    }
};

struct QueryGroupCoordinatorRes : public Marshallable
{
    std::string groupid; // not packed
    int16_t errcode;
    int32_t coordinatorId;
    std::string coordHost;
    int32_t coordPort;

    void unmarshal(const Unpack & up)
    {
        up >> errcode >> coordinatorId >> coordHost >> coordPort;
    }
};

struct PartitionOffsetMeta : public Marshallable
{
    int32_t parn;
    int64_t offset;
    std::string meta;

    void marshal(Pack &pk) const
    {
        pk << parn << offset << meta;
    }
};

struct TopicOffsetMeta : public Marshallable
{
    std::string topic;
    std::vector<PartitionOffsetMeta> parOffsetMetas;

    void marshal(Pack &pk) const
    {
        pk << topic << parOffsetMetas;
    }
};

struct OffsetCommitRequest : public Marshallable
{
    enum {apikey = ApiConstants::OFFSET_COMMIT_REQUEST_KEY, apiver = ApiConstants::API_VERSION2};

    std::string groupid;
    int32_t generationId;
    std::string consumerId;
    int64_t retentionTime;
    std::vector<TopicOffsetMeta> offsets;

    void marshal(Pack &pk) const
    {
        pk << groupid << generationId << consumerId << retentionTime << offsets;
    }
};

template <class T>
struct TopicBlock : public Marshallable
{
    std::string topic;
    std::vector<T> block;

    void marshal(Pack &pk) const
    { 
        pk << topic << block;
    }

    void unmarshal(const Unpack &up)
    {
        up >> topic >> block;
    }
};


struct PartitionErrcode : public Marshallable
{
    int32_t parn;
    int16_t errcode;

    void unmarshal(const Unpack &up) 
    {
        up >> parn >> errcode;
    }
};

struct OffsetCommitResponse : public Marshallable
{
    std::vector<TopicBlock<PartitionErrcode>> result;

    void unmarshal(const Unpack &up)
    {
        up >> result;
    }
};

struct MemberSubToppars : public Marshallable
{
    int32_t totalSize = 0;
    std::map<std::string,std::set<int>> toppars;

    void marshal(Pack &pk) const {
        size_t sizeHead = pk.size();
        pk << totalSize;
        int16_t cnt = toppars.size();
        pk << (int16_t)(toppars.size());
        for(auto& parset : toppars) {
            pk << parset.first;
            pk << (int16_t)(parset.second.size());
            for(auto par : parset.second) {
                pk << (int32_t)par;
            }
        }
        pk.replace_int32(sizeHead, pk.size() - sizeHead - 4);
    }

    void unmarshal(const Unpack & up) {
        up >> totalSize;
        if(totalSize <=0 )
            return;

        int16_t cnt;
        up >> cnt;
        for(auto i = 0; i < cnt; ++i) {
            std::string topic;
            up >> topic;
            std::set<int>& parset = toppars[topic];
            int16_t parn;
            up >> parn;
            for(auto j = 0; j < parn; ++j) {
                int32_t par;
                up >> par;
                parset.insert(par);
            }
        }
    }
};

struct ProtocolMetadata : public Marshallable {
    int32_t size;
    int16_t version;
    std::vector<std::string> topics; // 新增订阅的topic列表
    MemberSubToppars oldSubs; // 当前已分配的partition列表

    void marshal(Pack &pk) const {
        size_t sizeHead = pk.size();
        pk.push_int32(0);
        pk << version << topics << oldSubs;
        pk.replace_int32(sizeHead, pk.size() - sizeHead - 4);
    }
    void unmarshal(const Unpack &up) {
        up >> size;
        Unpack iup(up.data(), size);
        up.reset(up.data()+size, up.size() - size); // skip

        iup >> version >> topics >> oldSubs;
    }
};

struct GroupProtocol : public Marshallable {
    std::string name;
    ProtocolMetadata meta;

    void marshal(Pack &pk) const {
        pk << name;
        pk << meta;
    }
};

struct JoinGroupRequest : public Marshallable
{
    enum { apikey = ApiConstants::JOIN_GROUP_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::string groupid;
    int32_t timeout;
    std::string memberid;
    std::string prototype;
    std::vector<GroupProtocol> protocols;

    void marshal(Pack &pk) const {
        pk << groupid << timeout << memberid << prototype << protocols;
    }
};

struct GroupMemberMeta : public Marshallable
{
    std::string memberid;
    ProtocolMetadata meta;

    void unmarshal(const Unpack& up) {
        up >> memberid >> meta;
    }
};

struct JoinGroupResponse : public Marshallable
{
    int16_t errcode;
    int32_t genid;
    std::string proto;
    std::string leaderid;
    std::string memberid;
    std::vector<GroupMemberMeta> members;

    void unmarshal(const Unpack& up)
    {
        up >> errcode >> genid >> proto >> leaderid >> memberid >> members;
    }
};

struct TopicPartitionsBlock : public Marshallable
{
    std::string topic;
    std::vector<int32_t> partitions;

    void marshal(Pack &pk) const {
        pk << topic << partitions;
    }

    void unmarshal(const Unpack &up) {
        up >> topic >> partitions;
    }
};

struct MemberAssignment : public Marshallable
{
    int32_t size;
    int16_t version;
    std::vector<TopicPartitionsBlock> topicAssign;
    std::string userdata;

    void marshal(Pack &pk) const
    {
        size_t sizeHead = pk.size();
        pk << size;
        pk << version << topicAssign;
        pk.push_bytes(userdata.data(), userdata.size());
        pk.replace_int32(sizeHead, pk.size() - sizeHead - 4);
    }

    void unmarshal(const Unpack &up) {
        up >> size;
        if(size <= 0)
            return;

        Unpack iup(up.data(), size);
        up.reset(up.data()+size, up.size() - size); // skip

        iup >> version >> topicAssign;
        userdata = iup.pop_bytes();
    }
};

struct SyncGroupRequest : public Marshallable
{
    enum { apikey = ApiConstants::SYNC_GROUP_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::string groupid;
    int32_t genid; // generation id
    std::string memberid;
    std::map<std::string,MemberAssignment> assignments;

    void marshal(Pack &pk) const {
        pk << groupid << genid << memberid;
        pk.push_int32(assignments.size());
        for(auto& ma : assignments) {
            pk << ma.first << ma.second;
        }
    }
};

struct SyncGroupResponse : public Marshallable
{
    int16_t errcode;
    MemberAssignment assignment;

    void unmarshal(const Unpack& up) {
        up >> errcode >> assignment;
    }
};

struct HeartbeatRequest : public Marshallable
{
    enum { apikey = ApiConstants::HEARTBEAT_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};
    std::string groupid;
    int32_t genid;
    std::string memberid;

    void marshal(Pack &pk) const {
        pk << groupid << genid << memberid;
    }
};

struct HeartbeatResponse : public Marshallable
{
    int16_t errcode;

    void unmarshal(const Unpack& up) {
        up >> errcode;
    }
};

struct LeaveGroupRequest : public Marshallable
{
    enum { apikey = ApiConstants::LEAVE_GROUP_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::string groupid;
    std::string memberid;

    void marshal(Pack &pk) const {
        pk << groupid << memberid;
    }
};

struct LeaveGroupResponse : public Marshallable
{
    int16_t errcode;

    void unmarshal(const Unpack& up) {
        up >> errcode;
    }
};

struct GroupProtoInfo : public Marshallable {
    std::string groupid;
    std::string prototype;

    void unmarshal(const Unpack& up) {
        up >> groupid >> prototype;
    }
};

struct ListGroupRequest : public Marshallable
{
    enum {apikey = ApiConstants::LIST_GROUPS_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};
};

struct ListGroupResponse : public Marshallable
{
    int16_t errcode;
    std::vector<GroupProtoInfo> groups;

    void unmarshal(const Unpack& up) {
        up >> errcode >> groups;
    }
};

struct DescribeGroupRequest : public Marshallable
{
    enum {apikey = ApiConstants::DESCRIBE_GROUPS_REQUEST_KEY, apiver = ApiConstants::API_VERSION0};

    std::vector<std::string> groupids;

    void marshal(Pack &pk) const {
        pk << groupids;
    }
};

struct GroupMember : public Marshallable
{
    std::string memberid;
    std::string clientid;
    std::string host;
    ProtocolMetadata protometa;
    MemberAssignment assignment;

    void unmarshal(const Unpack& up)
    {
        up >> memberid >> clientid >> host;
        up >> protometa >> assignment;
    }
};

struct GroupInfo : public Marshallable {
    int16_t errcode;
    std::string groupid;
    std::string state;
    std::string prototype;
    std::string proto;
    std::vector<GroupMember> members;

    void unmarshal(const Unpack &up)
    {
        up >> errcode >> groupid >> state >> prototype >> proto >> members;
    }
};

struct DescribeGroupResponse : public Marshallable
{
    std::vector<GroupInfo> groupInfos;

    void unmarshal(const Unpack &up) {
        up >> groupInfos;
    }
};

// query group's offset
struct FetchGroupOffsetRequest : public Marshallable
{
    enum {apikey = ApiConstants::OFFSET_FETCH_REQUEST_KEY, apiver = ApiConstants::API_VERSION1};
    
    std::string groupid;
    std::vector<TopicPartitionsBlock> toppars;

    void marshal(Pack &pk) const {
        pk << groupid << toppars;
    }
};

struct PartitionOffsetMetaRes : public Marshallable
{
    int32_t parn;
    int64_t offset;
    std::string meta;
    int16_t errcode;

    void unmarshal(const Unpack &up) 
    {
        up >> parn >> offset >> meta >> errcode;
    }
};

struct TopicPartitionsOffsetBlock : public Marshallable
{
    std::string topic;
    std::vector<PartitionOffsetMetaRes> partitionOffsets;

    void unmarshal(const Unpack &up) {
        up >> topic >> partitionOffsets;
    }
};

struct FetchGroupOffsetResponse : public Marshallable
{
    std::string groupid; // not packed
    std::vector<TopicPartitionsOffsetBlock> offsets;

    void unmarshal(const Unpack &up) {
        up >> offsets;
    }
};


}
