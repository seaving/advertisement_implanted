/********************************************************
 *  IP报头格式数据结构定义在<netinet/ip.h>中  *
 *  ICMP数据结构定义在<netinet/ip_icmp.h>中       *
 *  套接字地址数据结构定义在<netinet/in.h>中   *
 ********************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
 
#define PACKET_SIZE 4096
#define MAX_WAIT_TIME   5
#define MAX_NO_PACKETS  10000
 
 
char *addr[];
char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
int sockfd,datalen = 56;
int nsend = 0, nreceived = 0;
double temp_rtt[MAX_NO_PACKETS];
double all_time = 0;
double min = 0;
double max = 0;
double avg = 0;
double mdev = 0;
 
struct sockaddr_in dest_addr;
struct sockaddr_in from;
struct timeval tvrecv;
pid_t pid;
 
void statistics(int sig);
void send_packet(void);
void recv_packet(void);
void computer_rtt(void);
void tv_sub(struct timeval *out,struct timeval *in);
int pack(int pack_no);
int unpack(char *buf,int len);
unsigned short cal_checksum(unsigned short *addr,int len);
 
/*计算rtt最小、大值，平均值，算术平均数差*/
void computer_rtt()
{
    double sum_avg = 0;
    int i;
    min = max = temp_rtt[0];
    avg = all_time/nreceived;
 
    for(i=0; i<nreceived; i++){
        if(temp_rtt[i] < min)
            min = temp_rtt[i];
        else if(temp_rtt[i] > max)
            max = temp_rtt[i];
 
        if((temp_rtt[i]-avg) < 0)
            sum_avg += avg - temp_rtt[i];
        else
            sum_avg += temp_rtt[i] - avg; 
        }
    mdev = sum_avg/nreceived;
}
 
/****统计数据函数****/
void statistics(int sig)
{
    computer_rtt();     //计算rtt
    printf("\n------ %s ping statistics ------\n",addr[0]);
    printf("%d packets transmitted,%d received,%d%% packet loss,time %.f ms\n",
        nsend,nreceived,(nsend-nreceived)/nsend*100,all_time);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
        min,avg,max,mdev);
    close(sockfd);
    exit(1);
}
 
