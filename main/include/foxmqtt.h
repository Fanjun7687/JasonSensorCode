/*
 * This file is part of libemqtt.
 *
 * libemqtt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libemqtt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libemqtt.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *
 * Created by Filipe Varela on 09/10/16.
 * Copyright 2009 Caixa Mágica Software. All rights reserved.
 *
 * Fork developed by Vicente Ruiz Rodríguez
 * Copyright 2012 Vicente Ruiz Rodríguez <vruiz2.0@gmail.com>. All rights reserved.
 *
 */
/*********************************************************
20180122
1,split send and receive function interface

20180120
1, Export func interface

20180119 Modify By Randy
1, removed unused type define

*/
#ifndef __LIBEMQTT_H__
#define __LIBEMQTT_H__

#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "ini_file.h"

#define DEBUG_PRINT //this flag control debug information output
/*********************************************
20180120 All foxconn export function interface define here
********************************************/
//Callback interface define
typedef void (*CallbackRecMsg)(const char* topic, const char* msg);
/*
Function:
	init mqtt and read all configuration from "inifile.ini"
Ini file format as below:
---------------------------
[FOXMQTT]
clientid=client888
mqttserver=10.193.204.162
mqttport=1883
---------------------------
Result:
0---OK
-1---Fail
*/
int fox_mqtt_init_send(char* inifile);
int fox_mqtt_init_receive(char* inifile);

int fox_mqtt_init_send_param(char *clientid,char *serverip,short port);
int fox_mqtt_init_receive_param(char *clientid,char *serverip,short port);


int fox_mqtt_init_send_param_auth2(char *clientid,char *serverip,short port,char* strusr,char* strpwd);  //add by Jason with Login MQTT with user/pwd 




/*
Function:
	If mqtt init ok, this func will send topic and message.

Result:
0---OK
-1---Fail
*/
int fox_mqtt_send(const char* topic, const char* msg);



/*
Function:
	Use this func close all mqtt connection.

Result:
void
*/
void fox_mqtt_close_send();
void fox_mqtt_close_receive();




/*
Function:
	For receive use. need callback function interface.
	When topic and message received by server, will call callback func to process.

Result:
0---OK
-1---Fail
*/
int fox_mqtt_receive(const char* topic, CallbackRecMsg callbackmsg);
//////////////////////////////////////////////////////////////////////////////
////Export FOXCONN function END!!!!///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////





#ifndef MQTT_CONF_USERNAME_LENGTH
	#define MQTT_CONF_USERNAME_LENGTH 13 // Recommended by MQTT Specification (12 + '\0')
#endif

#ifndef MQTT_CONF_PASSWORD_LENGTH
	#define MQTT_CONF_PASSWORD_LENGTH 13 // Recommended by MQTT Specification (12 + '\0')
#endif


#define MQTT_MSG_CONNECT       1<<4
#define MQTT_MSG_CONNACK       2<<4
#define MQTT_MSG_PUBLISH       3<<4
#define MQTT_MSG_PUBACK        4<<4
#define MQTT_MSG_PUBREC        5<<4
#define MQTT_MSG_PUBREL        6<<4
#define MQTT_MSG_PUBCOMP       7<<4
#define MQTT_MSG_SUBSCRIBE     8<<4
#define MQTT_MSG_SUBACK        9<<4
#define MQTT_MSG_UNSUBSCRIBE  10<<4
#define MQTT_MSG_UNSUBACK     11<<4
#define MQTT_MSG_PINGREQ      12<<4
#define MQTT_MSG_PINGRESP     13<<4
#define MQTT_MSG_DISCONNECT   14<<4


#define MQTT_DUP_FLAG     1<<3
#define MQTT_QOS0_FLAG    0<<1
#define MQTT_QOS1_FLAG    1<<1
#define MQTT_QOS2_FLAG    2<<1

#define MQTT_RETAIN_FLAG  1

#define MQTT_CLEAN_SESSION  1<<1
#define MQTT_WILL_FLAG      1<<2
#define MQTT_WILL_RETAIN    1<<5
#define MQTT_USERNAME_FLAG  1<<7
#define MQTT_PASSWORD_FLAG  1<<6


/** Extract the message type from buffer.
 * @param buffer Pointer to the packet.
 *
 * @return Message Type byte.
 */
#define MQTTParseMessageType(buffer) ( *buffer & 0xF0 )

/** Indicate if it is a duplicate packet.
 * @param buffer Pointer to the packet.
 *
 * @retval   0 Not duplicate.
 * @retval !=0 Duplicate.
 */
#define MQTTParseMessageDuplicate(buffer) ( *buffer & 0x08 )

/** Extract the message QoS level.
 * @param buffer Pointer to the packet.
 *
 * @return QoS Level (0, 1 or 2).
 */
#define MQTTParseMessageQos(buffer) ( (*buffer & 0x06) >> 1 )

/** Indicate if this packet has a retain flag.
 * @param buffer Pointer to the packet.
 *
 * @retval   0 Not duplicate.
 * @retval !=0 Duplicate.
 */
#define MQTTParseMessageRetain(buffer) ( *buffer & 0x01 )


