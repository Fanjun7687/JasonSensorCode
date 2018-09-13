/*
======================Release Notes========================
//-------2018.09.04
1. Add foxcurl.h for curl post data to server
//-------2018.09.06
1. change alarm(3) to alarm(1) to fixed in Discharge Demo, 3 only for debug
//-------2018.09.09
1. add Curl reply info to split
//-------2018.09.11
1. add runCmd_OpenOutivalue() in foxpipe.h to fixed GPIO 11 & 13 can't read with wiringpi lib In hall sensor read!!
//-------2018.09.12
1. test http post json data to Server ok , modfiy foxcurl.h to add to setopt "CURLOPT_HTTPHEADER " with json .

*/

#include <string.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pcf8591.h>
#include <pthread.h>
#include "exception.h"

#include "include/foxmqtt.h"
#include "include/foxpipe.h"
#include "nb_ai1c1.h"
#include "nb_av1c1.h"
//#include "xw_thb.h"
#include "linux_macaddr.h"
#include "bpi_gpio.h"
#include "bpi_uart.h"
#include "gpio_dht.h"    //for temperature and humidity
#include <curl/curl.h>
#include "foxcurl.h"    //for curl post data to Server


#define DEBUG

//for SKU project need to run main
int sku_DisCharge=0;
int sku_WireCut=0;
int sku_StamPing=0;
int sku_DemoRoom=0;
int sku_DebugWork=0;  //only for Jason Debug
int sku_HBWorking=0;   //for HangZhou HanBo Factory

//for sensor type debug
int iSensor_obstacle=0;
int iSensor_temphumi=0;
int iSensor_voice=0;
int iSensor_traction=0;
int iSensor_buzzer=0;
int iSensor_humanir=0;

//for config.ini parameter
char voltage_node[20];
char current_node[20];
short voltage_range=250;
short current_range=15000;
short time_val=200;

short GPIO_PORT=7;
int iGPIOValue = 0;

char uart_node[20];
uint32_t uart_rate=115200;

//for raw data test spec defined in config.ini
struct VCSpec {
    float fmin_Voltage;
    float fmax_Voltage;
    float fmin_Current;
    float fmax_Current;
} sVCSpec;

struct SewingMachine {
    char HB_Sewing_ID[20];
    int iFreq_HS_SendData;      //Hall sensor update data to server cycle time : default 60s
    int iFreq_PS_SendData;
    int iCycleNumb;
    int iCylceNumbSend;
    bool bFlag;     //if bFlag true, iCycleNumb + 1
} mSewInfo;

//for mqtt parameter in config.ini
char fox_mqtt_stationid[20];
char fox_mqtt_macaddrid[20];
char fox_mqtt_server_ip[20];
short fox_mqtt_port=1883;
char fox_mqtt_username[13];
char fox_mqtt_password[13];
bool bGetMAC=false;

char *mqtttopnic="senddata";
char *upload_type="1";      //1:Voltage & Current 2:temperature&humidity
char sendMsg[8192]= {'\0'};
char datetime[50]= {'\0'};

uint16_t v_Value,c_Value;
//for voltage and current raw data
float v_Valuef,c_Valuef;
float v_ArrayValuef[30]= {0.00f};
float c_ArrayValuef[30]= {0.00f};

//for temperature and humidity raw data
float temp_Valuef,humi_Valuef;
float temp_ArrayValuef[30]= {0.00f};
float humi_ArrayValuef[30]= {0.00f};

char datetimeArray[30][50]= {'\0'};
int iCycle = 0;

//for HTTP post url in config.inifile
char fox_httpurl_address[128]={'\0'};

void *SendData_thread();


void readValue ( int tmp )
{
    printf ( "%0x\n",tmp );

}

