/*
    https://github.com/eclipse/paho.mqtt.cpp, https://www.eclipse.org/paho/files/cppdoc/index.html
    https://github.com/open-source-parsers/jsoncpp
*/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>

#include <mqtt/async_client.h>
#include <mqtt/response_options.h>

#include "DBSPacket.hpp"

using namespace std;
using namespace std::chrono;

const std::string MQTT_ADDRESS{ "tcp://112.217.163.10:1883" };
const std::string MQTT_CLIENT_ID{ "MTIE1" };
const std::string TOPIC_CONNECTSTATE{ "DBSINFORMATION/DBS/CONNECTSTATE" };
const std::string TOPIC_SUBSCRIBELIST{ "DBSINFORMATION/DBS/SUBSCRIBELIST" };
const std::string TOPIC_RESPONSE_SUBSCRIBELIST{"MTIE1/DBSINFORMATIONDBS/SUBSCRIBELIST/@FIRST"};

constexpr int QOS_0 = 0;
constexpr int QOS_1 = 1;
constexpr int QOS_2 = 2;

const auto TIMEOUT = std::chrono::seconds(10);

//
// SUBSCRIBE TOPIC HANDLERs
// ------------------------------------------------------------------



int main()
{
 

    // Payload Sample 파일 읽어 사용하려고 했으나, make_payload(...) 함수로
    // 작성하면서 필요성이 없음. 그러나 set_json_value 함수 사용 예시로 남겨 두었음
    // string const HOME = getenv("HOME")? getenv("HOME"): ".";
    // Json::Value root = read_json_file(HOME + "/payload.json");
    // set_json_value( root, "Package/Header/Author", "MTIE1");


    mqtt::async_client client(MQTT_ADDRESS, MQTT_CLIENT_ID, 
        mqtt::create_options(MQTTVERSION_5));

    auto connOpts = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        .automatic_reconnect(seconds(2), seconds(30))
        .user_name(MQTT_CLIENT_ID)
        .clean_session(false)
        .finalize();

    try {
        
        // CONNECT PHASE :
        // - 서버에 접속하면서, will_message 전송
        // - will_topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - will_payload: { "Author": "MTIE1/CONNECTSTATE" },
        //                 { "DataSet/Value": [ "MTIE1", "FALSE" ]
        // ------------------------------------------------

        Json::Value payload = make_payload(ePayload::will_message);
        std::string strPayload = payload.toStyledString();

        // auto lwt = mqtt::will_options();
        // lwt.set_topic(TOPIC_CONNECTSTATE);
        // lwt.set_payload(payload.toStyledString());
        // lwt.set_qos(QOS_1);

        connOpts.set_will(mqtt::message(TOPIC_CONNECTSTATE, payload.toStyledString()));

        std::cout << "\nConnecting... ";
        mqtt::token_ptr conntok = client.connect(connOpts);
        std::cout << "Waiting for the connection... ";
        conntok->wait();
        std::cout << " ...OK" << std::endl;

        // STATE (connect) PUBLISH :
        // - 서버에 나의 접속을 통보
        // - topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - payload: { "Author": "MTIE1/CONNECTSTATE" },
        //            { "DataSet/Value": [ "MTIE1", "TRUE" ]
        // ------------------------------------------------
        {
            payload = make_payload(ePayload::connect_message);
            mqtt::message_ptr p_mqtt_msg = mqtt::make_message(TOPIC_CONNECTSTATE, payload.toStyledString());
            p_mqtt_msg->set_qos(1);

            std::cout << "Publish CONNECTSTATE(connect)";
            client.publish(p_mqtt_msg)->wait_for(TIMEOUT);
            std::cout << " ...OK" << std::endl;
        }
        
        // SUBSCRIBELIST PHASE :
        // - req/res 패턴으로 작성한다. 따라서 
        //   1) subscribe (respnse) topic 등록
        //   2) publish subscribelist topic 
        // ------------------------------------------------

        // 1) subscribe 등록 
        // if (!rsp.is_session_present()) {
        //     std::cout << "Subscribing to topics..." << std::flush;
        //     mqtt::subscribe_options subOpts;
    
        //     mqtt::properties response_subscribelist_prop{
        //         { mqtt::property::SUBSCRIPTION_IDENTIFIER, 1 },
        //     };
        //     client.subscribe(TOPIC_RESPONSE_SUBSCRIBELIST, QOS_1, subOpts, response_subscribelist_prop);
        // }
        // else {
        //     std::cout << "Session already present. Skipping subscribe." << std::endl;
        // }


        
        // 2) publish subscribe list (req/res pattern)
        {
            mqtt::properties props{{mqtt::property::RESPONSE_TOPIC, TOPIC_RESPONSE_SUBSCRIBELIST}};

            payload = make_payload(ePayload::request_subscribelist_message);
            mqtt::message_ptr p_mqtt_msg = mqtt::make_message(TOPIC_SUBSCRIBELIST, payload.toStyledString());
            p_mqtt_msg->set_qos(1);
            p_mqtt_msg->set_properties(props);

            std::cout << "Publish SUBSCRIBELIST";
            client.publish(p_mqtt_msg)->wait_for(TIMEOUT);
            std::cout << " ...OK" << std::endl;
        }


        // // PUBLISH DATA PHASE :
        // // ------------------------------------------------

        while (true) {
        //     auto msg = client.consume_message();

        //     if (msg) {

        //         int subId = mqtt::get<int>(msg->get_properties(),
        //             mqtt::property::SUBSCRIPTION_IDENTIFIER);

        //         if (!(handler[subId - 1])(*msg))
        //             break;
        //     }
        //     else if (!client.is_connected()) {
        //         cout << "Lost connection" << endl;
        //         while (!client.is_connected()) {
        //             this_thread::sleep_for(milliseconds(250));
        //         }
        //         cout << "Re-established connection" << endl;
        //     }
        }


        // STATE (disconnect) PUBLISH :
        // - 서버에 나의 단절을 통보 (will_message와 동일)
        // - topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - payload: { "Author": "MTIE1/CONNECTSTATE" },
        //            { "DataSet/Value": [ "MTIE1", "FALSE" ]
        // ------------------------------------------------
        {
            payload = make_payload(ePayload::disconnect_message);
            mqtt::message_ptr p_mqtt_msg = mqtt::make_message(TOPIC_CONNECTSTATE, payload.toStyledString());
            p_mqtt_msg->set_qos(1);

            std::cout << "Publish CONNECTSTATE(disconnect)";
            client.publish(p_mqtt_msg)->wait_for(TIMEOUT);
            std::cout << " ...OK" << std::endl;
        }

        // client.disconnect();
        // cout << "Disconnected" << endl;
    }
    catch (const mqtt::persistence_exception& exc)
    {
        cerr << "Persistence Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << endl;

        return -1;
    }
    catch (const mqtt::exception& exc)
    {
        cerr << "Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << endl;

        return -1;
    }

    return 0;
}