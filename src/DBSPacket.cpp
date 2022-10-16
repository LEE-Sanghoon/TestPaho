/*
    https://en.wikibooks.org/wiki/JsonCpp
*/

#include <iostream>
#include <string>
#include <time.h>

#include "DBSPacket.hpp"

using namespace std;

#define NUM_ITEMS 10

Json::Value make_payload( int iMsgType)
{
  Json::Value root;
  Json::Value package;
  Json::Value header;
  Json::Value timeserisedata;

  // 템플릿 작성
  // ----------------------------------------------------------------
  header["ShipID"] = "1234567";
  
  header["TimeSpan"]["Start"] = get_current_time();
  header["TimeSpan"]["End"] = get_current_time();
  
  header["DateCreated"] = get_current_time();
  header["DateModified"] = get_current_time();
  header["Author"] = "";
  header["SystemConfiguration"] = Json::Value(Json::arrayValue);

  timeserisedata["DataConfiguration"]["ID"] = "";
  timeserisedata["DataConfiguration"]["TimeStamp"] = get_current_time();
  timeserisedata["TabularData"] = Json::Value(Json::arrayValue);
 
  // PAYLOAD 별 실제 데이터 작성
  // ----------------------------------------------------------------
  switch (iMsgType) 
  {
    case ePayload::will_message: {
      header["Author"] = "MTIE1/CONNECTSTATE";
      
      timeserisedata["DataConfiguration"]["ID"] = "DIGITALBRIDGE";
      timeserisedata["TabularData"][0]["NumberOfDataSet"] = "1";
      timeserisedata["TabularData"][0]["NumberOfDataChannel"] = "2";
      timeserisedata["TabularData"][0]["DataChannelID"].append("USERNAME");
      timeserisedata["TabularData"][0]["DataChannelID"].append("CONNECT");
      
      Json::Value data;
      data["TimeStamp"] = get_current_time();
      data["Value"].append("MTIE1");
      data["Value"].append("FLASE");    // WILL_MESSAGE는 연결이 끊길 때 사용될 (disconnect 와 동일 payload 사용) 
      data["Quality"];
      timeserisedata["TabularData"][0]["DataSet"].append(data);
      timeserisedata["EventData"]["NumberOfDataSet"] = "";
      timeserisedata["EventData"]["DataSet"] = Json::Value(Json::arrayValue); 
      break;
    }

    case ePayload::connect_message: {
      header["Author"] = "MTIE1/CONNECTSTATE";
      
      timeserisedata["DataConfiguration"]["ID"] = "DIGITALBRIDGE";
      timeserisedata["TabularData"][0]["NumberOfDataSet"] = "1";
      timeserisedata["TabularData"][0]["NumberOfDataChannel"] = "2";
      timeserisedata["TabularData"][0]["DataChannelID"].append("USERNAME");
      timeserisedata["TabularData"][0]["DataChannelID"].append("CONNECT");
      
      Json::Value data;
      data["TimeStamp"] = get_current_time();
      data["Value"].append("MTIE1");
      data["Value"].append("TRUE");   // 접속으로 TRUE...
      data["Quality"];
      timeserisedata["TabularData"][0]["DataSet"].append(data);
      timeserisedata["EventData"]["NumberOfDataSet"] = "";
      timeserisedata["EventData"]["DataSet"] = Json::Value(Json::arrayValue); 
      break;
    }

    case ePayload::disconnect_message: {
      header["Author"] = "MTIE1/CONNECTSTATE";
      
      timeserisedata["DataConfiguration"]["ID"] = "DIGITALBRIDGE";
      timeserisedata["TabularData"][0]["NumberOfDataSet"] = "1";
      timeserisedata["TabularData"][0]["NumberOfDataChannel"] = "2";
      timeserisedata["TabularData"][0]["DataChannelID"].append("USERNAME");
      timeserisedata["TabularData"][0]["DataChannelID"].append("CONNECT");
      
      Json::Value data;
      data["TimeStamp"] = get_current_time();
      data["Value"].append("MTIE1");
      data["Value"].append("FLASE");    // 끊길때 사용되므로 FALSE 
      data["Quality"];
      timeserisedata["TabularData"][0]["DataSet"].append(data);
      timeserisedata["EventData"]["NumberOfDataSet"] = "";
      timeserisedata["EventData"]["DataSet"] = Json::Value(Json::arrayValue); 
      break;
    }

    case ePayload::publish_data_message: {
      header["Author"] = "MTIE1/SITUATIONAL1";
      
      timeserisedata["DataConfiguration"]["ID"] = "DataChannelID_Topic";
      timeserisedata["TabularData"][0]["NumberOfDataSet"] = "1";
      timeserisedata["TabularData"][0]["NumberOfDataChannel"] = "11";  // (1), (2)의 배열 갯수(두 배열의 크기는 동일함)
      // (1) ------------------------------------------------------------------------------------
      // timeserisedata["TabularData"][0]["DataChannelID"].append("");
      timeserisedata["TabularData"][0]["DataChannelID"].append("MESSAGEID");
      timeserisedata["TabularData"][0]["DataChannelID"].append("COURSEOVERGROUNDDEGREESTRUE");
      timeserisedata["TabularData"][0]["DataChannelID"].append("TCOURSEOVERGROUNDDEGREESTRUE");
      timeserisedata["TabularData"][0]["DataChannelID"].append("COURSEOVERGROUNDDEGREESMAGNETIC");
      timeserisedata["TabularData"][0]["DataChannelID"].append("MCOURSEOVERGROUNDDEGREESMAGNETIC");
      timeserisedata["TabularData"][0]["DataChannelID"].append("SPEEDOVERGROUNDKNOTS");
      timeserisedata["TabularData"][0]["DataChannelID"].append("NSPEEDOVERGROUNDKNOTS");
      timeserisedata["TabularData"][0]["DataChannelID"].append("SPEEDOVERGROUNDKMPERHOUR");
      timeserisedata["TabularData"][0]["DataChannelID"].append("KSPEEDOVERGROUNDKMPERHOUR");
      timeserisedata["TabularData"][0]["DataChannelID"].append("MODEINDICATOR");
      timeserisedata["TabularData"][0]["DataChannelID"].append("CHECKSUM");
      // ----------------------------------------------------------------------------------------

      Json::Value data;
      data["TimeStamp"] = get_current_time();
      // (2) ------------------------------------------------------------------------------------
      // data["Value"].append("");
      data["Value"].append("$EIVTG");
      data["Value"].append("0.0");
      data["Value"].append("T");
      data["Value"].append("");
      data["Value"].append("");
      data["Value"].append("0.0");
      data["Value"].append("N");
      data["Value"].append("0.0");
      data["Value"].append("K");
      data["Value"].append("A");
      data["Value"].append("5B");
      // ---------------------------------------------------------------------------------------- 
      data["Quality"];
      timeserisedata["TabularData"][0]["DataSet"].append(data);
      timeserisedata["EventData"]["NumberOfDataSet"] = "";
      timeserisedata["EventData"]["DataSet"] = Json::Value(Json::arrayValue); 
      break;
    }

    case ePayload::request_subscribelist_message: {
      header["Author"] = "MTIE1/SUBSCRIBELIST";
      
      timeserisedata["DataConfiguration"]["ID"] = "DIGITALBRIDGE";
      timeserisedata["TabularData"][0]["NumberOfDataSet"] = "1";
      timeserisedata["TabularData"][0]["NumberOfDataChannel"] = "2";
      timeserisedata["TabularData"][0]["DataChannelID"].append("USERNAME");
      timeserisedata["TabularData"][0]["DataChannelID"].append("PASSWORD");
      
      Json::Value data;
      data["TimeStamp"] = get_current_time();
      data["Value"].append("MTIE1");
      data["Value"].append("");
      data["Quality"];
      timeserisedata["TabularData"][0]["DataSet"].append(data);
      timeserisedata["EventData"]["NumberOfDataSet"] = "";
      timeserisedata["EventData"]["DataSet"] = Json::Value(Json::arrayValue); 
      break;
    }

    default:
      cout << "make an empty payload" << endl;
      break;
  }

  // PAYLOAD 완성
  // ----------------------------------------------------------------  
  package["Header"] = header;
  package["TimeSeriseData"].append(timeserisedata);
  root["Package"] = package;

  // // 테스트 출력 코드:
  // cout << "======== TRACE LOG : " << endl; 
  // cout << root.toStyledString() << endl;

  return root;
}