/****检验和算法****/
unsigned short cal_chksum(unsigned short *addr,int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short check_sum = 0;
 
    while(nleft>1)       //ICMP包头以字（2字节）为单位累加
    {
        sum += *w++;
        nleft -= 2;
    }
 
    if(nleft == 1)      //ICMP为奇数字节时，转换最后一个字节，继续累加
    {
        *(unsigned char *)(&check_sum) = *(unsigned char *)w;
        sum += check_sum;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    check_sum = ~sum;   //取反得到校验和
    return check_sum;
}
 
/*设置ICMP报头*/
int pack(int pack_no)
{
    int i,packsize;
    struct icmp *icmp;
    struct timeval *tval;
    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO; //ICMP_ECHO类型的类型号为0
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = pack_no;    //发送的数据报编号
    icmp->icmp_id = pid;
 
    packsize = 8 + datalen;     //数据报大小为64字节
    tval = (struct timeval *)icmp->icmp_data;
    gettimeofday(tval,NULL);        //记录发送时间
    //校验算法
    icmp->icmp_cksum =  cal_chksum((unsigned short *)icmp,packsize); 
    return packsize;
}
 
/****发送三个ICMP报文****/
void send_packet()
{
    int packetsize;
    if(nsend < MAX_NO_PACKETS)
    {
        nsend++;
        packetsize = pack(nsend);   //设置ICMP报头
        //发送数据报
        if(sendto(sockfd,sendpacket,packetsize,0,
            (struct sockaddr *)&dest_addr,sizeof(dest_addr)) < 0)
        {
            perror("sendto error");
        }
    }
 
}
 
 
/****接受所有ICMP报文****/
void recv_packet()
{
    int n,fromlen;
    extern int error;
    fromlen = sizeof(from);
    if(nreceived < nsend)
    {   
        //接收数据报
        if((n = recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,
            (struct sockaddr *)&from,&fromlen)) < 0)
        {
            perror("recvfrom error");
        }
        gettimeofday(&tvrecv,NULL);     //记录接收时间
        unpack(recvpacket,n);       //剥去ICMP报头
        nreceived++;
    }
}
 
 
/******剥去ICMP报头******/
int unpack(char *buf,int len)
{
    int i;
    int iphdrlen;       //ip头长度
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;
    double rtt;
 
 
    ip = (struct ip *)buf;
    iphdrlen = ip->ip_hl << 2; //求IP报文头长度，即IP报头长度乘4
    icmp = (struct icmp *)(buf + iphdrlen); //越过IP头，指向ICMP报头
    len -= iphdrlen;    //ICMP报头及数据报的总长度
    if(len < 8)      //小于ICMP报头的长度则不合理
    {
        printf("ICMP packet\'s length is less than 8\n");
        return -1;
    }
    //确保所接收的是所发的ICMP的回应
    if((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))
    {
        tvsend = (struct timeval *)icmp->icmp_data;
        tv_sub(&tvrecv,tvsend); //接收和发送的时间差
        //以毫秒为单位计算rtt
        rtt = tvrecv.tv_sec*1000 + tvrecv.tv_usec/1000;
        temp_rtt[nreceived] = rtt;
        all_time += rtt;    //总时间
        //显示相关的信息
        printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n",
                len,inet_ntoa(from.sin_addr),
                icmp->icmp_seq,ip->ip_ttl,rtt);
    }
    else return -1;
}
 
 
//两个timeval相减
void tv_sub(struct timeval *recvtime,struct timeval *sendtime)
{
    long sec = recvtime->tv_sec - sendtime->tv_sec;
    long usec = recvtime->tv_usec - sendtime->tv_usec;
    if(usec >= 0){
        recvtime->tv_sec = sec;
        recvtime->tv_usec = usec;
    }else{
        recvtime->tv_sec = sec - 1;
        recvtime->tv_usec = -usec;
    }
}
 
/*主函数*/
main(int argc,char *argv[])
{
    struct hostent *host;
    struct protoent *protocol;
    unsigned long inaddr = 0;
//  int waittime = MAX_WAIT_TIME;
    int size = 50 * 1024;
    addr[0] = argv[1];
    //参数小于两个
    if(argc < 2)     
    {
        printf("usage:%s hostname/IP address\n",argv[0]);
        exit(1);
    }
    //不是ICMP协议
    if((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("getprotobyname");
        exit(1);
    }
 
    //生成使用ICMP的原始套接字，只有root才能生成
    if((sockfd = socket(AF_INET,SOCK_RAW,protocol->p_proto)) < 0)
    {
        perror("socket error");
        exit(1);
    }
 
    //回收root权限，设置当前权限
    setuid(getuid());
 
    /*扩大套接字的接收缓存区导50K，这样做是为了减小接收缓存区溢出的
      可能性，若无意中ping一个广播地址或多播地址，将会引来大量的应答*/
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
    bzero(&dest_addr,sizeof(dest_addr));    //初始化
    dest_addr.sin_family = AF_INET;     //套接字域是AF_INET(网络套接字)
 
    //判断主机名是否是IP地址
    if(inet_addr(argv[1]) == INADDR_NONE)
    {
        if((host = gethostbyname(argv[1])) == NULL) //是主机名
        {
            perror("gethostbyname error");
            exit(1);
        }
        memcpy((char *)&dest_addr.sin_addr,host->h_addr,host->h_length);
    }
    else{ //是IP 地址
        dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    pid = getpid();
    printf("PING %s(%s):%d bytes of data.\n",argv[1],
            inet_ntoa(dest_addr.sin_addr),datalen);
 
    //当按下ctrl+c时发出中断信号，并开始执行统计函数
    signal(SIGINT,statistics);  
    while(nsend < MAX_NO_PACKETS){
        sleep(1);       //每隔一秒发送一个ICMP报文
        send_packet();      //发送ICMP报文
        recv_packet();      //接收ICMP报文
    }
    return 0;
}




