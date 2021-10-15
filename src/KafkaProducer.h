//
// Created by vicente on 10/15/21.
//

/*
Kafka uses Topic conception which comes to bring order into message flow.
To balance the load, a topic may be divided into multiple partitions and replicated across brokers.
Partitions are ordered, immutable sequences of messages that’s continually appended i.e. a commit log
Messages in the partition have a sequential id number that uniquely identifies each message within the partition.
Partitions allow a topic’s log to scale beyond a size that will fit on a single server (a broker) and act as the unit of parallelism.
The partitions of a topic are distributed over the brokers in the Kafka cluster where each broker handles data and requests for a share of the partitions.
Each partition is replicated across a configurable number of brokers to ensure fault tolerance.



***  https://medium.com/event-driven-utopia/understanding-kafka-topic-partitions-ae40f80552e8

 */


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


    /*
     * publish and flush the nessage to the broker
     */
    void publish(const std::string &topic, const std::string &msg){
        publish_(topic, msg);
        flush();
    }

    /*
     * Publish the message to the broker, needs flush() to sends the message
     * Ideal to publish multiple mesages with a single flush
     */
    void publishNoFlush(const std::string &topic, const std::string &msg){
        publish_(topic, msg);
    }

    /*
     * flush message to broker
     * should be used after one or multiple publishNoFlush()
     */
    void flush(){
        _producer->flush();
    }

    //TODO partition_value ?
    //TODO exceptions handler (broker offline etc etc)
private:
    void createBuilderForTopic_(const std::string & topic){
        _builders.insert({topic, std::move(MessageBuilder(topic))});
        _builders.at(topic).partition(-1);
    }
    void publish_(const std::string &topic, const std::string &msg){
        if(!_builders.contains(topic)){
            createBuilderForTopic_(topic);
        }
        _builders.at(topic).payload(msg);
        _producer->produce(_builders.at(topic));
        _producer->flush();
    }


    Configuration _config;
    using Builders = std::unordered_map<std::string, MessageBuilder>;
    Builders _builders;
    std::unique_ptr<Producer> _producer;

};


#endif //CUASMQ_KAFKAPRODUCER_H