void getconfigsetting ( char *inifile )
{
    sku_DebugWork =GetIniKeyInt ( "SKUSELECT","DebugWork",inifile );
    sku_DisCharge =GetIniKeyInt ( "SKUSELECT","DisCharge",inifile );
    sku_WireCut   =GetIniKeyInt ( "SKUSELECT","WireCut",inifile );
    sku_StamPing  =GetIniKeyInt ( "SKUSELECT","StamPing",inifile );
    sku_DemoRoom  =GetIniKeyInt ( "SKUSELECT","DemoRoom",inifile );
    sku_HBWorking =GetIniKeyInt ( "SKUSELECT","HBWorking",inifile );

    iSensor_obstacle  =GetIniKeyInt ( "SKUSELECT","Sensor_obstacle",inifile );
    iSensor_temphumi  =GetIniKeyInt ( "SKUSELECT","Sensor_temphumi",inifile );
    iSensor_voice     =GetIniKeyInt ( "SKUSELECT","Sensor_voice",inifile );
    iSensor_traction  =GetIniKeyInt ( "SKUSELECT","Sensor_traction",inifile );
    iSensor_buzzer    =GetIniKeyInt ( "SKUSELECT","Sensor_buzzer",inifile );
    iSensor_humanir   =GetIniKeyInt ( "SKUSELECT","Sensor_humanir",inifile );

    strcpy ( voltage_node,GetIniKeyString ( "DEVICE","voltageNode",inifile ) );
    strcpy ( current_node,GetIniKeyString ( "DEVICE","currentNode",inifile ) );
    voltage_range = GetIniKeyInt ( "DEVICE","voltageRange",inifile );
    current_range = GetIniKeyInt ( "DEVICE","currentRange",inifile );
    time_val = GetIniKeyInt ( "DEVICE","time_val",inifile );

    sVCSpec.fmax_Voltage = GetIniKeyFloat ( "SPEC","VoltageMax",inifile );
    sVCSpec.fmin_Voltage = GetIniKeyFloat ( "SPEC","VoltageMin",inifile );
    sVCSpec.fmax_Current = GetIniKeyFloat ( "SPEC","CurrentMax",inifile );
    sVCSpec.fmax_Current = GetIniKeyFloat ( "SPEC","CurrentMin",inifile );


    strcpy ( fox_mqtt_stationid,GetIniKeyString ( "FOXMQTT","stationid",inifile ) );
    strcpy ( fox_mqtt_server_ip,GetIniKeyString ( "FOXMQTT","mqttserver",inifile ) );
    fox_mqtt_port = GetIniKeyInt ( "FOXMQTT","mqttport",inifile );
    strcpy ( fox_mqtt_username,GetIniKeyString ( "FOXMQTT","mqttusr",inifile ) );
    strcpy ( fox_mqtt_password,GetIniKeyString ( "FOXMQTT","mqttpwd",inifile ) );

    get_mac ( fox_mqtt_macaddrid,sizeof ( fox_mqtt_macaddrid ) );
    if ( bGetMAC ) {
        return;
    }
    //bGetMAC=(PutIniKeyString("FOXMQTT","macaddrid",fox_mqtt_macaddrid,inifile)!=-1);

    strcpy ( uart_node,GetIniKeyString ( "DEVICE","uartNode",inifile ) );
    uart_rate=GetIniKeyInt ( "DEVICE","uartRate",inifile );
    GPIO_PORT=GetIniKeyInt ( "DEVICE","gpioPort",inifile );
    printf ( "%d\n",GPIO_PORT );

    strcpy ( mSewInfo.HB_Sewing_ID,GetIniKeyString ( "HTTPURL","SewingID",inifile ) );
    strcpy ( fox_httpurl_address,GetIniKeyString ( "HTTPURL","ServerUrl",inifile ) );
   

}
//splite below 3 step to run MQTT Send Data**********************
int Jason_mqtt_connect ( char *clientid,char *serverip,int port,char *usr,char *pwd ) //success: 0    NG: -1
{
    int iRet = fox_mqtt_init_send_param_auth2 ( clientid,serverip,port,usr,pwd );
    if ( !iRet )	{
        printf ( "FOXMQTT init OK.\n" );
    } else {
        printf ( "FOXMQTT init failed!\n" );
    }
    return iRet;
}
int Jason_mqtt_senddata ( char *mqtttopnic,char *sendmsg )              //success: 0
{
    int iRet = fox_mqtt_send ( mqtttopnic,sendmsg );
    if ( iRet<0 ) {
        printf ( "FOXMQTT Send Data Failed.\n" );
        return -1;
    }
    return 0;
}
void Jason_mqtt_disconnect()
{
    fox_mqtt_close_send();
}
//splite above 3 step to run MQTT Send Data**********************



