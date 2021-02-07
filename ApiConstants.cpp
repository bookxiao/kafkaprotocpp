#include "ApiConstants.h"

using namespace kafkaprotocpp;

const char* ApiConstants::ERRORSTRING_INVALID_ERROR_CODE = "invalid error code";
const char* ApiConstants::ERRORSTRING_UNKNOWN = "unexpected server error";
const char* ApiConstants::ERRORSTRING_NO_ERROR = "no error";
const char* ApiConstants::ERRORSTRING_OFFSET_OUT_OF_RANGE = "offset out of range";
const char* ApiConstants::ERRORSTRING_INVALID_MESSAGE = "invalid message";
const char* ApiConstants::ERRORSTRING_UNKNOWN_TOPIC_OR_PARTITION = "unknown topic or partition";
const char* ApiConstants::ERRORSTRING_INVALID_MESSAGE_SIZE = "invalid message size";
const char* ApiConstants::ERRORSTRING_LEADER_NOT_AVAILABLE = "leader not available";
const char* ApiConstants::ERRORSTRING_NOT_LEADER_FOR_PARTITION = "not leader for partition";
const char* ApiConstants::ERRORSTRING_REQUEST_TIMED_OUT = "request timed out";
const char* ApiConstants::ERRORSTRING_BROKER_NOT_AVAILABLE = "broker not available";
const char* ApiConstants::ERRORSTRING_REPLICA_NOT_AVAILABLE = "replica not available";
const char* ApiConstants::ERRORSTRING_MESSAGE_SIZE_TOO_LARGE = "message size too large";
const char* ApiConstants::ERRORSTRING_STALE_CONTROLLER_EPOCH_CODE = "stale controller epoch";
const char* ApiConstants::ERRORSTRING_OFFSET_METADATA_TOO_LARGE_CODE = "offset metadata too large";
const char* ApiConstants::ERRORSTRING_GROUP_LOAD_IN_PROCESS = "group load in process";
const char* ApiConstants::ERRORSTRING_GROUP_COORDINATOR_NOT_AVAIBLE = "group coordinator not avaible";
const char* ApiConstants::ERRORSTRING_GROUP_NOT_COORDINATOR = "broker is not coordinator";
const char* ApiConstants::ERRORSTRING_INVALID_TOPIC = "invalid topic";
const char* ApiConstants::ERRORSTRING_RECORD_LIST_TOO_LARGE = "record list too large";
const char* ApiConstants::ERRORSTRING_NOT_ENOUGH_REPLICAS = "not enough replicas";
const char* ApiConstants::ERRORSTRING_NOT_ENOUGH_REPLICAS_AFTER_APPEND = "not enough replicas after apend";
const char* ApiConstants::ERRORSTRING_INVALID_REQUIRED_ACKS = "invalid required acks";
const char* ApiConstants::ERRORSTRING_ILLEGAL_GENERATION = "illegal generationid";
const char* ApiConstants::ERRORSTRING_INCONSISTENT_GROUP_PROTOCOL = "inconsistent group protocol";
const char* ApiConstants::ERRORSTRING_INVALID_GROUPID = "invalid groupid";
const char* ApiConstants::ERRORSTRING_UNKNOWN_MEMBERID = "unknown memberid";
const char* ApiConstants::ERRORSTRING_INVALID_SESSION_TIMEOUT = "invalid session timeout value";
const char* ApiConstants::ERRORSTRING_GROUP_REBALANCE_IN_PROGRESS = "group rebalance in progress";
const char* ApiConstants::ERRORSTRING_INVALID_COMMIT_OFFSET_SIZE = "invalud commit offset size";
const char* ApiConstants::ERRORSTRING_TOPIC_AUTH_FAILED = "topic auth failed";
const char* ApiConstants::ERRORSTRING_GROUP_AUTHRIZATION_FAILED = "group auth failed";
const char* ApiConstants::ERRORSTRING_CLUSTER_AUTH_FAILED = "cluster auth failed";

const char * ApiConstants::errorStringLookupTable[] = {
    ERRORSTRING_NO_ERROR,
    ERRORSTRING_OFFSET_OUT_OF_RANGE,
    ERRORSTRING_INVALID_MESSAGE,
    ERRORSTRING_UNKNOWN_TOPIC_OR_PARTITION,
    ERRORSTRING_INVALID_MESSAGE_SIZE,
    ERRORSTRING_LEADER_NOT_AVAILABLE,
    ERRORSTRING_NOT_LEADER_FOR_PARTITION,
    ERRORSTRING_REQUEST_TIMED_OUT,
    ERRORSTRING_BROKER_NOT_AVAILABLE,
    ERRORSTRING_REPLICA_NOT_AVAILABLE,
    ERRORSTRING_MESSAGE_SIZE_TOO_LARGE,
    ERRORSTRING_STALE_CONTROLLER_EPOCH_CODE,
    ERRORSTRING_OFFSET_METADATA_TOO_LARGE_CODE,
    ERRORSTRING_UNKNOWN,
    ERRORSTRING_GROUP_LOAD_IN_PROCESS,
    ERRORSTRING_GROUP_COORDINATOR_NOT_AVAIBLE,
    ERRORSTRING_GROUP_NOT_COORDINATOR,
    ERRORSTRING_INVALID_TOPIC,
    ERRORSTRING_RECORD_LIST_TOO_LARGE,
    ERRORSTRING_NOT_ENOUGH_REPLICAS,
    ERRORSTRING_NOT_ENOUGH_REPLICAS_AFTER_APPEND,
    ERRORSTRING_INVALID_REQUIRED_ACKS,
    ERRORSTRING_ILLEGAL_GENERATION,
    ERRORSTRING_INCONSISTENT_GROUP_PROTOCOL,
    ERRORSTRING_INVALID_GROUPID,
    ERRORSTRING_UNKNOWN_MEMBERID,
    ERRORSTRING_INVALID_SESSION_TIMEOUT,
    ERRORSTRING_GROUP_REBALANCE_IN_PROGRESS,
    ERRORSTRING_INVALID_COMMIT_OFFSET_SIZE,
    ERRORSTRING_TOPIC_AUTH_FAILED,
    ERRORSTRING_GROUP_AUTHRIZATION_FAILED,
    ERRORSTRING_CLUSTER_AUTH_FAILED,
};

const char* ApiConstants::getErrorString(int errorCode)
{
    if ((errorCode < ERRORCODE_MINIMUM) || (errorCode > ERRORCODE_MAXIMUM))
        return ERRORSTRING_INVALID_ERROR_CODE;

    if (errorCode == ERRORCODE_UNKNOWN)
    {
        return ERRORSTRING_UNKNOWN;
    }

    return errorStringLookupTable[errorCode];
}
