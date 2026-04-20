#include "MQTTService.h"
#include <string>
#include <iostream>
#include "private.h"

#define MAX_RETRIES 5

MQTTService::MQTTService(const string &ssid, const string &pwd) :
ssid(ssid), pwd(pwd), ipstack(ssid.c_str(), pwd.c_str()),client(ipstack), data(), rc() {
	broker_ip = HOME_IP; //CLASS_IP
	port = 1883;
	data = MQTTPacket_connectData_initializer;
	data.MQTTVersion = 3;
	data.clientID.cstring = const_cast<char *>("PicoW-sample");
}

void MQTTService::connect_mqtt(){
	int retry_count = 1;
	if (!tcp_is_connect)
	{
		cout << "TCP is not connected" << endl;
		return;
	}
	rc = client.connect(data);
	while (rc != 0 && retry_count <= MAX_RETRIES){
		rc = client.connect(data);
		cout << "Connect to MQTT failed. Attempt " << retry_count << endl;
		retry_count++;
		cyw43_arch_poll();
		sleep_ms(100);
	}
	if (retry_count > MAX_RETRIES){
		cout << "MQTT connect failed after 5 attempts." << endl;
	}
	if (client.isConnected())
	{
		cout << "MQTT is connected." << endl;
		mqtt_is_connect = true;
	}
}

void MQTTService::connect_tcp(){
	int retry_count = 0;
	while (retry_count < MAX_RETRIES) {
		if (retry_count > 0) {
			ipstack.disconnect();
			sleep_ms(500);
		}

		rc = ipstack.connect(broker_ip.c_str(), port);
		if (rc == ERR_OK) {
			cout << "TCP initiating connection..." << endl;
			// tcp_connect() is async: poll the WiFi stack until the
			// tcp_client_connected callback fires (up to 3 seconds)
			auto timeout = make_timeout_time_ms(3000);
			while (!time_reached(timeout)) {
				cyw43_arch_poll();
				sleep_ms(10);
			}
			tcp_is_connect = true;
			return;
		}
		cout << "TCP connect failed. Attempt " << retry_count + 1 << endl;
		retry_count++;
	}

	cout << "TCP connect failed after " << MAX_RETRIES
		 << " attempts. Check broker IP and Wi-Fi." << endl;
}

void MQTTService::subscribe(const char* topic){
	rc = client.subscribe(topic, MQTT::QOS1, static_message_arrived);
	if (rc != 0){
		cout << "rc from MQTT subscribe is " << rc << endl;
	}
	cout << "MQTT subsribed" << endl;
}

void MQTTService::publish(const string &msg, const char* topic){
	if (!tcp_is_connect || !mqtt_is_connect) return;
	char buf[256];
	strncpy(buf, msg.c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = '\0';

	MQTT::Message message{};
	message.retained   = false;
	message.dup        = false;
	message.qos        = MQTT::QOS1;
	message.payload    = static_cast<void*>(buf);
	message.payloadlen = strlen(buf);

	rc = client.publish(topic, message);
	if (rc != 0) {
		cout << "[MQTT] Publish failed, rc=" << rc << endl;
	}
}

void (*MQTTService::command_handler)(const char*) = nullptr;
void MQTTService::set_command_handler(void (*handler)(const char*)){
	command_handler = handler;
}

void MQTTService::static_message_arrived(MQTT::MessageData& md) {
	char buf[256] = {};
	size_t len = md.message.payloadlen < sizeof(buf)-1
				 ? md.message.payloadlen : sizeof(buf)-1;
	memcpy(buf, md.message.payload, len);
	buf[len] = '\0';

	cout << "[MQTT] Command received: " << buf << endl;
	if (command_handler != nullptr) {
		command_handler(buf);
	}

}

void MQTTService::send_message(const string &msg, const char* topic){
	if (!tcp_is_connect || !mqtt_is_connect) return;

	if (time_reached(mqtt_send)){
		mqtt_send = delayed_by_ms(mqtt_send, 2000);
		char buf[100];
		strncpy(buf, msg.c_str(), sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		MQTT::Message message{};
		message.retained   = false;
		message.dup        = false;
		message.qos        = MQTT::QOS1;
		message.payload    = static_cast<void*>(buf);
		message.payloadlen = strlen(buf);

		rc = client.publish(topic, message);
		cout << "Publish rc = " << rc << endl;
	}
}

void MQTTService::client_yield() {
	cyw43_arch_poll(); // called every loop to keep Wi-Fi stack alive
	if (tcp_is_connect && mqtt_is_connected())
	{
		if (time_reached(yield_timer)) {
			client.yield(1); // process incoming MQTT packets
			yield_timer = make_timeout_time_ms(50);
		}
	}
}


bool MQTTService::mqtt_is_connected() const{
	return mqtt_is_connect;
}

