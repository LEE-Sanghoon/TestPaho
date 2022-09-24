/*
      https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/data_publish.cpp
      https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/mqttpp_chat.cpp
*/

#include <iostream>
#include <string>
#include <thread>
//#include <chrono>
#include <functional>
#include <vector>

#include <mqtt/client.h>


const std::string MQTT_ADDRESS{ "tcp://112.217.163.10:1883" };
const std::string MQTT_CLIENT_ID{ "MTIE1.KRISO" };
const std::string MQTT_TOPIC{ "SHIPINFORMATION/DBS/SUBSCRIBELIST" };
const std::string WILL_TOPIC{ "SHIPINFORMATION/DBS/SYSTEMSTATUS" };
const std::string WILL_MESSAGE{ "DBSINFORMATION/DBS/SYSTEMS/CONNECT" };

constexpr int QOS_0 = 0;
constexpr int QOS_1 = 1;
constexpr int QOS_2 = 2;

using handler_t = std::function<bool(const mqtt::message&)>;

// SUBSCRIBELIST
bool onSubscribeList(const mqtt::message& msg)
{
    std::cout << msg.get_topic() << ": " << msg.to_string() << std::endl;
   
    return true;
}



int main()
{
    mqtt::client client(MQTT_ADDRESS, MQTT_CLIENT_ID, mqtt::create_options(MQTTVERSION_5));

    // 접속 옵션 설정
    auto lwt = mqtt::message("DBSINFORMATION/DBS/SYSTESTATE/CONNECT", "", QOS_1, false);

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(600);
    connOpts.set_clean_session(true);
    connOpts.set_keep_alive_interval(600);
    connOpts.set_will(std::move(lwt));


    std::vector<handler_t> handler{
        onSubscribeList
    };

    try {
        mqtt::connect_response rsp = client.connect(connOpts);
        std::cout << "OK" << std::endl;

        if (!rsp.is_session_present()) {
            std::cout << "Subscribing to topics..." << std::flush;

            mqtt::subscribe_options subOpts;
            mqtt::properties props1{
                { mqtt::property::SUBSCRIPTION_IDENTIFIER, 1 },
            };
            client.subscribe("MTIE1/SHIPINFORMATION/DBS/SUBSCRIBELIST/@FIRST", QOS_1, subOpts, props1);
        }
        else {
            std::cout << "Session already present. Skipping subscribe." << std::endl;
        }

        //client.publish("SHIPINFORMATION/DBS/SUBSCRIBELIST");


        while (true) {
            auto msg = client.consume_message();

            if (msg) {

                int subId = mqtt::get<int>(msg->get_properties(),
                    mqtt::property::SUBSCRIPTION_IDENTIFIER);

                if (!(handler[subId - 1])(*msg))
                    break;
            }
            else if (!client.is_connected()) {
                std::cout << "Lost connection" << std::endl;
                while (!client.is_connected()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
                std::cout << "Re-established connection" << std::endl;
            }
        }


        // 연결 끊기
        client.disconnect();
        std::cout << "Disconnected" << std::endl;
    }
    catch (const mqtt::persistence_exception& exc)
    {
        std::cerr << "Persistence Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;

        return -1;
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;

        return -1;
    }

    return 0;
}