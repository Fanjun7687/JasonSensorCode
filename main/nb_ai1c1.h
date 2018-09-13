#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include "include/foxmqtt.h"
#include "include/modbus.h"
#include <fcntl.h>
#include <time.h>
#include <pthread.h>


#include <stdint.h>
#include <ctype.h>

short fox_read_type ( char *devicenode ); //check device type with read regsiter per devicenode , 0:Voltage 1:Current

uint16_t fox_cur_read_val ( char *devicenode,short range );

////////////////////////////////////////////////////////
//#define DEBUG 1

modbus_t *mbI;
char device_node[100];
char buf_flag[10] = {0};

char *upload_status;



int InitDeviceAI ( char *devicenode )
{

    mbI = modbus_new_rtu ( devicenode,9600,'N',8,1 );
    if ( mbI == NULL ) {
        printf ( "Unable to create the libmodbus context for current Init!\n" );
        return -1;
    }


    modbus_set_debug ( mbI,0 );
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=600000;//1000ms   600000
    modbus_set_response_timeout ( mbI,&t );

    if ( modbus_connect ( mbI ) == -1 ) {
        printf ( "Connected current modbus failed---------\n" );
        modbus_free ( mbI );
        return -1;
    }

    modbus_set_slave ( mbI,1 );
    //printf("========================11111111111111111111111\n");
    return 0;

}

void CloseDevice()
{
    modbus_close ( mbI );
    modbus_free ( mbI );

}

uint16_t ReadCurValue ( short range )
{
    uint16_t device_I0;
    uint16_t tab_reg[64]= {0};
    int regs=modbus_read_registers ( mbI, 0x000A, 1, tab_reg );
    if ( regs == -1 ) {
        fprintf ( stderr,"%s\n",modbus_strerror ( errno ) );
        return -1;
    }
    printf ( "Current tab_reg:%d\n",tab_reg[0] );
    //device_I0 = tab_reg[0] * 15000 / 10000;
    device_I0 = tab_reg[0]*range/10000;
    printf ( "Current :%dA\n",device_I0 );
    return  device_I0;

}
uint16_t fox_cur_read_val ( char *devicenode,short range )
{
    uint16_t nValue=0;
    InitDeviceAI ( devicenode );
    nValue = ReadCurValue ( range );
    CloseDevice();
    return nValue;
}

float ReadCurValuef ( short range )
{
    float fdevice_I0;
    uint16_t tab_reg[64]= {0};
    int regs=modbus_read_registers ( mbI, 0x000A, 1, tab_reg );
    if ( regs == -1 ) {
        fprintf ( stderr,"%s\n",modbus_strerror ( errno ) );
        return -1;
    }
    printf ( "Current tab_reg:%d\n",tab_reg[0] );
    //device_I0 = tab_reg[0] * 15000 / 10000;
    fdevice_I0 = tab_reg[0]*range/10000.0;
    printf ( "Current :%.2fA\n",fdevice_I0 );
    return  fdevice_I0;

}
float fox_cur_read_valf ( char *devicenode,short range )
{
    float nValue=0;
    //InitDeviceAI(devicenode);
    nValue = ReadCurValuef ( range );
    //CloseDevice();
    return nValue;
}


short fox_read_type ( char *devicenode )
{
    InitDeviceAI ( devicenode );
    uint16_t tab_reg[64]= {0};
    int regs=modbus_read_registers ( mbI, 0x0000, 1, tab_reg ); //read 2 bit from 0x0000H
    if ( regs == -1 ) {
        fprintf ( stderr,"%s\n",modbus_strerror ( errno ) );
        printf ( "===================2222222222222222\n" );
        return -1;
    }

    char cflg=toascii ( ( int ) tab_reg[0] );
    printf ( "Current tab_reg:%C\n",cflg );	//get Voltage or Icurrent  flag
    CloseDevice();
    if ( cflg=='V' ) {
        printf ( "This is Voltage Sensor\n" );
        return 0;
    }

    if ( cflg=='I' ) {
        printf ( "This is Current Sensor\n" );
        return 1;
    }
    return -1;
}












