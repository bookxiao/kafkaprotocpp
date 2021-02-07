# kafkaprotocpp

Kafka协议C++版本的不完全实现。

本库节选自个人实现的KafkaSdk，仅包括Kafka协议的编解码实现，不包括连接管理，以及 Consumer 或 Producer 模式的完全实现。

只针对1.0版本Kafka实现了特定版本的协议，仅用于学习Kafka协议。

## 安装

环境：Ubuntu12.04 以上，g++4.8以上（支持C++11）

下载后执行`make`即可生成静态库`libkafkaprotocpp.a`

## 示例

`examples/meta_query.cpp`发送`MetadataRequest`查询集群Broker列表和topic信息，其它协议使用类似方法测试即可。
