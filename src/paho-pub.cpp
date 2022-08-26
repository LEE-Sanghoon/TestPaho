/*
    // mosquitto 설치와, Paho library 설치는 보고서 참조

    // 검증방법: 터미널에서 다음 명령 수행 후, 프로그램을 실행하면 화면에 메시지("HELLO")가 출력됨
    $ mosquitto_sub -h localhost -t kriso/hello
*/

#include <iostream>
#include <string>

#include <mqtt/client.h>

const std::string MQTT_ADDRESS {"tcp://localhost:1883"};
const std::string MQTT_CLIENT_ID {""};
const std::string MQTT_TOPIC {"kriso/hello"};

int main(int, char**) {
    
    // mqtt client 생성
    mqtt::client client(MQTT_ADDRESS, MQTT_CLIENT_ID);

    // 접속 옵션 설정
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    // 접속
    client.connect(connOpts);

    // TOPIC 발행
    auto mqtt_msg = mqtt::make_message(MQTT_TOPIC, "HELLO");
    mqtt_msg->set_qos(1);
    client.publish(mqtt_msg);

    // 접속 종료
    client.disconnect();

    return 0;
}
