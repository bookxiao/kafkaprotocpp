#include <stdint.h>
#include <stdlib.h>

#include "Packet.h"
#include "Request.h"
#include "Response.h"

namespace kafkaprotocpp {

class Connection
{
public:
    Connection() = default;
    ~Connection();

    int Connect(const std::string& host, int port);
    int SendRequest(int apikey, int apiver, kafkaprotocpp::Marshallable& req, kafkaprotocpp::Marshallable& res);

private:
    int sockfd = 0;
};

}