/** Parse packet buffer for number of bytes in remaining length field.
 *
 * Given a packet, return number of bytes in remaining length
 * field in MQTT fixed header.  Can be from 1 - 4 bytes depending
 * on length of message.
 *
 * @param buf Pointer to the packet.
 *
 * @retval number of bytes
 */
uint8_t mqtt_num_rem_len_bytes(const uint8_t* buf);

/** Parse packet buffer for remaning length value.
 *
 * Given a packet, return remaining length value (in fixed header).
 * 
 * @param buf Pointer to the packet.
 *
 * @retval remaining length
 */
uint16_t mqtt_parse_rem_len(const uint8_t* buf);

/** Parse packet buffer for message id.
 *
 * @param buf Pointer to the packet.
 *
 * @retval message id
 */
//uint8_t mqtt_parse_msg_id(const uint8_t* buf);

/** Parse a packet buffer for the publish topic.
 *
 * Given a packet containing an MQTT publish message,
 * return the message topic.
 *
 * @param buf Pointer to the packet.
 * @param topic  Pointer destination buffer for topic
 *
 * @retval size in bytes of topic (0 = no publish message in buffer)
 */
uint16_t mqtt_parse_pub_topic(const uint8_t* buf, uint8_t* topic);

/** Parse a packet buffer for a pointer to the publish topic.
 *
 *  Not called directly - called by mqtt_parse_pub_topic
 */
uint16_t mqtt_parse_pub_topic_ptr(const uint8_t* buf, const uint8_t** topic_ptr);

/** Parse a packet buffer for the publish message.
 *
 * Given a packet containing an MQTT publish message,
 * return the message.
 *
 * @param buf Pointer to the packet.
 * @param msg Pointer destination buffer for message
 *
 * @retval size in bytes of topic (0 = no publish message in buffer)
 */
uint16_t mqtt_parse_publish_msg(const uint8_t* buf, uint8_t* msg);

/** Parse a packet buffer for a pointer to the publish message.
 *
 *  Not called directly - called by mqtt_parse_pub_msg
 */
uint16_t mqtt_parse_pub_msg_ptr(const uint8_t* buf, const uint8_t** msg_ptr);


typedef struct {
	void* socket_info;
	int (*send)(void* socket_info, const void* buf, unsigned int count);
	// Connection info
	char clientid[50];
	// Auth fields
	char username[MQTT_CONF_USERNAME_LENGTH];
	char password[MQTT_CONF_PASSWORD_LENGTH];
	// Will topic
	uint8_t will_retain;
	uint8_t will_qos;
	uint8_t clean_session;
	// Management fields
	uint16_t seq;
	uint16_t alive;
} mqtt_broker_handle_t;


/** Initialize the information to connect to the broker.
 * @param broker Data structure that contains the connection information with the broker.
 * @param clientid A string that identifies the client id.
 *
 * @note Only has effect before to call mqtt_connect
 */
void mqtt_init(mqtt_broker_handle_t* broker, const char* clientid);

void mqtt_init_auth2(mqtt_broker_handle_t* broker, const char* clientid,char* strusr,char* strpwd);   //add by Jason with Login MQTT with user/pwd


/** Enable the authentication to connect to the broker.
 * @param broker Data structure that contains the connection information with the broker.
 * @param username A string that contains the username.
 * @param password A string that contains the password.
 *
 * @note Only has effect before to call mqtt_connect
 */
void mqtt_init_auth(mqtt_broker_handle_t* broker, const char* username, const char* password);

/** Set the keep alive timer.
 * @param broker Data structure that contains the connection information with the broker.
 * @param alive Keep aliver timer value (in seconds).
 *
 * @note Only has effect before to call mqtt_connect
 */
void mqtt_set_alive(mqtt_broker_handle_t* broker, uint16_t alive);

/** Connect to the broker.
 * @param broker Data structure that contains the connection information with the broker.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_connect(mqtt_broker_handle_t* broker);

/** Disconnect to the broker.
 * @param broker Data structure that contains the connection information with the broker.
 *
 * @note The socket must also be closed.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_disconnect(mqtt_broker_handle_t* broker);

/** Publish a message on a topic. This message will be published with 0 Qos level.
 * @param broker Data structure that contains the connection information with the broker.
 * @param topic The topic name.
 * @param msg The message.
 * @param retain Enable or disable the Retain flag (values: 0 or 1).
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_publish(mqtt_broker_handle_t* broker, const char* topic, const char* msg, uint8_t retain);

/** Publish a message on a topic.
 * @param broker Data structure that contains the connection information with the broker.
 * @param topic The topic name.
 * @param msg The message.
 * @param retain Enable or disable the Retain flag (values: 0 or 1).
 * @param qos Quality of Service (values: 0, 1 or 2)
 * @param message_id Variable that will store the Message ID, if the pointer is not NULL.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_publish_with_qos(mqtt_broker_handle_t* broker, const char* topic, const char* msg, uint8_t retain, uint8_t qos, uint16_t* message_id);

/** Send a PUBREL message. It's used for PUBLISH message with 2 QoS level.
 * @param broker Data structure that contains the connection information with the broker.
 * @param message_id Message ID
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_pubrel(mqtt_broker_handle_t* broker, uint16_t message_id);

/** Subscribe to a topic.
 * @param broker Data structure that contains the connection information with the broker.
 * @param topic The topic name.
 * @param message_id Variable that will store the Message ID, if the pointer is not NULL.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_subscribe(mqtt_broker_handle_t* broker, const char* topic, uint16_t* message_id);

/** Unsubscribe from a topic.
 * @param broker Data structure that contains the connection information with the broker.
 * @param topic The topic name.
 * @param message_id Variable that will store the Message ID, if the pointer is not NULL.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_unsubscribe(mqtt_broker_handle_t* broker, const char* topic, uint16_t* message_id);

/** Make a ping.
 * @param broker Data structure that contains the connection information with the broker.
 *
 * @retval  1 On success.
 * @retval  0 On connection error.
 * @retval -1 On IO error.
 */