void gettime ( char *curTime )
{
    //char datetime[200];
    time_t now;
    struct tm *tm_now;
    time ( &now );
    tm_now = localtime ( &now );
    //strftime(curTime,50,"%x %X %n%Y-%m-%d %H:%M:%S %nzone: %Z\n",tm_now);
    strftime ( curTime,50,"%Y/%m/%d %H:%M:%S",tm_now );
    //printf("now datetime : %s\n",curTime);
    //strcpy(datetime,curTime);
}
void handler()
{
    printf ( "Here Add MQTT Send Data Array ~~~~\n" );
    //printf("Votage Capture: %d\n",v_Value);
    //printf("Current Capture: %d\n",c_Value);

    memset ( sendMsg,'\0',sizeof ( sendMsg ) );
    //sprintf(sendMsg,"{\"success\":\"%s\",\"mac\":\"%s\",\"data\":[{\"Voltage_value\":\"%d\",\"Current_value\":\"%d\",\"time\":\"%s\"}]}",upload_type,fox_mqtt_macaddrid,v_Value,c_Value,datetime);

    sprintf ( sendMsg,"{\"success\":\"%s\",\"mac\":\"%s\",\"data\":[{\"Voltage_value\":\"%.2f\",\"Current_value\":\"%.2f\",\"time\":\"%s\"}",upload_type,fox_mqtt_macaddrid,v_ArrayValuef[0],c_ArrayValuef[0],datetimeArray[0] );
    char DataALL[8192]= {'\0'};
    char DataTmp[200]= {'\0'};
    printf ( "Get Data Array Number Is  %d~~~~\n",iCycle );

    for ( int i=1; i<iCycle; i++ ) { //i<10 OK
        memset ( DataTmp,'\0',sizeof ( DataTmp ) );
        sprintf ( DataTmp,",{\"Voltage_value\":\"%.2f\",\"Current_value\":\"%.2f\",\"time\":\"%s\"}",v_ArrayValuef[i],c_ArrayValuef[i],datetimeArray[i] );
        strncat ( DataALL,DataTmp,sizeof ( DataTmp ) );
    }
    strcat ( sendMsg,DataALL );
    strcat ( sendMsg,"]}" );

    printf ( "%s-----%d\n",sendMsg, ( int ) strlen ( sendMsg ) );
    //pause();


    memset ( v_ArrayValuef,0.00f,sizeof ( v_ArrayValuef ) );
    memset ( c_ArrayValuef,0.00f,sizeof ( c_ArrayValuef ) );
    memset ( datetimeArray,'\0',sizeof ( datetimeArray ) );
    //dataSend_auth(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password,sendMsg);



//		Jason_mqtt_senddata(mqtttopnic,sendMsg);

    if ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        delay ( 10 );
        Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password );
    }
    Jason_mqtt_senddata ( mqtttopnic,sendMsg );
    Jason_mqtt_disconnect();
//		Jason_mqtt_connect(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password);


    /*
    		if(Jason_mqtt_connect(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password)!=0)
    		{
    			delay(10);
    			Jason_mqtt_connect(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password);
    		}
    		Jason_mqtt_senddata(mqtttopnic,sendMsg);
    		Jason_mqtt_disconnect();
    */

    /*
    	if(Jason_mqtt_senddata(mqtttopnic,sendMsg)!=0)
    	{
    		delay(10);
    		Jason_mqtt_disconnect();delay(2);
    		while(Jason_mqtt_connect(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password)!=0)
    		{
    			delay(2);
      			if(Jason_mqtt_senddata(mqtttopnic,sendMsg)==0) break;
    			else            Jason_mqtt_disconnect();delay(2);

    		}
    	}
    */
    printf ( "************************************************\n" );

    signal ( SIGALRM,handler );
    alarm ( 1 );  //debug : 3 ;   Discharge: 1
    iCycle = 0;
}
void handler_temphumi()
{
    printf ( "Here Add MQTT Send Data Array ~~~~\n" );
    memset ( sendMsg,'\0',sizeof ( sendMsg ) );
    sprintf ( sendMsg,"{\"success\":\"%s\",\"mac\":\"%s\",\"data\":[{\"Temperature_value\":\"%.2f\",\"Humidity_value\":\"%.2f\",\"time\":\"%s\"}",upload_type,fox_mqtt_macaddrid,temp_ArrayValuef[0],humi_ArrayValuef[0],datetimeArray[0] );
    char DataALL[4096]= {'\0'};
    char DataTmp[200]= {'\0'};
    printf ( "Get Data Array Number Is  %d~~~~\n",iCycle );

    for ( int i=1; i<iCycle; i++ ) { //i<10 OK
        memset ( DataTmp,'\0',sizeof ( DataTmp ) );
        sprintf ( DataTmp,",{\"Temperature_value\":\"%.2f\",\"Humidity_value\":\"%.2f\",\"time\":\"%s\"}",temp_ArrayValuef[i],humi_ArrayValuef[i],datetimeArray[i] );
        strncat ( DataALL,DataTmp,sizeof ( DataTmp ) );
    }
    strcat ( sendMsg,DataALL );
    strcat ( sendMsg,"]}" );

    printf ( "%s-----%d\n",sendMsg, ( int ) strlen ( sendMsg ) );
    //pause();


    memset ( temp_ArrayValuef,0.00f,sizeof ( temp_ArrayValuef ) );
    memset ( humi_ArrayValuef,0.00f,sizeof ( humi_ArrayValuef ) );
    memset ( datetimeArray,'\0',sizeof ( datetimeArray ) );
    //dataSend_auth(fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password,sendMsg);
    while ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        printf ( "Please check network connection~\n" );
        sleep ( 1 );
    }

    while ( Jason_mqtt_senddata ( mqtttopnic,sendMsg ) !=0 ) {
        Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password );
        if ( Jason_mqtt_senddata ( mqtttopnic,sendMsg ) ==0 ) {
            break;
        }
    }
    Jason_mqtt_disconnect();
    printf ( "************************************************\n" );

    //signal(SIGALRM,handler_temphumi);
    //alarm(1);
    iCycle = 0;
}

