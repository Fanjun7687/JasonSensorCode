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
    ifreq�ṹ������/usr/include/net/if.h����������ip��ַ������ӿڣ�����MTU�Ƚӿ���Ϣ�ġ�
    ���а�����һ���ӿڵ��� �ֺ;������ݡ������Ǹ������壬�п�����IP��ַ���㲥��ַ���������룬MAC�ţ�MTU���������ݣ���
    ifreq������ifconf�ṹ�С��� ifconf�ṹͨ���������������нӿڵ���Ϣ�ġ�
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
    return szRet;   //success ���� mac string����
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