int mqtt_ping(mqtt_broker_handle_t* broker);
//
//common function for pub*********************************************************
int socket_id;
int socket_id_receive;
#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];
uint8_t packet_buffer_rec[RCVBUFSIZE];

int send_packet(void* socket_info, const void* buf, unsigned int count)
{
	
	int fd = *((int*)socket_info);
	return send(fd, buf, count, 0);
}

int send_packet_receive(void* socket_info, const void* buf, unsigned int count)
{
	int fd = *((int*)socket_info);
	int ret = send(fd, buf, count, 0);
	return ret;
	//return send(fd, buf, count, 0);
}

int init_socket_send(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	int keepalive = 3; // Seconds

	// Create the socket
	if((socket_id = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// Disable Nagle Algorithm
	if (setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;

	struct sockaddr_in socket_address;
	// Create the stuff we need to connect
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	socket_address.sin_addr.s_addr = inet_addr(hostname);

	// Connect the socket
	if((connect(socket_id, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
		return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&socket_id;
	broker->send = send_packet;

	return 0;
}

int init_socket_receive(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	int keepalive = 30; // Seconds

	// Create the socket
	if((socket_id_receive = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	// Disable Nagle Algorithm
	if (setsockopt(socket_id_receive, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
		return -2;

	struct sockaddr_in socket_address;
	// Create the stuff we need to connect
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	socket_address.sin_addr.s_addr = inet_addr(hostname);

	// Connect the socket
	if((connect(socket_id_receive, (struct sockaddr*)&socket_address, sizeof(socket_address))) < 0)
		return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&socket_id_receive;
	broker->send = send_packet_receive;

	return 0;
}

int close_socket(mqtt_broker_handle_t* broker)
{
	int fd = *((int*)broker->socket_info);
	return close(fd);
}

int read_packet(int timeout)
{
	if(timeout > 0)
	{
		fd_set readfds;
		struct timeval tmv;

		// Initialize the file descriptor set
		FD_ZERO (&readfds);
		FD_SET (socket_id, &readfds);

		// Initialize the timeout data structure
		tmv.tv_sec = timeout;
		tmv.tv_usec = 0;

		// select returns 0 if timeout, 1 if input available, -1 if error
		if(select(1, &readfds, NULL, NULL, &tmv))
			return -2;
	}

	int total_bytes = 0, bytes_rcvd, packet_length;
	memset(packet_buffer, 0, sizeof(packet_buffer));

	while(total_bytes < 2) // Reading fixed header
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length

	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	return packet_length;
}

int read_packet_receive(int timeout)
{
	if(timeout > 0)
	{
		fd_set readfds;
		struct timeval tmv;

		// Initialize the file descriptor set
		FD_ZERO (&readfds);
		FD_SET (socket_id_receive, &readfds);

		// Initialize the timeout data structure
		tmv.tv_sec = timeout;
		tmv.tv_usec = 0;

		// select returns 0 if timeout, 1 if input available, -1 if error
		if(select(1, &readfds, NULL, NULL, &tmv))
			return -2;
	}

	int total_bytes = 0, bytes_rcvd, packet_length;
	memset(packet_buffer_rec, 0, sizeof(packet_buffer_rec));
	
	if((bytes_rcvd = recv(socket_id_receive, (packet_buffer_rec+total_bytes), RCVBUFSIZE, 0)) <= 0) {
		return -1;
	}

	total_bytes += bytes_rcvd; // Keep tally of total bytes
	if (total_bytes < 2)
		return -1;
	
	// now we have the full fixed header in packet_buffer
	// parse it for remaining length and number of bytes
	uint16_t rem_len = mqtt_parse_rem_len(packet_buffer_rec);
	uint8_t rem_len_bytes = mqtt_num_rem_len_bytes(packet_buffer_rec);
	
	//packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length
	// total packet length = remaining length + byte 1 of fixed header + remaning length part of fixed header
	packet_length = rem_len + rem_len_bytes + 1;

	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(socket_id_receive, (packet_buffer_rec+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}

	return packet_length;
}
//*********************************************************************

//
uint8_t mqtt_num_rem_len_bytes(const uint8_t* buf) {
	uint8_t num_bytes = 1;
	
	//printf("mqtt_num_rem_len_bytes\n");
	
	if ((buf[1] & 0x80) == 0x80) {
		num_bytes++;
		if ((buf[2] & 0x80) == 0x80) {
			num_bytes ++;
			if ((buf[3] & 0x80) == 0x80) {
				num_bytes ++;
			}
		}
	}
	return num_bytes;
}

uint16_t mqtt_parse_rem_len(const uint8_t* buf) {
	uint16_t multiplier = 1;
	uint16_t value = 0;
	uint8_t digit;
	
	//printf("mqtt_parse_rem_len\n");
	
	buf++;	// skip "flags" byte in fixed header

	do {
		digit = *buf;
		value += (digit & 127) * multiplier;
		multiplier *= 128;
		buf++;
	} while ((digit & 128) != 0);

	return value;
}

uint16_t mqtt_parse_msg_id(const uint8_t* buf) {
	uint8_t type = MQTTParseMessageType(buf);
	uint8_t qos = MQTTParseMessageQos(buf);
	uint16_t id = 0;
	
	//printf("mqtt_parse_msg_id\n");
	
	if(type >= MQTT_MSG_PUBLISH && type <= MQTT_MSG_UNSUBACK) {
		if(type == MQTT_MSG_PUBLISH) {
			if(qos != 0) {
				// fixed header length + Topic (UTF encoded)
				// = 1 for "flags" byte + rlb for length bytes + topic size
				uint8_t rlb = mqtt_num_rem_len_bytes(buf);
				uint8_t offset = *(buf+1+rlb)<<8;	// topic UTF MSB
				offset |= *(buf+1+rlb+1);			// topic UTF LSB
				offset += (1+rlb+2);					// fixed header + topic size
				id = *(buf+offset)<<8;				// id MSB
				id |= *(buf+offset+1);				// id LSB
			}
		} else {
			// fixed header length
			// 1 for "flags" byte + rlb for length bytes
			uint8_t rlb = mqtt_num_rem_len_bytes(buf);
			id = *(buf+1+rlb)<<8;	// id MSB
			id |= *(buf+1+rlb+1);	// id LSB
		}
	}
	return id;
}

uint16_t mqtt_parse_pub_topic(const uint8_t* buf, uint8_t* topic) {
	const uint8_t* ptr;
	uint16_t topic_len = mqtt_parse_pub_topic_ptr(buf, &ptr);
	
	//printf("mqtt_parse_pub_topic\n");
	
	if(topic_len != 0 && ptr != NULL) {
		memcpy(topic, ptr, topic_len);
	}
	
	return topic_len;
}

uint16_t mqtt_parse_pub_topic_ptr(const uint8_t* buf, const uint8_t **topic_ptr) {
	uint16_t len = 0;
	
	//printf("mqtt_parse_pub_topic_ptr\n");

	if(MQTTParseMessageType(buf) == MQTT_MSG_PUBLISH) {
		// fixed header length = 1 for "flags" byte + rlb for length bytes
		uint8_t rlb = mqtt_num_rem_len_bytes(buf);
		len = *(buf+1+rlb)<<8;	// MSB of topic UTF
		len |= *(buf+1+rlb+1);	// LSB of topic UTF
		// start of topic = add 1 for "flags", rlb for remaining length, 2 for UTF
		*topic_ptr = (buf + (1+rlb+2));
	} else {
		*topic_ptr = NULL;
	}
	return len;
}

uint16_t mqtt_parse_publish_msg(const uint8_t* buf, uint8_t* msg) {
	const uint8_t* ptr;
	
	//printf("mqtt_parse_publish_msg\n");
	
	uint16_t msg_len = mqtt_parse_pub_msg_ptr(buf, &ptr);
	
	if(msg_len != 0 && ptr != NULL) {
		memcpy(msg, ptr, msg_len);
	}
	
	return msg_len;
}

uint16_t mqtt_parse_pub_msg_ptr(const uint8_t* buf, const uint8_t **msg_ptr) {
	uint16_t len = 0;
	
	//printf("mqtt_parse_pub_msg_ptr\n");
	
	if(MQTTParseMessageType(buf) == MQTT_MSG_PUBLISH) {
		// message starts at
		// fixed header length + Topic (UTF encoded) + msg id (if QoS>0)
		uint8_t rlb = mqtt_num_rem_len_bytes(buf);
		uint8_t offset = (*(buf+1+rlb))<<8;	// topic UTF MSB
		offset |= *(buf+1+rlb+1);			// topic UTF LSB
		offset += (1+rlb+2);				// fixed header + topic size

		if(MQTTParseMessageQos(buf)) {
			offset += 2;					// add two bytes of msg id
		}

		*msg_ptr = (buf + offset);
				
		// offset is now pointing to start of message
		// length of the message is remaining length - variable header
		// variable header is offset - fixed header
		// fixed header is 1 + rlb
		// so, lom = remlen - (offset - (1+rlb))
      	len = mqtt_parse_rem_len(buf) - (offset-(rlb+1));
	} else {
		*msg_ptr = NULL;
	}
	return len;
}

void mqtt_init(mqtt_broker_handle_t* broker, const char* clientid) {
	// Connection options
	broker->alive = 300; // 300 seconds = 5 minutes
	broker->seq = 1; // Sequency for message indetifiers
	// Client options
	memset(broker->clientid, 0, sizeof(broker->clientid));
	memset(broker->username, 0, sizeof(broker->username));strcpy(broker->username,"admin");
	memset(broker->password, 0, sizeof(broker->password));strcpy(broker->password,"password");
	if(clientid) {
		strncpy(broker->clientid, clientid, sizeof(broker->clientid));
	} else {
		strcpy(broker->clientid, "emqtt");
	}
	// Will topic
	broker->clean_session = 1;
}

void mqtt_init_auth2(mqtt_broker_handle_t* broker, const char* clientid,char* strusr,char* strpwd) {
	// Connection options
	broker->alive = 300; // 300 seconds = 5 minutes
	broker->seq = 1; // Sequency for message indetifiers
	// Client options
	memset(broker->clientid, 0, sizeof(broker->clientid));
	memset(broker->username, 0, sizeof(broker->username));strcpy(broker->username,strusr);
	memset(broker->password, 0, sizeof(broker->password));strcpy(broker->password,strpwd);
	if(clientid) {
		strncpy(broker->clientid, clientid, sizeof(broker->clientid));
	} else {
		strcpy(broker->clientid, "emqtt");
	}
	// Will topic
	broker->clean_session = 1;
}


void mqtt_init_auth(mqtt_broker_handle_t* broker, const char* username, const char* password) {
	if(username && username[0] != '\0')
		strncpy(broker->username, username, sizeof(broker->username)-1);
	if(password && password[0] != '\0')
		strncpy(broker->password, password, sizeof(broker->password)-1);
}

void mqtt_set_alive(mqtt_broker_handle_t* broker, uint16_t alive) {
	broker->alive = alive;
}

int mqtt_connect(mqtt_broker_handle_t* broker)
{
	uint8_t flags = 0x00;

	uint16_t clientidlen = strlen(broker->clientid);
	uint16_t usernamelen = strlen(broker->username);
	uint16_t passwordlen = strlen(broker->password);
	uint16_t payload_len = clientidlen + 2;

	// Preparing the flags
	if(usernamelen) {
		payload_len += usernamelen + 2;
		flags |= MQTT_USERNAME_FLAG;
	}
	if(passwordlen) {
		payload_len += passwordlen + 2;
		flags |= MQTT_PASSWORD_FLAG;
	}
	if(broker->clean_session) {
		flags |= MQTT_CLEAN_SESSION;
	}

	// Variable header
	uint8_t var_header[] = {
		0x00,0x06,0x4d,0x51,0x49,0x73,0x64,0x70, // Protocol name: MQIsdp
		0x03, // Protocol version
		flags, // Connect flags
		broker->alive>>8, broker->alive&0xFF, // Keep alive
	};


   	// Fixed header
    uint8_t fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length
    uint8_t remainLen = sizeof(var_header)+payload_len;
    if (remainLen > 127) {
        fixedHeaderSize++;          // add an additional byte for Remaining Length
    }
    uint8_t fixed_header[fixedHeaderSize];
    
    // Message Type
    fixed_header[0] = MQTT_MSG_CONNECT;

    // Remaining Length
    if (remainLen <= 127) {
        fixed_header[1] = remainLen;
    } else {
        // first byte is remainder (mod) of 128, then set the MSB to indicate more bytes
        fixed_header[1] = remainLen % 128;
        fixed_header[1] = fixed_header[1] | 0x80;
        // second byte is number of 128s
        fixed_header[2] = remainLen / 128;
    }

	uint16_t offset = 0;
	uint8_t packet[sizeof(fixed_header)+sizeof(var_header)+payload_len];
	memset(packet, 0, sizeof(packet));
	memcpy(packet, fixed_header, sizeof(fixed_header));
	offset += sizeof(fixed_header);
	memcpy(packet+offset, var_header, sizeof(var_header));
	offset += sizeof(var_header);
	// Client ID - UTF encoded
	packet[offset++] = clientidlen>>8;
	packet[offset++] = clientidlen&0xFF;
	memcpy(packet+offset, broker->clientid, clientidlen);
	offset += clientidlen;

	if(usernamelen) {
		// Username - UTF encoded
		packet[offset++] = usernamelen>>8;
		packet[offset++] = usernamelen&0xFF;
		memcpy(packet+offset, broker->username, usernamelen);
		offset += usernamelen;
	}

	if(passwordlen) {
		// Password - UTF encoded
		packet[offset++] = passwordlen>>8;
		packet[offset++] = passwordlen&0xFF;
		memcpy(packet+offset, broker->password, passwordlen);
		offset += passwordlen;
	}

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
		return -1;
	}

	return 1;
}

int mqtt_disconnect(mqtt_broker_handle_t* broker) {
	uint8_t packet[] = {
		MQTT_MSG_DISCONNECT, // Message Type, DUP flag, QoS level, Retain
		0x00 // Remaining length
	};
	
	if(broker!=NULL)
	{
		if(broker->send != NULL)
		{
			// Send the packet
			if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
				return -1;
			}
		}
	}
	return 1;
}

int mqtt_ping(mqtt_broker_handle_t* broker) {
	uint8_t packet[] = {
		MQTT_MSG_PINGREQ, // Message Type, DUP flag, QoS level, Retain
		0x00 // Remaining length
	};

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
		return -1;
	}

	return 1;
}

int mqtt_publish(mqtt_broker_handle_t* broker, const char* topic, const char* msg, uint8_t retain) {
	return mqtt_publish_with_qos(broker, topic, msg, retain, 0, NULL);
}

int mqtt_publish_with_qos(mqtt_broker_handle_t* broker, const char* topic, const char* msg, uint8_t retain, uint8_t qos, uint16_t* message_id) {
	uint16_t topiclen = strlen(topic);
	uint16_t msglen = strlen(msg);

	uint8_t qos_flag = MQTT_QOS0_FLAG;
	uint8_t qos_size = 0; // No QoS included
	if(qos == 1) {
		qos_size = 2; // 2 bytes for QoS
		qos_flag = MQTT_QOS1_FLAG;
	}
	else if(qos == 2) {
		qos_size = 2; // 2 bytes for QoS
		qos_flag = MQTT_QOS2_FLAG;
	}
printf("=================> Here 1\n");

	// Variable header
	uint8_t var_header[topiclen+2+qos_size]; // Topic size (2 bytes), utf-encoded topic
	memset(var_header, 0, sizeof(var_header));
	var_header[0] = topiclen>>8;
	var_header[1] = topiclen&0xFF;
	memcpy(var_header+2, topic, topiclen);
	if(qos_size) {
		var_header[topiclen+2] = broker->seq>>8;
		var_header[topiclen+3] = broker->seq&0xFF;
		if(message_id) { // Returning message id
			*message_id = broker->seq;
		}
		broker->seq++;
	}
printf("=================> Here 2\n");

	// Fixed header
	// the remaining length is one byte for messages up to 127 bytes, then two bytes after that
	// actually, it can be up to 4 bytes but I'm making the assumption the embedded device will only
	// need up to two bytes of length (handles up to 16,383 (almost 16k) sized message)
	uint8_t fixedHeaderSize = 2;    // Default size = one byte Message Type + one byte Remaining Length
	uint16_t remainLen = sizeof(var_header)+msglen;
	if (remainLen > 127) {
		fixedHeaderSize++;          // add an additional byte for Remaining Length
	}
	uint8_t fixed_header[fixedHeaderSize];
    
   // Message Type, DUP flag, QoS level, Retain
   fixed_header[0] = MQTT_MSG_PUBLISH | qos_flag;
	if(retain) {
		fixed_header[0] |= MQTT_RETAIN_FLAG;
   }
   // Remaining Length
   if (remainLen <= 127) {
       fixed_header[1] = remainLen;
   } else {
       // first byte is remainder (mod) of 128, then set the MSB to indicate more bytes
       fixed_header[1] = remainLen % 128;
       fixed_header[1] = fixed_header[1] | 0x80;
       // second byte is number of 128s
       fixed_header[2] = remainLen / 128;
   }

	uint8_t packet[sizeof(fixed_header)+sizeof(var_header)+msglen];
	memset(packet, 0, sizeof(packet));
	memcpy(packet, fixed_header, sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header), var_header, sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header), msg, msglen);
printf("=================> Here 3\n");

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet))
    {
printf("=================> Here 4\n");
		return -1;
	}


	return 1;
}

int mqtt_pubrel(mqtt_broker_handle_t* broker, uint16_t message_id) {
	uint8_t packet[] = {
		MQTT_MSG_PUBREL | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		0x02, // Remaining length
		message_id>>8,
		message_id&0xFF
	};

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
		return -1;
	}

	return 1;
}

int mqtt_subscribe(mqtt_broker_handle_t* broker, const char* topic, uint16_t* message_id) {
	uint16_t topiclen = strlen(topic);

	// Variable header
	uint8_t var_header[2]; // Message ID
	var_header[0] = broker->seq>>8;
	var_header[1] = broker->seq&0xFF;
	if(message_id) { // Returning message id
		*message_id = broker->seq;
	}
	broker->seq++;

	// utf topic
	uint8_t utf_topic[topiclen+3]; // Topic size (2 bytes), utf-encoded topic, QoS byte
	memset(utf_topic, 0, sizeof(utf_topic));
	utf_topic[0] = topiclen>>8;
	utf_topic[1] = topiclen&0xFF;
	memcpy(utf_topic+2, topic, topiclen);

	// Fixed header
	uint8_t fixed_header[] = {
		MQTT_MSG_SUBSCRIBE | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		sizeof(var_header)+sizeof(utf_topic)
	};

	uint8_t packet[sizeof(var_header)+sizeof(fixed_header)+sizeof(utf_topic)];
	memset(packet, 0, sizeof(packet));
	memcpy(packet, fixed_header, sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header), var_header, sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header), utf_topic, sizeof(utf_topic));

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
		return -1;
	}

	return 1;
}