void handler_hallsensor()
{
    printf ( "***************************************************\n" );
    printf ( "Send HTTP Server Sewing Machine Cycle Number : %d \n",mSewInfo.iCycleNumb );
    printf ( "***************************************************\n" );

    signal ( SIGALRM,handler_hallsensor );
    alarm ( mSewInfo.iFreq_HS_SendData );
    mSewInfo.iCylceNumbSend =  mSewInfo.iCycleNumb;
    mSewInfo.iCycleNumb = 0;

    pthread_t tid_sd;
    pthread_create ( &tid_sd,NULL,SendData_thread,NULL );
    pthread_join ( tid_sd,NULL );
}

int chkDeviceType ( bool *bnodesetok )
{
    if ( fox_read_type ( voltage_node ) ==0&& ( fox_read_type ( current_node ) ==1 ) ) {
        *bnodesetok = true;
        return 1;
    } else if ( fox_read_type ( voltage_node ) ==1&& ( fox_read_type ( current_node ) ==0 ) ) {
        *bnodesetok = false;
        return 2;    //need change USB0 and USB1 for two sensor
    } else {
        printf ( "Please Check ModBus2USB Device Setup Firstly~~\n" );
    }
    return 0;
}

void getData_HallSensor()     //for Hall sensor report
{
    int nvalue=0;
    upload_type="HS";
    signal ( SIGALRM,handler_hallsensor );
    alarm ( mSewInfo.iFreq_HS_SendData );

    switch ( GPIO_PORT ) {
    case 7: {
        wiringPiSetup();
        pinMode ( GPIO_PORT,INPUT );
        while ( 1 ) {
            nvalue=digitalRead ( GPIO_PORT );
            if ( nvalue==1 ) {
                //printf ( "Hall Sensor Away!\n");
                mSewInfo.bFlag = false;
            } else {
                //printf ( "Hall Sensor Near!===============================================>\n");
                if ( mSewInfo.bFlag==false ) {
                    mSewInfo.bFlag=true;
                    mSewInfo.iCycleNumb++;
                    printf ( "Sewing Machine Run Cycle : %d \n",mSewInfo.iCycleNumb );
                }
            }
            delay ( 50 );
        }
    }
    break;
    case 11: {
        while ( 1 ) {
            //nvalue=runCmd_PipeOutivalue("cat /sys/class/gpio/gpio231/value");
            nvalue=runCmd_OpenOutivalue ( "/sys/class/gpio/gpio231/value" );
            if ( nvalue==1 ) {
                //printf ( "Hall Sensor Away!\n");
                mSewInfo.bFlag = false;
            } else {
                //printf ( "Hall Sensor Near!===============================================>\n");
                if ( mSewInfo.bFlag==false ) {
                    mSewInfo.bFlag=true;
                    mSewInfo.iCycleNumb++;
                    printf ( "Sewing Machine Run Cycle : %d \n",mSewInfo.iCycleNumb );
                }
            }
            delay ( 50 );
        }
    }
    break;
    case 13: {
        while ( 1 ) {
            //nvalue=runCmd_PipeOutivalue("cat /sys/class/gpio/gpio234/value");
            nvalue=runCmd_OpenOutivalue ( "/sys/class/gpio/gpio234/value" );
            if ( nvalue==1 ) {
                //printf ( "Hall Sensor Away!\n");
                mSewInfo.bFlag = false;
            } else {
                //printf ( "Hall Sensor Near!===============================================>\n");
                if ( mSewInfo.bFlag==false ) {
                    mSewInfo.bFlag=true;
                    mSewInfo.iCycleNumb++;
                    printf ( "Sewing Machine Run Cycle : %d \n",mSewInfo.iCycleNumb );
                }
            }
            delay ( 50 );
        }
    }
    break;
    }

}

