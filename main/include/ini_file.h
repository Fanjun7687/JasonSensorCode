#include <stdio.h>  
#include <string.h>  
//#pragma warning(disable:4996)
//
//
//
char *GetIniKeyString(char *title, char *key, char *filename)  
{  
    FILE *fp=NULL;  
    int  flag = 0;  
    char sTitle[32], *wTmp;  
    static char sLine[1024];  
  
    sprintf(sTitle, "[%s]", title);  
    if (NULL == (fp = fopen(filename, "r"))) {  
        fclose(fp);  
        perror("fopen");  
        return NULL;  
    }  
    else {  
        while (NULL != fgets(sLine, 1024, fp)) {  
            // 这是注释行  
            if (0 == strncmp("//", sLine, 2)) continue;  
            if ('#' == sLine[0])              continue;  
  
            wTmp = strchr(sLine, '=');  
            if ((NULL != wTmp) && (1 == flag)) {  
                if (0 == strncmp(key, sLine, wTmp - sLine)) { // 长度依文件读取的为准  
                    sLine[strlen(sLine) - 1] = '\0';  
                    fclose(fp);  
                    return wTmp + 1;  
                }  
            }  
            else {  
                if (0 == strncmp(sTitle, sLine, strlen(sLine) - 1)) { // 长度依文件读取的为准  
                    flag = 1; // 找到标题位置  
                }  
            }  
        }  
    }  
    fclose(fp);  
    return NULL;  
}
//
//
//
int GetIniKeyInt(char *title, char *key, char *filename)  
{  
    return atoi(GetIniKeyString(title, key, filename));  
}
//
//
//
float GetIniKeyFloat(char *title,char *key,char *filename)
{
	return atof(GetIniKeyString(title,key,filename));
}


int PutIniKeyString(char *title, char *key, char *val, char *filename)  
{  
    FILE *fpr, *fpw;  
    int  flag = 0;  
    char sLine[1024], sTitle[32], *wTmp;  
  
    sprintf(sTitle, "[%s]", title);  
    if (NULL == (fpr = fopen(filename, "r"))) {  
        fclose(fpr);  
        perror("fopen");  
        return -1;  
    }  
    sprintf(sLine, "%s.tmp", filename);  
    if (NULL == (fpw = fopen(sLine, "w"))){  
        fclose(fpw);  
        perror("fopen");  
        return -1;  
    }  
    while (NULL != fgets(sLine, 1024, fpr)) {  
        if (2 != flag) { // 如果找到要修改的那一行，则不会执行内部的操作  
            wTmp = strchr(sLine, '=');  
            if ((NULL != wTmp) && (1 == flag)) {  
                if (0 == strncmp(key, sLine, wTmp - sLine)) { // 长度依文件读取的为准  
                    flag = 2;// 更改值，方便写入文件  
                    sprintf(wTmp + 1, "%s\n", val);  
                }  
            }  
            else {  
                if (0 == strncmp(sTitle, sLine, strlen(sLine) - 1)) { // 长度依文件读取的为准  
                    flag = 1; // 找到标题位置  
                }  
            }  
        }  
  
        fputs(sLine, fpw); // 写入临时文件  
    }  
    fclose(fpr);  
    fclose(fpw);  
  
    sprintf(sLine, "%s.tmp", filename);  
    return rename(sLine, filename);// 将临时文件更新到原文件  
}
//
//
//
int PutIniKeyInt(char *title, char *key, int val, char *filename)  
{  
    char sVal[32];  
    sprintf(sVal, "%d", val);  
    return PutIniKeyString(title, key, sVal, filename);  
}  

int PutIniKeyFloat(char *title, char *key, float val, char *filename)  
{  
    char sVal[32];  
    sprintf(sVal, "%f", val);  
    return PutIniKeyString(title, key, sVal, filename);  
}  


void split(char **arr,char *str,const char *del)
{
	char *s=strtok(str,del);
	while(s!=NULL)
	{
		*arr++=s;
		s=strtok(NULL,del);
	}
}

char *getContentBySplit(char *src,char *key1,char *key2)
{
	char *p1,*p2;
	char *getContent=(char*)malloc(1024);  //without free(getContent)
	p1=strstr(src,key1);
	p2=strstr(src,key2);
	if(p1==NULL||p2==NULL||p1>p2)
	{
		return getContent;
	}
	else
	{
		p1+=strlen(key1);
		memcpy(getContent,p1,p2-p1);
		printf("%s\n",getContent);
	}
	return getContent;
}



