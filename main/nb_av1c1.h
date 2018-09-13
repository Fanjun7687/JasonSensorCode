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

uint16_t fox_vol_read_val(char *devicenode,short range);

////////////////////////////////////////////////////////
//#define DEBUG 1

modbus_t *mbV;
char device_node[100];

char *upload_status;



int InitDeviceAV(char *devicenode)
{
   
   mbV = modbus_new_rtu(devicenode,9600,'N',8,1);
    if(mbV == NULL)
	{
		printf("Unable to create the libmodbus context for Voltage Modbus!\n");
		return -1;
	}


	modbus_set_debug(mbV,0);
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=600000;//1000ms  600000

    modbus_set_response_timeout(mbV,&t);
    if(modbus_connect(mbV) == -1){
		printf("Connected Voltage Modbus failed---------\n");
		modbus_free(mbV);		
		return -1;
	}

    modbus_set_slave(mbV,1);
	return 0;
}

void CloseDeviceAV()
{
	modbus_close(mbV); 
    modbus_free(mbV);

}

uint16_t ReadVolValue(short range)
{
	uint16_t device_U0;
	uint16_t tab_reg[64]={0};
    int regs=modbus_read_registers(mbV, 0x000A, 1, tab_reg); 
    if(regs == -1){
		fprintf(stderr,"%s\n",modbus_strerror(errno));
		return -1;
    }
	printf("Voltage tab_reg:%d\n",tab_reg[0]);
	//device_U0 = tab_reg[0] * 250 / 10000;
	device_U0 = tab_reg[0]*range / 10000;
    printf("Voltage :%dV\n",device_U0);  
	return  device_U0;
}




uint16_t fox_vol_read_val(char *devicenode,short range)
{
	uint16_t nValue=0;
    InitDeviceAV(devicenode);
	nValue = ReadVolValue(range);
    CloseDeviceAV();
	return nValue;
}

float ReadVolValuef(short range)
{
	float fdevice_U0;
	uint16_t tab_reg[64]={0};
    int regs=modbus_read_registers(mbV, 0x000A, 1, tab_reg); 
    if(regs == -1){
		fprintf(stderr,"%s\n",modbus_strerror(errno));
		return -1;
    }
	printf("Voltage tab_reg:%d\n",tab_reg[0]);
	//device_U0 = tab_reg[0] * 250 / 10000;
	fdevice_U0 = tab_reg[0]*range / 10000.0;
    printf("Voltage :%.2fV\n",fdevice_U0);  
	return  fdevice_U0;
}


float fox_vol_read_valf(char *devicenode,short range)
{
	float nValue=0.00f;
    //InitDeviceAV(devicenode);
	nValue = ReadVolValuef(range);
    //CloseDeviceAV();
	return nValue;
}















