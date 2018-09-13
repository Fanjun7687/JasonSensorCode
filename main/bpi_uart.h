#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

int fd;
int UART_Init(char *uartDev,uint32_t rate)
{
	fd = 0;	
	if((fd= serialOpen(uartDev,rate))<0)
	{
		printf("Init Uart Port Failed~~\n");		
		return -1;
	}
	printf("fd = %d\n",fd);
	return 0; 
}

void UART_SendMsg(char *msg)
{
	serialPuts(fd,msg);     //   send string to uart
	//serialPutchar(ufd,'1');               // send char to uart
}

void UART_GetValue(int pinIndex)
{
	char a;	
	int n;
	while(1)
	{
		scanf("%c",&a);
	}
	if(a>0)
	{
		serialPutchar(fd,a);
	}
	while(n=serialDataAvail(fd)>0)
	{
		printf("%c\n",serialGetchar(fd));	
	}
}

void UART_Close()
{
	serialClose(fd);
}

