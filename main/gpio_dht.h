#include <wiringPi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include "foxmqtt.h"

#define MAX_TIME 85
#define GpioPort 7

void fox_dht11_read_val(short gpio);
////////////////////////////////////////////////////  
char *upload_status;
 
int dht11_val[5] = { 0, 0, 0, 0, 0 };
float humi_value=0;
float temp_value=0;
void fox_InitTempHumi_pin(short gpio)
{
	pinMode(gpio, OUTPUT);
    digitalWrite(gpio, LOW);
    delay(18);  //18
    digitalWrite(gpio, HIGH);
    //delayMicroseconds(40);
    pinMode(gpio, INPUT);
}

int fox_read_tmp_humi(short gpio)
{
    uint8_t lststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    
    for (i = 0; i < 5; i++)
    	dht11_val[i] = 0;
   
    fox_InitTempHumi_pin(gpio);
    for (i = 0; i < MAX_TIME; i++){
	counter = 0;
        while (digitalRead(gpio) == lststate){
	    counter++;
	    delayMicroseconds(1);
	    if (counter == 255)
                break;
        }
        lststate = digitalRead(gpio);
        if (counter == 255)
            break;
        if ((i >= 4) && (i % 2 == 0)){
            dht11_val[j / 8] <<= 1;
            if (counter > 16) 
	        dht11_val[j / 8] |= 1; 
            j++;
        }
    }

    if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF))){
	humi_value = dht11_val[0] + dht11_val[1]/10.0;
	temp_value = dht11_val[2] + dht11_val[3]/10.0;
	printf("humi_value:%.2f,temp_value:%.2f\n",humi_value,temp_value);
	return 1;
    }else
	return 0;

}

void upload_server()
{
    char *inifile = "dht11.ini";
    char strDeviceId[50];
    char strDeviceType[50];
    strcpy(strDeviceId, GetIniKeyString("FOXMQTT","deviceid",inifile));
    strcpy(strDeviceType, GetIniKeyString("FOXMQTT","devtype",inifile));
    upload_status = "1";
    char datetime[200];
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    strftime(datetime,200,"%Y/%m/%d %H:%M:%S",tm_now);
    //printf("dht11--time:%s\n",datetime);

    char *control_cmd = "0";
    char *device_type = strDeviceType;
    char *devid = strDeviceId;
    char buf[800];
    sprintf(buf,"{\"success\":\"%s\",\"cmd\":\"%s\",\"data\":[{\"devtype\":\"%s\",\"devid\":\"%s\",\"humi\":\"%f\",\"temp\":\"%f\",\"time\":\"%s\"}]}",upload_status,control_cmd,device_type,devid,humi_value,temp_value,datetime);
    //printf("buf is %s\n",buf);

    int ncode2 = fox_mqtt_init_send("dht11.ini");
    if(!ncode2)
        printf("FOXMQTT init ok.\n");
    else
        printf("FOXMQTT init failed!!!\n");
    int ret = fox_mqtt_send("senddata",buf);
    if(ret < 0){
        printf("fox_mqtt_send failed!\n");
    }
    fox_mqtt_close_send();

}

void fox_dht11_read_val(short gpio)
{
	
    int attempts = 1;
    while(attempts){
	int success = fox_read_tmp_humi(gpio);
	
	if(success){
   	    //upload_server();
	    break;
	}
	attempts--;
	//delay(2000);
    }
}    

