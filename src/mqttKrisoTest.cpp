/*
    https://github.com/eclipse/paho.mqtt.cpp, https://www.eclipse.org/paho/files/cppdoc/index.html
    https://github.com/open-source-parsers/jsoncpp

    Request Response: https://www.emqx.com/en/blog/mqtt5-request-response
*/

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>

#include <mqtt/async_client.h>
#include <mqtt/response_options.h>
#include <mqtt/message.h>

#include "DBSPacket.hpp"

using namespace std;
using namespace std::chrono;

const std::string MQTT_ADDRESS{ "tcp://112.217.163.10:1883" };
const std::string MQTT_CLIENT_ID{ "MTIE1" };
const std::string TOPIC_CONNECTSTATE{ "DBSINFORMATION/DBS/CONNECTSTATE" };
const std::string TOPIC_REQUEST_SUBSCRIBELIST{ "DBSINFORMATION/DBS/SUBSCRIBELIST" };
const std::string TOPIC_RESPONSE_SUBSCRIBELIST{"MTIE1/DBSINFORMATION/DBS/SUBSCRIBELIST/@FIRST"};
const std::string TOPIC_PUBLISH_DATA{"KASSINFORMATION/MTIE1/SITUALTIONAL1"};

constexpr int QOS_0 = 0;
constexpr int QOS_1 = 1;
constexpr int QOS_2 = 2;

const auto TIMEOUT = std::chrono::seconds(10);

//
// SUBSCRIBE TOPIC HANDLERs
// ------------------------------------------------------------------
class MqttCallback : public virtual mqtt::callback
{
public:
	void connection_lost(const string& cause) override {
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;
	}

	void delivery_complete(mqtt::delivery_token_ptr tok) override {
		std::cout << "\tDelivery complete for token: ["
			<< (tok ? tok->get_message_id() : -1) << "]";
	}

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Message arribed" << std::endl;
        std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
        std::cout << "\tpayload: '" << msg->to_string() << "'" << std::endl;
    }
};

class ActionListener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token& tok) override {
		std::cout << name_ << " failure";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		std::cout << std::endl;
	}

	void on_success(const mqtt::token& tok) override {
		std::cout << name_ << " success";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;

		auto top = tok.get_topics();
		if (top && !top->empty())
            for( int i = 0; i < top->size(); i++)
    			std::cout << "\n\ttoken topic: '" << (*top)[i];
            std::cout << std::endl;
	}

public:
	ActionListener(const std::string& name) : name_(name) {}
};


