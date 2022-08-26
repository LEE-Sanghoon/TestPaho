/******************************************************************************
 *
 * Find dependencies
 *     find_package(PahoMqttCpp REQUIRED)
 *     target_link_libraries(publisher pthread paho-mqtt3a paho-mqttpp3)
 * Reference
 *    https://github.com/eclipse/paho.mqtt.cpp/blob/master/src/samples/async_subscribe.cpp
 *****************************************************************************/

#include <mqtt/async_client.h>

const std::string MQTT_ADDRESS("tcp://localhost:1883");
const std::string MQTT_CLIENT_ID("");
const std::string MQTT_TOPIC("kriso/hello");

const int	QOS = 1;
const int	N_RETRY_ATTEMPTS = 5;

class action_listener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token& tok) override;

	void on_success(const mqtt::token& tok) override; 

public:
  action_listener(const std::string& name);
};



class callback : public virtual mqtt::callback,
					public virtual mqtt::iaction_listener
{
	// Counter for the number of connection retries
	int nretry_;
	// The MQTT client
	mqtt::async_client& client_;
	// Options to use if we need to reconnect
	mqtt::connect_options& connOpts_;
	// An action listener to display the result of actions.
	action_listener subListener_;


	// This deomonstrates manually reconnecting to the broker by calling
	// connect() again. This is a possibility for an application that keeps
	// a copy of it's original connect_options, or if the app wants to
	// reconnect with different options.
	// Another way this can be done manually, if using the same options, is
	// to just call the async_client::reconnect() method.
	void reconnect();

	// Re-connection failure
	void on_failure(const mqtt::token& tok) override;

	// (Re)connection success
	// Either this or connected() can be used for callbacks.
	void on_success(const mqtt::token& tok) override;

	// (Re)connection success
	void connected(const std::string& cause) override;


  // Callback for when the connection is lost.
  // This will initiate teh attempt to manually reconnect.
	void connection_lost(const std::string& cause) override;

	void message_arrived(mqtt::const_message_ptr msg) override;

	void delivery_complete(mqtt::delivery_token_ptr token) override;

public:
	callback(mqtt::async_client& client, mqtt::connect_options& connOpts);
};