int mqtt_unsubscribe(mqtt_broker_handle_t* broker, const char* topic, uint16_t* message_id) {
	uint16_t topiclen = strlen(topic);

	// Variable header
	uint8_t var_header[2]; // Message ID
	var_header[0] = broker->seq>>8;
	var_header[1] = broker->seq&0xFF;
	if(message_id) { // Returning message id
		*message_id = broker->seq;
	}
	broker->seq++;

	// utf topic
	uint8_t utf_topic[topiclen+2]; // Topic size (2 bytes), utf-encoded topic
	memset(utf_topic, 0, sizeof(utf_topic));
	utf_topic[0] = topiclen>>8;
	utf_topic[1] = topiclen&0xFF;
	memcpy(utf_topic+2, topic, topiclen);

	// Fixed header
	uint8_t fixed_header[] = {
		MQTT_MSG_UNSUBSCRIBE | MQTT_QOS1_FLAG, // Message Type, DUP flag, QoS level, Retain
		sizeof(var_header)+sizeof(utf_topic)
	};

	uint8_t packet[sizeof(var_header)+sizeof(fixed_header)+sizeof(utf_topic)];
	memset(packet, 0, sizeof(packet));
	memcpy(packet, fixed_header, sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header), var_header, sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header), utf_topic, sizeof(utf_topic));

	// Send the packet
	if(broker->send(broker->socket_info, packet, sizeof(packet)) < sizeof(packet)) {
		return -1;
	}

	return 1;
}


