#include <string>
#include <iostream>
#include "MQTTConnection.h"
#include "private.h"

#define MAX_RETRIES 5

//Initialized to nullptr to avoid dangling pointer execution before setup.
void (*MQTTConnection::command_handler)(const char*) = nullptr;

MQTTConnection::MQTTConnection(const std::string &ssid, const std::string &pwd) :
	ssid_(ssid), pwd_(pwd), ipstack_(ssid.c_str(), pwd.c_str()),
	client(ipstack_), tcp_is_connected_(false), mqtt_is_connected_(false), rc_(0) {
	broker_ip_ = HOME_IP; //CLASS_IP
	port_ = 1883;
	data_ = MQTTPacket_connectData_initializer;
	data_.MQTTVersion = 3;
	data_.clientID.cstring = const_cast<char *>("PicoW-sample");
	mqtt_send_  = make_timeout_time_ms(2000);
	yield_timer_ = make_timeout_time_ms(50);
}

void MQTTConnection::connect() {
	connect_tcp();
	if (tcp_is_connected_) {
		connect_mqtt();
	}
}

void MQTTConnection::connect_tcp(){
	int retry_count = 0;
	while (retry_count < MAX_RETRIES) {
		if (retry_count > 0) {
			ipstack_.disconnect();
			sleep_ms(500);
		}

		rc_ = ipstack_.connect(broker_ip_.c_str(), port_);
		if (rc_ == ERR_OK) {
			std::cout << "TCP initiating connection..." << std::endl;
			// tcp_connect() is async: poll the WiFi stack until the
			// tcp_client_connected callback fires (up to 3 seconds)
			auto timeout = make_timeout_time_ms(3000);
			while (!time_reached(timeout)) {
				cyw43_arch_poll();
				sleep_ms(10);
			}
			tcp_is_connected_ = true;
			return;
		}
		std::cout << "TCP connect failed. Attempt " << retry_count + 1 << std::endl;
		retry_count++;
	}

	std::cout << "TCP connect failed after " << MAX_RETRIES
		 << " attempts. Check broker IP and Wi-Fi." << std::endl;
}

void MQTTConnection::connect_mqtt(){
	int retry_count = 1;
	rc_ = client.connect(data_);
	while (rc_ != 0 && retry_count <= MAX_RETRIES){
		rc_ = client.connect(data_);
		std::cout << "Connect to MQTT failed. Attempt " << retry_count << std::endl;
		retry_count++;
		cyw43_arch_poll();
		sleep_ms(100);
	}

	if (client.isConnected()){
		std::cout << "MQTT is connected." << std::endl;
		mqtt_is_connected_ = true;
	} else {
		std::cout << "MQTT connect failed after 5 attempts." << std::endl;
	}
}



void MQTTConnection::subscribe(const char* topic){
	rc_ = client.subscribe(topic, MQTT::QOS1, static_message_arrived);
	if (rc_ != 0){
		std::cout << "rc from MQTT subscribe is " << rc_ << std::endl;
	}else {
		std::cout << "MQTT subscribed to " << topic << std::endl;
	}
}

void MQTTConnection::publish(const char* topic,const std::string &msg){
	if (!tcp_is_connected_ || !mqtt_is_connected_) return;
	char buf[256];
	strncpy(buf, msg.c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';

	MQTT::Message message{};
	message.retained   = false;
	message.dup        = false;
	message.qos        = MQTT::QOS1;
	message.payload    = static_cast<void*>(buf);
	message.payloadlen = strlen(buf);

	rc_ = client.publish(topic, message);
	if (rc_ != 0) {
		std::cout << "[MQTT] Publish failed, rc=" << rc_ << std::endl;
	}
}


void MQTTConnection::set_command_handler(void (*handler)(const char*)){
	command_handler = handler;
}

void MQTTConnection::static_message_arrived(MQTT::MessageData& md) {
	char buf[256] = {};
	size_t len = md.message.payloadlen < sizeof(buf)-1
				 ? md.message.payloadlen : sizeof(buf)-1;
	memcpy(buf, md.message.payload, len);
	buf[len] = '\0';

	std::cout << "[MQTT] Command received: " << buf << std::endl;
	if (command_handler != nullptr) {
		command_handler(buf);
	}

}

void MQTTConnection::send_message(const char* topic,const std::string &msg){
	if (!tcp_is_connected_ || !mqtt_is_connected_) return;

	if (time_reached(mqtt_send_)){
		mqtt_send_ = delayed_by_ms(mqtt_send_, 2000);
		publish(topic,msg);
		std::cout << "Publish rc = " << rc_ << std::endl;
	}
}

void MQTTConnection::client_yield() {
	cyw43_arch_poll(); // called every loop to keep Wi-Fi stack alive
	if (tcp_is_connected_ && mqtt_is_connected_){
		if (time_reached(yield_timer_)) {
			client.yield(1); // process incoming MQTT packets
			yield_timer_ = make_timeout_time_ms(50);
		}
	}
}

bool MQTTConnection::mqtt_is_connected() const{
	return mqtt_is_connected_;
}

