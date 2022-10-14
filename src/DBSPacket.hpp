#ifndef __DBSPacket_hpp__
#define __DBSPacket_hpp__

#include <fstream>
#include <string>

#include "json/json.h"

enum ePayload{
  will_message = 1,                   // TOPIC: DBSINFORMATION/DBS/CONNECTSTATE
  connect_message,                    // TOPIC: DBSINFORMATION/DBS/CONNECTSTATE
  disconnect_message,                 // TOPIC: DBSINFORMATION/DBS/CONNECTSTATE
  publish_data_message,               // TOPIC: KASSINFORMATION/MTIE1/SITUATIONAL1
  request_subscribelist_message       // TOPIC: DBSINFORMATION/DBS/SUBSCRIBELIST
};

Json::Value make_payload( int iMsgType);

Json::Value read_json_file (std::string path);

bool set_json_value( Json::Value root, std::string const key, std::string const value);

Json::Value make_topic_path();

std::string get_current_time();

#endif