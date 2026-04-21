#ifndef GARAGE_DOOR_MQTT_H
#define GARAGE_DOOR_MQTT_H
#include <string>
#include "Countdown.h"
#include "IPStack.h"
#include "MQTTPacket.h"
#include "paho.mqtt.embedded-c/MQTTClient/src/MQTTClient.h"

class MQTTConnection {
	public:
		MQTTConnection(const std::string &ssid, const std::string &pwd);
		~MQTTConnection() = default;

		void connect();
		void client_yield();
		bool mqtt_is_connected() const;

		void send_message(const char* topic,const std::string &msg);
		void subscribe(const char* topic);
		void publish(const char* topic,const std::string &msg);

		static void set_command_handler(void (*handler)(const char*));

	private:
		void connect_tcp();
		void connect_mqtt();
		static void static_message_arrived(MQTT::MessageData& md);

		std::string ssid_;
		std::string pwd_;
		std::string broker_ip_;

		int port_;
		bool tcp_is_connected_;
		bool mqtt_is_connected_;
		int rc_;

		IPStack ipstack_;
		MQTT::Client<IPStack,Countdown> client;
		MQTTPacket_connectData data_;

		absolute_time_t mqtt_send_;
		absolute_time_t yield_timer_;

		static void (*command_handler)(const char*);
};
#endif //GARAGE_DOOR_MQTT_H