int main()
{
 

    // Payload Sample 파일 읽어 사용하려고 했으나, make_payload(...) 함수로
    // 작성하면서 필요성이 없음. 그러나 set_json_value 함수 사용 예시로 남겨 두었음
    // string const HOME = getenv("HOME")? getenv("HOME"): ".";
    // Json::Value root = read_json_file(HOME + "/payload.json");
    // set_json_value( root, "Package/Header/Author", "MTIE1");


    mqtt::async_client client(MQTT_ADDRESS, MQTT_CLIENT_ID, mqtt::create_options(MQTTVERSION_5));

    MqttCallback mqtt_callback;
    client.set_callback(mqtt_callback);

    auto connect_opts = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        .user_name(MQTT_CLIENT_ID)
        .password("")
        .clean_start()
        .finalize();

    mqtt::token_ptr connect_token;
    
    try {
        
        // CONNECT PHASE :
        // - 서버에 접속하면서, will_message 전송
        // - will_topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - will_payload: { "Author": "MTIE1/CONNECTSTATE" },
        //                 { "DataSet/Value": [ "MTIE1", "FALSE" ]
        // ------------------------------------------------
        {
            Json::Value payload = make_payload(ePayload::will_message);
            connect_opts.set_will(mqtt::message(TOPIC_CONNECTSTATE, payload.toStyledString()));

            mqtt::properties props {
                { mqtt::property::REQUEST_RESPONSE_INFORMATION, true}
            };            
            connect_opts.set_properties(props);

  
            std::cout << "\nConnect ";
            connect_token = client.connect(connect_opts);
            std::cout << "Waiting for the connection... ";
            connect_token->wait();
            std::cout << " ...OK: " 
                << connect_token->get_return_code()   // 0 : No Error
                << std::endl;
        }

        // STATE (connect) PUBLISH :
        // - 서버에 나의 접속을 통보
        // - topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - payload: { "Author": "MTIE1/CONNECTSTATE" },
        //            { "DataSet/Value": [ "MTIE1", "TRUE" ]
        // ------------------------------------------------
        {
            Json::Value payload = make_payload(ePayload::connect_message);

            mqtt::properties props {
            };

            auto pubmsg = mqtt::message_ptr_builder()
                .qos(QOS_1)
                .topic(TOPIC_CONNECTSTATE)
                .payload(payload.toStyledString())
                .properties(props)
                .finalize();

            std::cout << "\nPublish CONNECTSTATE(connect)";
            mqtt::token_ptr p_token = client.publish(pubmsg);
            p_token->wait();
            std::cout << "...OK: "
                << p_token->get_return_code()
                << std::endl;
        }

        ActionListener subscribelist_listener("SUBSCRIBELIST");

        // SUBSCRIBELIST PHASE :
        // - req/res 패턴으로 작성한다. 따라서 
        //   1) subscribe (respnse) topic 등록
        //   2) publish subscribelist topic 
        // ------------------------------------------------

        // 1) subscribe 등록 
        {
            auto rsp = connect_token->get_connect_response();

            if (!rsp.is_session_present()) {
                std::cout << "\nSubscribe TOPIC: ";
                mqtt::token_ptr p_token = client.subscribe(TOPIC_RESPONSE_SUBSCRIBELIST, QOS_1, nullptr, subscribelist_listener);
                p_token->wait();
            }
        }


        
        // 2) publish subscribelist (req/res pattern)
        {
            Json::Value payload = make_payload(ePayload::request_subscribelist_message);

            mqtt::properties props {
                { mqtt::property::RESPONSE_TOPIC, TOPIC_RESPONSE_SUBSCRIBELIST }
            };

            auto pubmsg = mqtt::message_ptr_builder()
                .qos(QOS_1)
                .topic(TOPIC_REQUEST_SUBSCRIBELIST)
                .payload(payload.toStyledString())
                .properties(props)
                .finalize();

            std::cout << "\nPublish SUBSCRIBELIST";
            client.publish(pubmsg)->wait_for(TIMEOUT);
        }


        // // PUBLISH DATA PHASE :
        // // ------------------------------------------------

        while (true) {
            this_thread::sleep_for(seconds(10));

            {
                Json::Value payload = make_payload(ePayload::publish_data_message);

                mqtt::properties props {
                };

                auto pubmsg = mqtt::message_ptr_builder()
                    .qos(QOS_1)
                    .topic(TOPIC_PUBLISH_DATA)
                    .payload(payload.toStyledString())
                    .properties(props)
                    .finalize();

                std::cout << "\npublish DATA";
                client.publish(pubmsg)->wait_for(TIMEOUT);
            }
        }


        // STATE (disconnect) PUBLISH :
        // - 서버에 나의 단절을 통보 (will_message와 동일)
        // - topic: DBSINFORMATION/DBS/CONNECTSTATE
        // - payload: { "Author": "MTIE1/CONNECTSTATE" },
        //            { "DataSet/Value": [ "MTIE1", "FALSE" ]
        // ------------------------------------------------
        {
            Json::Value payload = make_payload(ePayload::disconnect_message);

            mqtt::properties props {
            };

            auto pubmsg = mqtt::message_ptr_builder()
                .qos(QOS_1)
                .topic(TOPIC_CONNECTSTATE)
                .payload(payload.toStyledString())
                .properties(props)
                .finalize();

            std::cout << "Publish CONNECTSTATE(disconnect)";
            mqtt::token_ptr p_token = client.publish(pubmsg);
            p_token->wait();
            std::cout << " ...OK" << std::endl;
        }

        client.disconnect();
        cout << "Disconnected" << endl;
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