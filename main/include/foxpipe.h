//gcc myTest.c -o myTest
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>

int runCmd_PipeOutivalue(char *cmd)   //read pipe out from extern programe
{
	FILE *read_fp;
	char *buffer=(char *)malloc(BUFSIZ+1);
	int chars_read,iRet;
	memset(buffer,'\0',sizeof(buffer));
	read_fp = popen(cmd,"r");  //the handle of open pipe
	if(read_fp!=NULL)
	{
		chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);  //
		if(chars_read>0)
		{
			//printf("Pipe Output was : %s\n",buffer);
		}
		pclose(read_fp);
		iRet=atoi(buffer);
		free(buffer);
	}
	return iRet;
}

int runCmd_OpenOutivalue(char *cmd)     //top cpu loading < runCmd_PipeOutivalue()
{
	char buffer[10]={0};
	int fp=open(cmd,O_RDONLY);
	int r=read(fp,buffer,sizeof(buffer));
	if(r<0)
	{
	  close(fp); return 0;
	}
        close(fp);
	return atoi(buffer);
}




int runCmd_PipeOut(char *cmd)   //read pipe out from extern programe
{
	FILE *read_fp;
	char buffer[BUFSIZ+1];
	int chars_read;
	memset(buffer,'\0',sizeof(buffer));
	read_fp = popen(cmd,"r");  //the handle of open pipe
	if(read_fp!=NULL)
	{
		chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);  //BUFSIZ defined in stdio.h 8192
		if(chars_read>0)
		{
			printf("Pipe Output was : %s\n",buffer);
		}
		pclose(read_fp);
		return 0;
	}
	return 1;
}

int runCmd_PipeOutMore(char *cmd)   //read pipe out from extern programe
{
	FILE *read_fp;
	char buffer[BUFSIZ+1];
	int chars_read;
	memset(buffer,'\0',sizeof(buffer));
	read_fp = popen(cmd,"r");  //the handle of open pipe
	if(read_fp!=NULL)
	{
		chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);  //BUFSIZ defined in stdio.h 8192
		bool bGetOnce=false;
		while(chars_read>0)
		{
			buffer[chars_read -1]='\0';
			if(!bGetOnce) {printf("Pipe Output was : -\n%s\n",buffer);bGetOnce=true;}
			else          printf("%s\n",buffer);
			chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);
		}
		pclose(read_fp);
		return 0;
	}
	return 1;
}


int runCmd_PipeIn(char *cmd)    // echo "????" | od -c
{
	FILE *write_fp;
	char buffer[BUFSIZ+1];
	strcpy(buffer,cmd);
	write_fp=popen("od -c","w");
	puts("Pipe Input was :");
	if(write_fp!=NULL)
	{
		fwrite(buffer,sizeof(char),strlen(buffer),write_fp);
		pclose(write_fp);
		return 0;
	}
	return 1;

}

