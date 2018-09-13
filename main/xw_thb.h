#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include "foxmqtt.h"
#include "include/modbus.h"
#include <fcntl.h>
#include <time.h>
#include <pthread.h>


#include <stdint.h>
#include <time.h>

void fox_ht_read_val(int i,char *devid,char *devtype,char *clientid,char *serverip,short port);

////////////////////////////////////////////////////////
//#define DEBUG 1

modbus_t *mb;
char device_node[100];
//char buf_flag3[10] = {0};

char *upload_status;



int InitXwbDevice()
{
   
   mb = modbus_new_rtu(/*"/dev/ttyUSB0"*/"/dev/ttyUSB0",9600,'N',8,1);
    if(mb == NULL)
	{
		printf("Unable to create the libmodbus context!\n");
		return -1;
	}
	modbus_set_debug(mb,0);
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=600000;//1000ms
    modbus_set_response_timeout(mb,&t);

    if(modbus_connect(mb) == -1){
		printf("Connected failed---------\n");
		modbus_free(mb);		
		return -1;
	}

    modbus_set_slave(mb,1);
	return 0;

}

void CloseXwbDevice()
{
	modbus_close(mb); 
    modbus_free(mb);

}

float ReadHumiValue()
{

	float device_Humi;
	uint16_t tab_reg[64]={0};
    int regs=modbus_read_registers(mb, 0x0001, 1, tab_reg); 
    if(regs == -1){
		fprintf(stderr,"%s\n",modbus_strerror(errno));
		return -1;
    }
	device_Humi = (float)tab_reg[0]/100;
    printf("Humi:%.1f%\n",(float)tab_reg[0]/100);  
	return device_Humi;
}



float ReadTempValue()
{

	float device_Temp;
	uint16_t tab_reg[64]={0};

    int regs=modbus_read_registers(mb, 0x0000, 1, tab_reg); 
    if(regs == -1){
		fprintf(stderr,"%s\n",modbus_strerror(errno));
		return -1;
    }
	device_Temp = (float)tab_reg[0]/100;
    printf("temp:%.1f°C\n",(float)tab_reg[0]/100);   
	sleep(1); 
	return device_Temp;

}



void fox_ht_read_val(int nCH_Index,char *devidtmp,char *devtypetmp,char *clientid,char *serverip,short port)
{
	float nTempValue=0;
	float nHumiValue=0;

    InitXwbDevice();

	nTempValue = ReadTempValue();
	nHumiValue = ReadHumiValue();

	char *upload_status="1";
	char datetime[200];
        time_t now;
        struct tm *tm_now;
        time(&now);
        tm_now = localtime(&now);
        strftime(datetime,200,"%Y/%m/%d %H:%M:%S",tm_now);
	char *control_cmd = "0";
        char *device_type = devtypetmp;
        char *devid = devidtmp;
        char buf[800];
        sprintf(buf,"{\"success\":\"%s\",\"cmd\":\"%s\",\"data\":[{\"devtype\":\"%s\",\"devid\":\"%s\",\"tempvalue\":\"%f\",\"humivalue\":\"%f\",\"time\":\"%s\"}]}",upload_status,control_cmd,device_type,devid,nTempValue,nHumiValue,datetime);
#if 1
	int ncode = fox_mqtt_init_send_param(clientid,serverip,port);
        if(!ncode)
                printf("FOXMQTT init ok.\n");
        else
                printf("FOXMQTT init failed!!!\n");

        int ret = fox_mqtt_send("senddata",buf);
        if(ret < 0){
            printf("fox_mqtt_send failed!\n");
        }

        fox_mqtt_close_send();
#endif
		CloseXwbDevice();

}
