/*********************************************************************
All export FOXCONN funintion interface
20180120
*/
//typedef void (*CallbackRecMsg)(const char* topic, const char* msg);
int keepalive = 30;//30;  sam debug
int send_init_ok_flag = -1;
int receive_init_ok_flag = -1;
mqtt_broker_handle_t broker_send;
mqtt_broker_handle_t broker_receive;

void alive(int sig)
{
#ifdef DEBUG_PRINT
	printf("Timeout! Sending ping...\n");
#endif
	mqtt_ping(&broker_receive);

	alarm(keepalive);
}

void term(int sig)
{
#ifdef DEBUG_PRINT
	printf("Goodbye!\n");
#endif
	// >>>>> DISCONNECT
	mqtt_disconnect(&broker_receive);
	close_socket(&broker_receive);

	exit(0);
}


int fox_mqtt_init_send_param(char *clientid,char *serverip,short port)
{
	int packet_length;
	//mqtt_broker_handle_t broker;
	send_init_ok_flag = -1;
#ifdef DEBUG_PRINT
	printf("clientid:%s\n", clientid);
	printf("server ip:%s\n", serverip);
	printf("server port:%d\n", port);
#endif
	//start init
	//mqtt_init(&broker, "sw-mqtt-pub");
	mqtt_init(&broker_send, clientid);
	//init_socket(&broker, "10.193.204.162", 1883);
	init_socket_send(&broker_send, serverip, port);

	// >>>>> CONNECT
	mqtt_connect(&broker_send);

	// <<<<< CONNACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) init_send on read packet!\n", packet_length);
		return -1;
	}

	send_init_ok_flag = 1;
	return 0;


}
int fox_mqtt_init_send_param_auth2(char *clientid,char *serverip,short port,char* usr,char* pwd)
{
	int packet_length;
	//mqtt_broker_handle_t broker;
	send_init_ok_flag = -1;
#ifdef DEBUG_PRINT
	printf("clientid:%s\n", clientid);
	printf("server ip:%s\n", serverip);
	printf("server port:%d\n", port);
#endif
	//start init
	mqtt_init_auth2(&broker_send, clientid,usr,pwd);
	init_socket_send(&broker_send, serverip, port);

	// >>>>> CONNECT
	mqtt_connect(&broker_send);

	// <<<<< CONNACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) init_send on read packet!\n", packet_length);
		return -1;
	}

	send_init_ok_flag = 1;
	return 0;


}


