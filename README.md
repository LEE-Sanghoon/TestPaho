# Paho MQTT C++ 테스트 코드 작성 


### MQTT Broker 설치

```console
$ sudo apt update && sudo apt install mosquitto mosquitto-clients -y
$ sudo systemctl status mosquitto
```

### Paho MQTT C++ 라이브러리 설치

* 사전 준비
```console
$ sudo apt install build-essential gcc make cmake git libssh-dev doxygen
```

* 설치
```console
$ git clone https://github.com/eclipse/paho.mqtt.cpp
$ cd paho.mqtt.cpp
$ sudo ./install_paho_mqtt_c.sh
$ cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
$ sudo cmake --build build/ --target install
$ sudo ldconfig
```

* 확인
```console
$ ls /usr/local/lib
```