void *SendData_thread()
{
    memset ( datetime,'\0',sizeof ( datetime ) );
    gettime ( datetime );
    printf ( "************************%s\n",datetime );

    memset ( sendMsg,'\0',sizeof ( sendMsg ) );
    //sprintf ( sendMsg,"{\"success\":\"%s\",\"DeviceID\":\"%s\",\"data\":\"%d\",\"time\":\"%s\"}",upload_type,mSewInfo.HB_Sewing_ID,mSewInfo.iCylceNumbSend,datetime );
    sprintf ( sendMsg,"{\"device_id\":\"%s\",\"device_data\":\"%d\",\"device_datetime\":\"%s\"}",mSewInfo.HB_Sewing_ID,mSewInfo.iCylceNumbSend,datetime );
    printf ( "%s-----%d\n",sendMsg, ( int ) strlen ( sendMsg ) );
    printf("%s\n",fox_httpurl_address);
    int iRet=curl_SendData (fox_httpurl_address,sendMsg );
    //int iRet=curl_SendData ( "http://10.193.205.82:3000",sendMsg );
    //int iRet=curl_SendData ( "http://192.168.1.107:3004",sendMsg );  //BSSAPI/V2/LoginBSS
    
    char *replyinfo= ( char * ) malloc ( 1024 );
    curl_GetReplyInfo ( replyinfo );
    printf ( "reply info : %s \n",replyinfo );
    //printf ( "reply Get Key Info : %s\n",getContentBySplit ( replyinfo,"BssID\":",",\"BssName" ) );

    printf ( "==========> iRet: %d\n",iRet );
    free ( replyinfo );
}

void *HBWorking_thread ( void *arg )
{
    char *type= ( char * ) arg;
    if ( strstr ( type,"IA" ) ) {
        InitDeviceAI ( current_node );
        while ( 1 ) {
            c_Valuef = fox_cur_read_valf ( current_node,current_range );
//			memset(datetime,'\0',sizeof(datetime));
//			gettime(datetime);printf("************************%s\n",datetime);
//			strcpy(datetimeArray[iCycle],datetime);
//			v_ArrayValuef[iCycle] = v_Valuef;
//			c_ArrayValuef[iCycle] = c_Valuef;
//			printf("=================================================%d\n",iCycle);
//			iCycle++;
            //sleep(time_val/1000.0);
            printf ( "=============> IA Get Data\n" );
            usleep ( time_val );
        }

    } else if ( strstr ( type,"PS" ) ) {
        while ( 1 ) {
            printf ( "=============> PS Get Data\n" );
            usleep ( time_val );
            pthread_t tid_sd;
            pthread_create ( &tid_sd,NULL,SendData_thread,NULL );
            pthread_join ( tid_sd,NULL );
            delay ( 1000 );
            //curl_SendData("http://192.168.1.101:3004/BSSAPI/V2/LoginBSS","wd=hava&hehe=123456");
        }
    } else if ( strstr ( type,"HS" ) ) {
        getData_HallSensor();
    }
}

void getData_HBWorking()
{
//	InitDeviceAI(current_node);
//	wiringPiSetup();

    upload_type="1";
    mSewInfo.iFreq_HS_SendData = 60;    //60s
    mSewInfo.iFreq_PS_SendData = 60;    //60s
    pthread_t tid_IA,tid_PS,tid_HS;    //IA: Current Sensor   PS: Photoeclectricity Sensor   HS: HALL Sensor
    int ret;
    /*
        ret = pthread_create(&tid_IA,NULL,HBWorking_thread,"IA");
        if(ret!=0)
        {
        	fprintf(stderr,"Create %s thread failed\n","IA");
        	return;
        }

        ret = pthread_create(&tid_PS,NULL,HBWorking_thread,"PS" );
        if ( ret!=0 ) {
            fprintf ( stderr,"Create %s thread failed\n","PS" );
            return;
        }
    */
    ret = pthread_create ( &tid_HS,NULL,HBWorking_thread,"HS" );
    if ( ret!=0 ) {
        fprintf ( stderr,"Create %s thread failed\n","HS" );
        return;
    }

    //pthread_detach(tid_IA);  //only run once, cancel release source
    //pthread_detach(tid_PS);

    //pthread_join(tid_IA,NULL);  //must add pthread_join!! could run loop ~~
    //pthread_join(tid_PS,NULL );
    pthread_join ( tid_HS,NULL );
    /*
    */
    return;

}