int fox_mqtt_init_send(char* inifile)
{
	int packet_length;
	//mqtt_broker_handle_t broker;
	send_init_ok_flag = -1;
	char fox_clientid[50];
	char fox_mqtt_server_ip[50];
	short fox_mqtt_port=1883;

	if(inifile==NULL)
		inifile = "test.ini";

	//strncpy(broker->clientid, clientid, sizeof(broker->clientid));
	strcpy(fox_clientid, GetIniKeyString("FOXMQTT","clientid",inifile));
	strcpy(fox_mqtt_server_ip, GetIniKeyString("FOXMQTT","mqttserver",inifile));
	fox_mqtt_port = GetIniKeyInt("FOXMQTT","mqttport",inifile);
#ifdef DEBUG_PRINT
	printf("clientid:%s\n", fox_clientid);
	printf("server ip:%s\n", fox_mqtt_server_ip);
	printf("server port:%d\n", fox_mqtt_port);
#endif
	//start init
	//mqtt_init(&broker, "sw-mqtt-pub");
	mqtt_init(&broker_send, fox_clientid);
	//init_socket(&broker, "10.193.204.162", 1883);
	init_socket_send(&broker_send, fox_mqtt_server_ip, fox_mqtt_port);

	// >>>>> CONNECT
	mqtt_connect(&broker_send);
	// <<<<< CONNACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) init_send on read packet!\n", packet_length);
		return -1;
	}
	send_init_ok_flag = 1;
	return 0;
}

