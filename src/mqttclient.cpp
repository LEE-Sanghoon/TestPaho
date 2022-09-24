#include "mqttclient.hpp"


void action_listener::on_failure(const mqtt::token& tok) {
    std::cout << name_ << " failure";

  if (tok.get_message_id() != 0)
    std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
  std::cout << std::endl;
}


void action_listener::on_success(const mqtt::token& tok) {
  std::cout << name_ << " success";
  
  if (tok.get_message_id() != 0)
    std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
  
  auto top = tok.get_topics();
  if (top && !top->empty())
    std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
  std::cout << std::endl;
}

action_listener::action_listener(const std::string& name)
  : name_(name) {
}


void callback::reconnect() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2500));

  try {
    client_.connect(connOpts_, nullptr, *this);
  }
  catch (const mqtt::exception& exc) {
    std::cerr << "Error: " << exc.what() << std::endl;
    exit(1);
  }
}

  
void callback::on_failure(const mqtt::token& tok) {
  std::cout << "Connection attempt failed" << std::endl;

  if (++nretry_ > N_RETRY_ATTEMPTS)
    exit(1);
  reconnect();
}


void callback::on_success(const mqtt::token& tok) {

}


void callback::connected(const std::string& cause) {
  std::cout << "\nConnection success" << std::endl;

  std::cout << "\nSubscribing to topic '" << MQTT_TOPIC << "'\n"
    << "\tfor client " << MQTT_CLIENT_ID
    << " using QoS" << QOS << "\n"
    << "\nPress Q<Enter> to quit\n" << std::endl;

  client_.subscribe(MQTT_TOPIC, QOS, nullptr, subListener_);
}  


void callback::connection_lost(const std::string& cause) {
  std::cout << "\nConnection lost" << std::endl;

  if (!cause.empty())
    std::cout << "\tcause: " << cause << std::endl;

  std::cout << "Reconnecting..." << std::endl;
  nretry_ = 0;
  reconnect();
}


void callback::message_arrived(mqtt::const_message_ptr msg) {
  std::cout << "Message arrived" << std::endl;
  std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
  std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
}


void callback::delivery_complete(mqtt::delivery_token_ptr token) {

}


callback::callback(mqtt::async_client& client, mqtt::connect_options& connOpts)
  : nretry_(0), client_(client), connOpts_(connOpts), subListener_("Subscription") {

}