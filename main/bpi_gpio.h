#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <pcf8591.h>   //AD or DA 
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

int GPIO_Init(int pinIndex,int Mode)
{
	if(wiringPiSetup()==-1)          //wiringPiSetup for wiringPi    wiringPiSetupGpio for BCM GPIO
	{
		printf("Setup wiringPi Failed~~\n");		
		return -1;
	}
	pinMode(pinIndex,Mode);     //pinMode:OUTPUT (control other device)   :input (can get value from other device)
	printf("wiringPi GPIO Port setting OK~~\n");
	return 0; 
}

int GPIO_GetValue(int pinIndex)
{
	int nValue = 0;
	nValue = digitalRead(pinIndex);    //LOW =0 or HIGH = 1
	printf("Read Value From GPIO Port Is : %d \n",nValue);
	return nValue;
}
