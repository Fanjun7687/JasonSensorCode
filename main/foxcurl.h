//curl :  gcc ?.c -o ? -lcurl
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <iconv.h>

#define URLADDR "http://192.168.1.110:3004/BSSAPI/V2/LoginBSS"
#define SENDDATA "wd=hava&hehe=123456"
CURL *curl;
CURLcode res;
char m_replyData[1024]={'\0'};
struct curl_slist *list=NULL;


static size_t getreply_data(void *buffer,size_t size,size_t nmemb,void *stream)
{
	(void)buffer;
	strcat(m_replyData,(char *)buffer);
	//printf("======> %s\n",m_replyData);
	//(void)stream;
	return size*nmemb;
	
}

void curl_GetReplyInfo(char *m_reply)
{
	strcpy(m_reply,m_replyData);
}

int curl_SendData(char *urladdr,char *senddata)
{
	int iRet=1;
	
	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl,CURLOPT_URL,urladdr);
		curl_easy_setopt(curl,CURLOPT_TIMEOUT,5);   //
		curl_easy_setopt(curl,CURLOPT_POST,1L);
		list = curl_slist_append(list,"Accept:application/json");
		list = curl_slist_append(list,"Content-Type:application/json;charset=utf-8");   //must to setopt json HTTPHEADER
		curl_easy_setopt(curl,CURLOPT_HTTPHEADER,list);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,getreply_data);
/*
		curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
		curl_easy_setopt(curl,CURLOPT_HEADER,1);
		curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
*/		
		curl_easy_setopt(curl,CURLOPT_POSTFIELDS,senddata);
		memset(m_replyData,'\0',1024);
		res=curl_easy_perform(curl);
		if(res!=CURLE_OK)
		{
			fprintf(stderr,"perform failed : %s\n",curl_easy_strerror(res));
		}
		else
		{
			puts("send data ok~~\n");iRet=0;
		}
		curl_easy_cleanup(curl);
		return iRet;
	}
}





/*
int main(int argc,char *argv[])
{
	curl_SendData(URLADDR,SENDDATA);	
	exit(EXIT_SUCCESS);
	

}
*/
