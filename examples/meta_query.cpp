#include "../KafkaMessage.h"
#include "../Request.h"
#include "../Response.h"
#include "../Connection.h"

#include <sstream>

using namespace kafkaprotocpp;

int main(int argc, char**argv)
{
    if(argc != 4) {
        printf("usage: ./meta_query host ip topic\n");
        return -1;
    }

    std::string host(argv[1]);
    int port = atoi(argv[2]);
    std::string topic(argv[3]);

    int result = 4; // -1 for connect failed, -2 for timeout, 1 for ok, 0 for unknown

    Connection con;
    if(con.Connect(host, port) < 0) {
        printf("Connect to %s:%d failed\n", host.c_str(), port);
        return -1;
    }


    MetadataRequest metareq;
    if(topic != "all")
        metareq.vecTopic.push_back(argv[3]);

    MetadataResponse meta;

    int ret = con.SendRequest(metareq.apikey, metareq.apiver, metareq, meta);
    if(ret < 0) {
        printf("send request failed\n");
        return -1;
    }

    for(auto& broker : meta.vecBroker) {
        printf("broker:%d, %s:%d\n", broker.nodeid, broker.host.c_str(), broker.port);
    }

    for(auto& tmeta : meta.vecTopicMeta) {
        printf("topic:%s, errcode:%d, parn:%ld\n", tmeta.strTopic.c_str(), tmeta.errcode, tmeta.vecParMeta.size());
        for(auto& pmeta : tmeta.vecParMeta) {
            std::stringstream ss;
            if(pmeta.replicas.empty()) {
                ss << "ëmpty";
            } else {
                for(size_t i = 0; i < pmeta.replicas.size(); ++i) { 
                    if( i > 0) ss << ",";
                    ss << pmeta.replicas[i];
                }
                if(pmeta.replicas.size() == 1) {
                    ss << "(single)";
                }
            }
            printf("\"topic\":\"%s\", \"partition\":%d, \"replicas\":[%s]\n", tmeta.strTopic.c_str(), pmeta.parid, ss.str().c_str());
        }
    }

    return result;
}