Json::Value read_json_file (string path = "")
{
  Json::Value root;

  ifstream fJson( path, ifstream::binary);
  if (!fJson.fail())
  {
    fJson >> root;
  }
  else
  {
    cerr << "Can't load " << path << endl;
  }

  fJson.close();

  return root;
}


bool set_json_value(Json::Value root, string const key, string const value)
{
  string path = key;
  string token;
  string delimiter("/");
  size_t pos = 0;

  Json::Value& val = root;

  while ((pos = path.find(delimiter)) != string::npos) {
    token = path.substr(0, pos);
    cout << token << endl;
    val = val[token];
    path.erase( 0, pos + delimiter.length()); 
  }
  val = val[path];

  
  cout << "LAST KEY:" <<  val.asString() << endl;

  return true;
}


Json::Value make_topic_path()
{
  Json::Value root;

  string const MAIN_COMPONENT("KASSINFORMATION");
  string const SUB_COMPONENT("MTIE1");
  string const ITEMS[NUM_ITEMS] = { 
      "NUMBEROFOBJECTS",
      "MMSI",
      "LATITUDE",
      "LONGITUDE",
      "COG",
      "SOG",
      "IDENTIFICATIONTYPE",
      "DCPA",
      "TCPA",
      "DEGREEOFCOLLISIONRISK"     
    };

    for( int i = 0; i < NUM_ITEMS; i++)
    {
      Json::Value topic_path;
      topic_path["TOPIC_PATH"] = Json::Value(MAIN_COMPONENT + "/" + SUB_COMPONENT + "/" + ITEMS[i]);
      root.append(topic_path);
    }

  return root;
}

string get_current_time() 
{
  string str;

  time_t curTime = time(NULL);
  struct tm *pLocal = localtime(&curTime);
  
  str += to_string(pLocal->tm_year + 1900);
  str += "-" + to_string(pLocal->tm_mon + 1);
  str += "-" + to_string(pLocal->tm_mday);
  str += "T" + to_string(pLocal->tm_hour);
  str += ":" + to_string(pLocal->tm_min);
  str += ":" + to_string(pLocal->tm_sec);
  str += "+09:00";

  return str;
}