int fox_mqtt_init_receive_param(char *clientid,char *serverip,short port)
{
	int packet_length;
	//mqtt_broker_handle_t broker;
	receive_init_ok_flag = -1;
#ifdef DEBUG_PRINT
	printf("clientid:%s\n", clientid);
	printf("server ip:%s\n", serverip);
	printf("server port:%d\n", port);
#endif
	//start init
	//mqtt_init(&broker, "sw-mqtt-pub");
	mqtt_init(&broker_receive, clientid);
	//init_socket(&broker, "10.193.204.162", 1883);
	init_socket_receive(&broker_receive, serverip, port);

	// >>>>> CONNECT
	mqtt_connect(&broker_receive);
	// <<<<< CONNACK
	packet_length = read_packet_receive(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) init_receive on read packet!\n", packet_length);
		return -1;
	}
	receive_init_ok_flag = 1;
	return 0;


}

int fox_mqtt_init_receive(char* inifile)
{
	int packet_length;
	//mqtt_broker_handle_t broker;
	receive_init_ok_flag = -1;
	char fox_clientid[50];
	char fox_mqtt_server_ip[50];
	short fox_mqtt_port=1883;

	if(inifile==NULL)
		inifile = "test.ini";

	//strncpy(broker->clientid, clientid, sizeof(broker->clientid));
	strcpy(fox_clientid, GetIniKeyString("FOXMQTT","clientid",inifile));
	strcpy(fox_mqtt_server_ip, GetIniKeyString("FOXMQTT","mqttserver",inifile));
	fox_mqtt_port = GetIniKeyInt("FOXMQTT","mqttport",inifile);
#ifdef DEBUG_PRINT
	printf("clientid:%s\n", fox_clientid);
	printf("server ip:%s\n", fox_mqtt_server_ip);
	printf("server port:%d\n", fox_mqtt_port);
#endif
	//start init
	//mqtt_init(&broker, "sw-mqtt-pub");
	mqtt_init(&broker_receive, fox_clientid);
	//init_socket(&broker, "10.193.204.162", 1883);
	init_socket_receive(&broker_receive, fox_mqtt_server_ip, fox_mqtt_port);

	// >>>>> CONNECT
	mqtt_connect(&broker_receive);
	// <<<<< CONNACK
	packet_length = read_packet_receive(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) init_receive on read packet!\n", packet_length);
		return -1;
	}
	receive_init_ok_flag = 1;
	return 0;
}

