#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>

enum NetInterfaceType {eth0,wlan0};

int get_mac ( char *mac, int imaclen )
{
    /*
    ifreq结构定义在/usr/include/net/if.h，用来配置ip地址，激活接口，配置MTU等接口信息的。
    其中包含了一个接口的名 字和具体内容——（是个共用体，有可能是IP地址，广播地址，子网掩码，MAC号，MTU或其他内容）。
    ifreq包含在ifconf结构中。而 ifconf结构通常是用来保存所有接口的信息的。
    */
    struct ifreq ifreq;
    int sock;
//default ethname is "eth0"
    strcpy ( ifreq.ifr_name,"wlan0" ); //eth0
    if ( ( sock=socket ( AF_INET,SOCK_STREAM,0 ) ) <0 ) {
        perror ( "Socket Error" );
        return -1;
    }
    if ( ioctl ( sock,SIOCGIFHWADDR,&ifreq ) <0 ) {
        perror ( "Ioctl Error" );
        return -1;
    }
    int szRet=snprintf ( mac,imaclen,"%02X%02X%02X%02X%02X%02X", ( unsigned char ) ifreq.ifr_hwaddr.sa_data[0],
                         ( unsigned char ) ifreq.ifr_hwaddr.sa_data[1],
                         ( unsigned char ) ifreq.ifr_hwaddr.sa_data[2],
                         ( unsigned char ) ifreq.ifr_hwaddr.sa_data[3],
                         ( unsigned char ) ifreq.ifr_hwaddr.sa_data[4],
                         ( unsigned char ) ifreq.ifr_hwaddr.sa_data[5] );
    return szRet;   //success 返回 mac string长度
}

int connect_statusCheck (enum NetInterfaceType k)    //
{
    int net_fd;
    char status[20]= {'\0'};
    switch ( k ) {
    case eth0: {
        net_fd=open ( "sys/class/net/eth0/operstate",O_RDONLY );
    }
    break;
    case wlan0: {
        net_fd=open ( "sys/class/net/wlan0/operstate",O_RDONLY );
    }
    break;
    }
    if ( net_fd<0 ) {
        fprintf ( stderr,"Open Err\n" );
        return 0;
    }
    puts ( "Open OK\n" );
    int ret=read ( net_fd,status,10 );
    printf ( "Status is %s\n",status );
    if ( strstr ( status,"up" ) !=0 ) {
        puts ( "On Line\n" );
        return 1;
    } else if ( strstr ( status,"down" ) !=0 ) {
        puts ( "Off Line\n" );
        return 0;
    } else {
        puts ( "Unkown status\n" );
        return 0;
    }


}
/*
int main(int argc,char* argv[])
{
        char szMac[18];
        int nRtn=get_mac(szMac,sizeof(szMac));
        if(nRtn > 0)
        {
                fprintf(stderr,"MAC ADRESS : %s \n",szMac);
        }
        return 0;

}
*/