void getData_DisCharge()
{
    bool bNodeSetOK=true;     //config.ini set voltage & current node path correctly
    while ( 1 ) {
        if ( chkDeviceType ( &bNodeSetOK ) !=0 ) {
            break;
        }
    }


    while ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        printf ( "Please check network connection~\n" );
        sleep ( 1 );
    }

    InitDeviceAV ( voltage_node );
    InitDeviceAI ( current_node );

    upload_type="1";
    signal ( SIGALRM,handler );
    alarm ( 1 );

    while ( 1 ) {


        if ( bNodeSetOK ) { //node path set OK in config.ini
            v_Valuef = fox_vol_read_valf ( voltage_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( current_node,current_range );
        } else { //need change node path
            v_Valuef = fox_vol_read_valf ( current_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( voltage_node,current_range );
        }
        memset ( datetime,'\0',sizeof ( datetime ) );
        gettime ( datetime );
        printf ( "************************%s\n",datetime );
        strcpy ( datetimeArray[iCycle],datetime );
        v_ArrayValuef[iCycle] = v_Valuef;
        c_ArrayValuef[iCycle] = c_Valuef;
        printf ( "=================================================%d\n",iCycle );
        iCycle++;
        //sleep(time_val/1000.0);
        usleep ( time_val );
    }
    Jason_mqtt_disconnect();
}

void getErrData_DisCharge()   //when capture error data over spec ,send to MQTT Server at once!!
{
    bool bNodeSetOK=true;     //config.ini set voltage & current node path correctly
    while ( 1 ) {
        if ( chkDeviceType ( &bNodeSetOK ) !=0 ) {
            break;
        }
    }


    while ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        printf ( "Please check network connection~\n" );
        sleep ( 1 );
    }

    InitDeviceAV ( voltage_node );
    InitDeviceAI ( current_node );

    upload_type="1";

    while ( 1 ) {
        if ( bNodeSetOK ) { //node path set OK in config.ini
            v_Valuef = fox_vol_read_valf ( voltage_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( current_node,current_range );
        } else { //need change node path
            v_Valuef = fox_vol_read_valf ( current_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( voltage_node,current_range );
        }
        if ( v_Valuef<sVCSpec.fmin_Voltage||v_Valuef>sVCSpec.fmax_Voltage||c_Valuef<sVCSpec.fmin_Current||c_Valuef>sVCSpec.fmax_Current ) {
            memset ( sendMsg,'\0',sizeof ( sendMsg ) );
            memset ( datetime,'\0',sizeof ( datetime ) );
            gettime ( datetime );
            sprintf ( sendMsg,"{\"success\":\"%s\",\"mac\":\"%s\",\"data\":[{\"Voltage_value\":\"%.2f\",\"Current_value\":\"%.2f\",\"time\":\"%s\"}",upload_type,fox_mqtt_macaddrid,v_Valuef,c_Valuef,datetime );
            printf ( "%s-----%d\n",sendMsg, ( int ) strlen ( sendMsg ) );
            while ( Jason_mqtt_senddata ( mqtttopnic,sendMsg ) !=0 ) {
                Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password );
                if ( Jason_mqtt_senddata ( mqtttopnic,sendMsg ) ==0 ) {
                    break;
                }
            }
        }
        printf ( "=================================================%d\n",iCycle );
        iCycle++;
        //sleep(time_val/1000.0);
        usleep ( time_val );
    }
    Jason_mqtt_disconnect();
}


void getData_WireCut()
{
    bool bNodeSetOK=true;     //config.ini set voltage & current node path correctly
    current_range = 100;    //it's different with DisCharge Line
    while ( 1 ) {
        if ( chkDeviceType ( &bNodeSetOK ) !=0 ) {
            break;
        }
    }


    while ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        printf ( "Please check network connection~\n" );
        sleep ( 1 );
    }

    InitDeviceAV ( voltage_node );
    InitDeviceAI ( current_node );

    upload_type="1";
    signal ( SIGALRM,handler );
    alarm ( 1 );

    while ( 1 ) {
        if ( bNodeSetOK ) { //node path set OK in config.ini
            v_Valuef = fox_vol_read_valf ( voltage_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( current_node,current_range );
        } else { //need change node path
            v_Valuef = fox_vol_read_valf ( current_node,voltage_range );
            c_Valuef = fox_cur_read_valf ( voltage_node,current_range );
        }
        memset ( datetime,'\0',sizeof ( datetime ) );
        gettime ( datetime );
        printf ( "************************%s\n",datetime );
        strcpy ( datetimeArray[iCycle],datetime );
        v_ArrayValuef[iCycle] = v_Valuef;
        c_ArrayValuef[iCycle] = c_Valuef;
        printf ( "=================================================%d\n",iCycle );
        iCycle++;
        //sleep(time_val/1000.0);
        usleep ( time_val );
    }
    Jason_mqtt_disconnect();
}
void getData_StamPing()
{
//for GPIO  NPN msg send
#if 0
    GPIO_Init ( GPIO_PORT,INPUT );  //if need GPIO Port Read value ,need enable
    while ( 1 ) {
        iGPIOValue = GPIO_GetValue ( GPIO_PORT );
        printf ( "%d ======== !!\n",iGPIOValue );
        sleep ( 1 );
    }
#endif

//for UART communictation
#if 0
    UART_Init ( uart_node,uart_rate );
    UART_SendMsg ( "1" );
    UART_Close();
    return 0;
#endif
    UART_Init ( uart_node,uart_rate );
    GPIO_Init ( GPIO_PORT,INPUT );
    char strGPIOV[2]= {'\0'};
    while ( 1 ) {
        iGPIOValue = GPIO_GetValue ( GPIO_PORT );
        printf ( "%d ======== !!\n",iGPIOValue );
        sprintf ( strGPIOV,"%d",iGPIOValue );
        UART_SendMsg ( strGPIOV );
    }
    UART_Close();

}
void *dht_thread()
{
    int szet;
    while ( 1 ) {
        //fox_dht11_read_val(GPIO_PORT);
        szet = fox_read_tmp_humi ( GPIO_PORT );
        if ( szet==1 ) {
            temp_Valuef = temp_value;
            humi_Valuef = humi_value;
            memset ( datetime,'\0',sizeof ( datetime ) );
            gettime ( datetime );
            printf ( "************************%s\n",datetime );
            strcpy ( datetimeArray[iCycle],datetime );
            temp_ArrayValuef[iCycle] = temp_Valuef;
            humi_ArrayValuef[iCycle] = humi_Valuef;
            printf ( "=================================================%d\n",iCycle );
            iCycle++;
            if ( iCycle == 20 ) {
                handler_temphumi();
            }
        }
    }
    Jason_mqtt_disconnect();

}
void getData_DemoRoom()    //for A2 DemoRoom to get temperature and humidity raw Data to MQTT Server
{
    wiringPiSetup();
    upload_type="2";

#if 1
    pthread_t tid;
    int ret;
    ret=pthread_create ( &tid,NULL,dht_thread,NULL );
    if ( ret!=0 ) {
        fprintf ( stderr,"create thread failed\n" );
        return ;
    }
    //ret = pthread_detach(tid);
    pthread_join ( tid,NULL );
#endif

}
void getData_obstacle()    //vcc pin need insert 5V pin location
{
    wiringPiSetup();
    upload_type="x";
    int nValue=0;
    while ( 1 ) {
        pinMode ( GpioPort, INPUT );
        nValue=digitalRead ( GpioPort );
        //printf("obstacle_sensor value=%d\n",nValue);
        if ( nValue==1 ) {
            printf ( "Somebody maybe near the Device~~~\n" );
        } else {
            printf ( "Device is free~~\n" );
        }
        delay ( 300 );
    }
}

void getData_temphumi()
{

}
void getData_voice()     //insert 3.3v pin  (if have voice ,put 1 value)
{
    wiringPiSetup();
    upload_type="x";
    int nValue=0;
    while ( 1 ) {
        pinMode ( GpioPort, INPUT );
        nValue=digitalRead ( GpioPort );
        //printf("voice_sensor value=%d\n",nValue);
#if 1
        if ( nValue==1 ) {
            printf ( "Somebody maybe play voice now!~~~\n" );
        } else {
            printf ( "Device is quiet~~\n" );
        }
#endif
        delay ( 300 );
    }
}

void getData_traction()     //tracking sensor....insert 3.3V pin
{
    wiringPiSetup();
    upload_type="x";
    int nValue=0;
    while ( 1 ) {
        pinMode ( GpioPort, INPUT );
        nValue=digitalRead ( GpioPort );
        //printf("voice_sensor value=%d\n",nValue);
#if 1
        if ( nValue==0 ) {
            printf ( "Object maybe found under now!~~~\n" );
        } else {
            printf ( "Road is nothing now~~\n" );
        }
#endif
        delay ( 300 );
    }
}

void setData_buzzer()
{
    wiringPiSetup();
    upload_type="x";
    while ( 1 ) {
        pinMode ( GpioPort, OUTPUT );
        digitalWrite ( GpioPort,HIGH ); //buzzer quiet
        //printf("voice_sensor value=%d\n",nValue);
        delay ( 100 );
        digitalWrite ( GpioPort,LOW );  //set buzzer voice
        delay ( 100 );

    }
}
void getData_humanir()
{
    wiringPiSetup();
    upload_type="x";
    int nValue=0;
    while ( 1 ) {
        pinMode ( GpioPort, INPUT );
        nValue=digitalRead ( GpioPort );
        //printf("humanir_sensor value=%d\n",nValue);
#if 1
        if ( nValue==1 ) {
            printf ( "Somebody maybe near Device now!~~~\n" );
        } else {
            printf ( "Go away from Device~~\n" );
        }
#endif
        delay ( 300 );
    }


}
void DebugWorking()    //for generate Random Voltage & Current Value to test MIMO MQTT Services
{
    while ( Jason_mqtt_connect ( fox_mqtt_macaddrid,fox_mqtt_server_ip,fox_mqtt_port,fox_mqtt_username,fox_mqtt_password ) !=0 ) {
        printf ( "Please check network connection~\n" );
        sleep ( 1 );
    }

    upload_type="1";
    signal ( SIGALRM,handler );
    alarm ( 1 );

    while ( 1 ) {
        srand ( ( int ) time ( 0 ) );	//seed with time(0)
        v_Valuef = 0.01+ ( 50.0*rand() / ( RAND_MAX+1.0 ) );
        c_Valuef = 0.01+ ( 20.0*rand() / ( RAND_MAX+1.0 ) );
        printf ( "Voltage Value : %.2f\n",v_Valuef );
        printf ( "Current Value : %.2f\n",c_Valuef );

        memset ( datetime,'\0',sizeof ( datetime ) );
        gettime ( datetime );
        printf ( "************************%s\n",datetime );
        strcpy ( datetimeArray[iCycle],datetime );
        v_ArrayValuef[iCycle] = v_Valuef;
        c_ArrayValuef[iCycle] = c_Valuef;

        printf ( "=================================================%d\n",iCycle );
        iCycle++;
        //sleep(time_val/1000.0);
        //usleep(time_val);
        delay ( 100 );
        //getchar();
    }
    Jason_mqtt_disconnect();
}
int main()
{
    char *inifile = "config.ini";
    getconfigsetting ( inifile );

    /*
    	UART_Init("/dev/ttyUSB0",9600);
    	UART_SendMsg("1232156545");
    	UART_Close();

    return 1;


    	curl_SendData("http://192.168.1.110:3004/BSSAPI/V2/LoginBSS","wd=hava&hehe=123456");
    	return 1;
    */
    if ( sku_DebugWork ) {
        DebugWorking();
    }
    if ( sku_HBWorking ) {
        getData_HBWorking();
    }

    if ( sku_DisCharge ) {
        getData_DisCharge();   //get all raw data from YSD device
        getErrData_DisCharge();   //get raw data if over spec from YSD device
    }

    if ( sku_WireCut ) {
        getData_WireCut();
    }

    if ( sku_StamPing ) {
        getData_StamPing();
    }

    if ( sku_DemoRoom ) {
        getData_DemoRoom();
    }

    if ( iSensor_obstacle ) {
        getData_obstacle();
    }

    if ( iSensor_temphumi ) {
        getData_temphumi();
    }

    if ( iSensor_voice ) {
        getData_voice();
    }

    if ( iSensor_traction ) {
        getData_traction();
    }

    if ( iSensor_buzzer ) {
        setData_buzzer();
    }
    if ( iSensor_humanir ) {
        getData_humanir();
    }
    return 0;

}