int fox_mqtt_send(const char* topic, const char* msg)
{
	if(send_init_ok_flag)
		return mqtt_publish(&broker_send, topic, msg, 0);
	else
		return -1;
}

//close all mqtt interface
void fox_mqtt_close_send()
{
	if(send_init_ok_flag)
	{
		mqtt_disconnect(&broker_send);
		close_socket(&broker_send);
#ifdef DEBUG_PRINT
		printf("fox_mqtt_close ok.\n");
#endif
	}
}

void fox_mqtt_close_receive()
{
	if(receive_init_ok_flag)
	{
		mqtt_disconnect(&broker_receive);
		close_socket(&broker_receive);
#ifdef DEBUG_PRINT
		printf("fox_mqtt_close ok.\n");
#endif
	}
}

//for receive callback function interface
int fox_mqtt_receive(const char* topic, CallbackRecMsg callbackmsg)
{
	int packet_length;
	uint16_t msg_id;

	if(!receive_init_ok_flag)
		return -1;
#ifdef DEBUG_PRINT
	printf("topic need: %s\n", topic);
#endif

	// Signals after connect MQTT
	signal(SIGALRM, alive);
	alarm(keepalive);
	signal(SIGINT, term);

	//mqtt_subscribe(&broker, "swtest_topic", &msg_id);
	mqtt_subscribe(&broker_receive, topic, &msg_id);

	while(1)
	{
		// <<<<<
		packet_length = read_packet_receive(0);
		if(packet_length == -1)
		{
			fprintf(stderr, "Error(%d) mqtt_receive on read packet!\n", packet_length);
			return -1;
		}
		else if(packet_length > 0)
		{
#ifdef DEBUG_PRINT
			printf("Packet Header: 0x%x...\n", packet_buffer_rec[0]);
#endif
			if(MQTTParseMessageType(packet_buffer_rec) == MQTT_MSG_PUBLISH)
			{
				uint8_t topic[255], msg[1000];
				uint16_t len;
				len = mqtt_parse_pub_topic(packet_buffer_rec, topic);
				topic[len] = '\0'; // for printf
				len = mqtt_parse_publish_msg(packet_buffer_rec, msg);
				msg[len] = '\0'; // for printf
#ifdef DEBUG_PRINT
				printf("foxmqtt:%s:%s\n", topic, msg);
#endif
				//callback to top
				callbackmsg((char*)topic, (char*)msg);
			}
			
		}	

	}
	
}

#endif // __LIBEMQTT_H__
