//
// Created by vicente on 10/15/21.
//

#include <string>
#include <iostream>
#include <producer.h>
#include "KafkaProducer.h"


using std::string;
using std::exception;
using std::cout;
using std::endl;


int main(){
    KafkaProducer kp("localhost:9092");
    kp.publish("wifi01.status", "wifi01 status message ... ");
    kp.publish("wifi01.detections", "wifi01 detection message ...");
}