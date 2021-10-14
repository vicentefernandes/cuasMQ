//
// Created by vicente on 10/10/21.
//

#include <stdexcept>
#include <iostream>
#include <any>
#include <type_traits>
#include <utility>
#include <csignal>
#include <thread>
#include "../include/cppkafka/consumer.h"
#include "../include/cppkafka/configuration.h"

#include <memory>
#include <functional>

#ifndef CPPKAFKA_KAFKACONSUMER_H
#define CPPKAFKA_KAFKACONSUMER_H

using cppkafka::Consumer;
using cppkafka::Configuration;
using cppkafka::Message;
using cppkafka::TopicPartitionList;

using std::string;
using std::exception;
using std::cout;
using std::endl;


/*
template <typename T>
void variadic_vector_emplace(std::vector<T>&) {}

template <typename T, typename First, typename... Args>
void variadic_vector_emplace(std::vector<T>& v, First&& first, Args&&... args)
{
    v.emplace_back(std::forward<First>(first));
    variadic_vector_emplace(v, std::forward<Args>(args)...);
}
*/

class KafkaConsumer {
public:
    KafkaConsumer() = delete;
    KafkaConsumer(const std::string& brokers, const int groupId) : _running(false) {
        _config = {
                { "metadata.broker.list", brokers },
                { "group.id", groupId },
                { "enable.auto.commit", false }
        };
        _consumer = std::make_shared<cppkafka::Consumer>(_config);
        _consumer->set_assignment_callback([](const TopicPartitionList& partitions) {
            cout << "Got assigned: " << partitions << endl;
            //TODO
        });

        _consumer->set_revocation_callback([](const TopicPartitionList& partitions) {
            cout << "Got revoked: " << partitions << endl;
            //TODO
        });
    }

    template<typename Function, typename... Args>
    void subscribe(const std::string & topic, Function&& callback, Args... args){
        _topics.emplace_back(topic);

        ///_callbacksArgs[topic] = {};
        ///variadic_vector_emplace(_callbacks[topic].second, std::forward<Args>(args)...);

        _callbacks[topic].second = {args...};

        link(topic, callback);
        _consumer->subscribe(_topics);
    }

    void stop(){
        _running=false;
    }

    void process(){
        _running = true;
        std::thread(
                [&](){
                    while (_running) {
                        // Try to consume a message
                        _msg = _consumer->poll();
                        if (_msg) {
                            // If we managed to get a message
                            if (_msg.get_error()) {
                                // Ignore EOF notifications from rdkafka
                                if (!_msg.is_eof()) {
                                    cout << "[+] Received error notification: " << _msg.get_error() << endl;
                                }
                            }
                            else {
                                // Print the key (if any)
                                if (_msg.get_key()) {
                                    cout << _msg.get_key() << " -> ";
                                }
                                // Print the payload
                                cout << _msg.get_topic() << " @" << _msg.get_payload() << endl;
                                // Now commit the message
                                _consumer->commit(_msg);
                                auto topic = _msg.get_topic();

                                call(topic, std::move(std::string(_msg.get_payload())), _callbacks[topic].second.at(0));
                            }
                        }
                    }
                }
        ).detach();
    }

private:

    //to link many at once //TODO pass args
    //void newTopic(std::initializer_list<std::pair<const std::string, std::any>> il){
    //    _callbacks = std::move(std::unordered_map<std::string, std::any>(il));
    //}
    template<typename Function>
    void link(std::string key, Function&& someFunction) { _callbacks[key].first = std::any(someFunction); };

    template <typename ... Args>
    void call(std::string key, Args ... args) {
        if (_callbacks.find(key) != _callbacks.end()) {
            std::any_cast<std::add_pointer_t<void(Args ...)>>(_callbacks[key].first)(args...);
        }
    }

    template <typename T>
    using CallbacksMap = std::unordered_map<std::string , std::pair<std::any, std::vector<std::any>>>;

    std::vector<std::string> _topics;
    CallbacksMap<std::string> _callbacks;

    Configuration _config;
    std::shared_ptr<Consumer> _consumer;
    Message _msg;
    bool _running;
};

#endif //CPPKAFKA_KAFKACONSUMER_H
