cmake_minimum_required(VERSION 3.0.0)
project(TestPaho VERSION 0.1.0)

include(CTest)
enable_testing()

# Find dependencies
find_package(PahoMqttCpp REQUIRED)

add_executable(publisher src/paho-pub.cpp)
target_link_libraries(publisher pthread paho-mqtt3a paho-mqttpp3)

add_executable(subscriber src/paho-sub.cpp src/mqttclient.cpp)
target_link_libraries(subscriber pthread paho-mqtt3a paho-mqttpp3)

add_executable(MQTT.KRISO.TEST src/mqttKrisoTest.cpp src/DBSPacket.cpp src/jsoncpp.cpp)
target_link_libraries(MQTT.KRISO.TEST pthread paho-mqtt3a paho-mqttpp3)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
