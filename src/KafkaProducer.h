//
// Created by vicente on 10/15/21.
//

#ifndef CUASMQ_KAFKAPRODUCER_H
#define CUASMQ_KAFKAPRODUCER_H

using cppkafka::Producer;
using cppkafka::Configuration;
using cppkafka::Topic;
using cppkafka::MessageBuilder;

class KafkaProducer {
public:
    KafkaProducer() = delete;
    KafkaProducer(const std::string& brokers){
        _config = {
                { "metadata.broker.list", brokers }
        };
        _producer = std::make_unique<cppkafka::Producer>(_config);
    }

    void publish(const std::string &topic, const std::string &msg){
        if(!_builders.contains(topic)){
            createBuilderForTopic(topic);
        }
        _builders.at(topic).payload(msg);
        _producer->produce(_builders.at(topic));
        _producer->flush();
    }

    //TODO partition_value ?
    //TODO exceptions handler (broker offline etc etc)
private:
    Configuration _config;
    using Builders = std::unordered_map<std::string, MessageBuilder>;
    Builders _builders;
    std::unique_ptr<Producer> _producer;


    void createBuilderForTopic(const std::string & topic){
        _builders.insert({topic, std::move(MessageBuilder(topic))});
    }
};


#endif //CUASMQ_KAFKAPRODUCER_H
