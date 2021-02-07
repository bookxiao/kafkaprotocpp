#pragma once

namespace kafkaprotocpp {

class ApiConstants {
public:
    const static int API_VERSION0 = 0;
    const static int API_VERSION1 = 1;
    const static int API_VERSION2 = 2;

    // API request key values
    const static int PRODUCE_REQUEST_KEY = 0;
    const static int FETCH_REQUEST_KEY = 1;
    const static int LIST_OFFSET_REQUEST_KEY = 2;
    const static int METADATA_REQUEST_KEY = 3;
    const static int LEADER_AND_ISR_REQUEST_KEY = 4;
    const static int STOP_REPLICA_REQUEST_KEY = 5;
    //const static int OFFSET_COMMIT_REQUEST_KEY = 6;
    //const static int OFFSET_FETCH_REQUEST_KEY = 7;
    const static int OFFSET_COMMIT_REQUEST_KEY = 8;
    const static int OFFSET_FETCH_REQUEST_KEY = 9;
    const static int GROUP_COORDINATOR_REQUEST_KEY = 10;
    const static int JOIN_GROUP_REQUEST_KEY = 11;
    const static int HEARTBEAT_REQUEST_KEY = 12;
    const static int LEAVE_GROUP_REQUEST_KEY = 13;
    const static int SYNC_GROUP_REQUEST_KEY = 14;
    const static int DESCRIBE_GROUPS_REQUEST_KEY = 15;
    const static int LIST_GROUPS_REQUEST_KEY = 16;

    // Message compression attribute values
    const static signed char MESSAGE_COMPRESSION_NONE = 0x00;
    const static signed char MESSAGE_COMPRESSION_GZIP = 0x01;
    const static signed char MESSAGE_COMPRESSION_SNAPPY = 0x02;

    // API error codes
    const static int ERRORCODE_UNKNOWN = -1;
    const static int ERRORCODE_NO_ERROR = 0;
    const static int ERRORCODE_OFFSET_OUT_OF_RANGE = 1;
    const static int ERRORCODE_INVALID_MESSAGE = 2;
    const static int ERRORCODE_UNKNOWN_TOPIC_OR_PARTITION = 3;
    const static int ERRORCODE_INVALID_MESSAGE_SIZE = 4;
    const static int ERRORCODE_LEADER_NOT_AVAILABLE = 5;
    const static int ERRORCODE_NOT_LEADER_FOR_PARTITION = 6;
    const static int ERRORCODE_REQUEST_TIMED_OUT = 7;
    const static int ERRORCODE_BROKER_NOT_AVAILABLE = 8;
    const static int ERRORCODE_REPLICA_NOT_AVAILABLE = 9;
    const static int ERRORCODE_MESSAGE_SIZE_TOO_LARGE = 10;
    const static int ERRORCODE_STALE_CONTROLLER_EPOCH_CODE = 11;
    const static int ERRORCODE_OFFSET_METADATA_TOO_LARGE_CODE = 12;

    const static int ERRORCODE_GROUP_LOAD_IN_PROCESS = 14;
    const static int ERRORCODE_GROUP_COORDINATOR_NOT_AVAIBLE = 15;
    const static int ERRORCODE_GROUP_NOT_COORDINATOR = 16;
    const static int ERRORCODE_INVALID_TOPIC = 17;
    const static int ERRORCODE_RECORD_LIST_TOO_LARGE = 18;
    const static int ERRORCODE_NOT_ENOUGH_REPLICAS = 19;
    const static int ERRORCODE_NOT_ENOUGH_REPLICAS_AFTER_APPEND = 20;
    const static int ERRORCODE_INVALID_REQUIRED_ACKS= 21;
    const static int ERRORCODE_ILLEGAL_GENERATION = 22;
    const static int ERRORCODE_INCONSISTENT_GROUP_PROTOCOL = 23;
    const static int ERRORCODE_INVALID_GROUPID = 24;
    const static int ERRORCODE_UNKNOWN_MEMBERID = 25;
    const static int ERRORCODE_INVALID_SESSION_TIMEOUT = 26;
    const static int ERRORCODE_GROUP_REBALANCE_IN_PROGRESS = 27;
    const static int ERRORCODE_INVALID_COMMIT_OFFSET_SIZE = 28;
    const static int ERRORCODE_TOPIC_AUTH_FAILED = 29;
    const static int ERRORCODE_GROUP_AUTHRIZATION_FAILED = 30;
    const static int ERRORCODE_CLUSTER_AUTH_FAILED = 31;

    const static int ERRORCODE_MINIMUM = -1;
    const static int ERRORCODE_MAXIMUM = 31;

    // API error strings
    const static char* ERRORSTRING_NO_ERROR;
    const static char* ERRORSTRING_OFFSET_OUT_OF_RANGE;
    const static char* ERRORSTRING_INVALID_MESSAGE;
    const static char* ERRORSTRING_UNKNOWN_TOPIC_OR_PARTITION;
    const static char* ERRORSTRING_INVALID_MESSAGE_SIZE;
    const static char* ERRORSTRING_LEADER_NOT_AVAILABLE;
    const static char* ERRORSTRING_NOT_LEADER_FOR_PARTITION;
    const static char* ERRORSTRING_REQUEST_TIMED_OUT;
    const static char* ERRORSTRING_BROKER_NOT_AVAILABLE;
    const static char* ERRORSTRING_REPLICA_NOT_AVAILABLE;
    const static char* ERRORSTRING_MESSAGE_SIZE_TOO_LARGE;
    const static char* ERRORSTRING_STALE_CONTROLLER_EPOCH_CODE;
    const static char* ERRORSTRING_OFFSET_METADATA_TOO_LARGE_CODE;

    const static char* ERRORSTRING_GROUP_LOAD_IN_PROCESS;
    const static char* ERRORSTRING_GROUP_COORDINATOR_NOT_AVAIBLE;
    const static char* ERRORSTRING_GROUP_NOT_COORDINATOR;
    const static char* ERRORSTRING_INVALID_TOPIC;
    const static char* ERRORSTRING_RECORD_LIST_TOO_LARGE;
    const static char* ERRORSTRING_NOT_ENOUGH_REPLICAS;
    const static char* ERRORSTRING_NOT_ENOUGH_REPLICAS_AFTER_APPEND;
    const static char* ERRORSTRING_INVALID_REQUIRED_ACKS;
    const static char* ERRORSTRING_ILLEGAL_GENERATION;
    const static char* ERRORSTRING_INCONSISTENT_GROUP_PROTOCOL;
    const static char* ERRORSTRING_INVALID_GROUPID;
    const static char* ERRORSTRING_UNKNOWN_MEMBERID;
    const static char* ERRORSTRING_INVALID_SESSION_TIMEOUT;
    const static char* ERRORSTRING_GROUP_REBALANCE_IN_PROGRESS;
    const static char* ERRORSTRING_INVALID_COMMIT_OFFSET_SIZE;
    const static char* ERRORSTRING_TOPIC_AUTH_FAILED;
    const static char* ERRORSTRING_GROUP_AUTHRIZATION_FAILED;
    const static char* ERRORSTRING_CLUSTER_AUTH_FAILED;

    const static char* ERRORSTRING_INVALID_ERROR_CODE;
    const static char* ERRORSTRING_UNKNOWN;

public:
    static const char * errorStringLookupTable[];
    static const char *getErrorString(int errorCode);
};

}
