#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "my_c_string_deal_tools.h"

#define bool short
#define true 1
#define false 0

#define MY_MACRO_INT unsigned int
#define INT_BYTE_LENGTH sizeof(MY_MACRO_INT)

#define QUEUE 200
#define BUFFER_SIZE (1024*512)
#define MAX_THREAD_NUM 200
#define IP_SIZE 48
#define MAC_SIZE 64

#define CONNECT 1
#define REQUEST_PARTNER_HOST_INFO 2
#define DISCONNECT 3
#define KEEP_ALIVE_SIGNAL 4

#define REQUESTED_HOST_NOT_EXISTS 5
#define REQUESTED_YOUR_SELF 6
#define REQUEST_SUCCESS 7
#define REQUEST_UNKOWN_ERROR 8
#define RECEIVED_STRING_TOO_SHORT 9

#define REQUEST_TO_SHARE_SCREEN 10
#define REQUEST_TO_CONTROL_DEVICE_BY_REMOTE 11
#define YOU_WARE_REQUESTED_TO_BE_CONNECTED_BY_THIS_HOST 12
#define I_AM_READY_TO_BE_CONNECTED 13
#define PARTNER_HOST_ARE_ALSO_READY_TO_BE_CONNECTED 14
#define I_AM_NOT_ALLOW_TO_BE_CONNECTED 15
#define YOUR_PARTNER_DONOT_ALLOW_TO_BE_CONNECTED 16
#define I_WILL_EXIT 17

#define THIS_IS_PICTURE_DATA 18
#define CURRENT_DATA_IS_TOO_LONG 19

//char * dataEnd="TheEnd";
//#define END_FLAG_LENGTH strlen(dataEnd)

//#define EXISTS -1
//#define NOTEXISTS 0

char * logFilePath="./log";//日志路径
char * logFileName="/serverOfSSAC.log";//日志文件名
char logFile[256]={'\0'};//日志文件的路径+文件名
int thread_count=0;//线程计数器
pthread_t threads[MAX_THREAD_NUM];
int threads_head_index=0;
int threads_tail_index=0;

char * conf="./conf/server.conf";//服务器端配置文件路径
int server_default_port=0;//服务器默认准备监听的端口
int port_max_difference_value=5;//如果端口被占用，则尝试进行另一个端口的最大差值
int server_final_port=0;//程序最终绑定使用的端口值

pthread_mutex_t arr_mutex;//写内存中的主机名ip地址对数组的互斥锁

typedef struct Pthread_Para{
	int socket_ser;
	struct sockaddr_in client_addr;
}Pthread_Para,*Pthread_Para_ptr;

typedef struct MAC_addr{//存储MAC地址的链表节点
	char mac[MAC_SIZE];//mac地址
	struct MAC_addr *next;//下一节点的指针
}MAC_addr,*MAC_addr_ptr;

typedef struct Host_Info{//在内存中存储客户机的ID、密码、以及其所有硬件地址
	int hostID;
	int hostPasswd;
	char hostIP[IP_SIZE];
	int hostPort;
	MAC_addr mac;
}HostInfo,*HostInfo_ptr;

//HostInfo_ptr  hosts[MAX_THREAD_NUM];//存储主机ID及其ip等信息的数组，数组大小和最大线程数相同

//Pthread_Para_ptr socket_s[MAX_THREAD_NUM];//存储当前的tcp连接信息，和hosts数组中的主机相对应

typedef struct Hosts_And_Sockets_And_Their_Partner{//这里存储主机信息，主机对应的套接字以及主机对应的伙伴机
	HostInfo_ptr  the_host;
	Pthread_Para_ptr the_socket;
	int index_of_partner;
}Hosts_And_Sockets_And_Their_Partner,*Hosts_And_Sockets_And_Their_Partner_ptr;

Hosts_And_Sockets_And_Their_Partner hosts_sockets[MAX_THREAD_NUM];//存储主机ID及其ip等信息的数组，也存储当前的tcp连接信息，数组大小和最大线程数相同
int hosts_index=0;//hosts_sockets数组的当前可存储位置的下标


void init();
long file_size(char*);
char *getLocalTime();
void child_thread(Pthread_Para_ptr);
int search_host(HostInfo_ptr);
int recod_host(HostInfo_ptr);
void outputLog(char *);
int te_connect(int *,Pthread_Para_ptr,char *);
int te_request(int *,Pthread_Para_ptr,char *);
int te_disconnect(int *,Pthread_Para_ptr,char *);
int te_keepalive(int *,Pthread_Para_ptr,char *);
int reply_partner_is_ready(int *,Pthread_Para_ptr,char *);
int partner_donot_allow_to_be_connected(int *,Pthread_Para_ptr,char *);
int picture_data_transfer(int *,Pthread_Para_ptr,char *);

int main()
{
	//初始化
	init();


	pthread_mutex_init(&arr_mutex,NULL);
	memset(hosts_sockets,0,sizeof(hosts_sockets));
	int i = 0;
	for(i = 0;i<MAX_THREAD_NUM;i++)//这里需要对存储伙伴主机下标的域进行初始化，初始化值为-1，因为只有-1才表示当前没有伙伴主机连接
		hosts_sockets[i].index_of_partner = -1;
	//memset(socket_s,0,sizeof(socket_s));

	///定义sockfd
	int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

	///定义sockaddr_in
	struct sockaddr_in server_sockaddr;
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(server_default_port);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bool bind_success_flag=false;
	server_final_port=server_default_port;
	while(!bind_success_flag && server_final_port <= (server_default_port + port_max_difference_value))
	{//该循环用于在配置文件指定的端口被占用或者绑定不成功的时候，自动尝试下一个端口，再继续尝试port_max_difference_value个端口，如果都不成功，则程序退出

		server_sockaddr.sin_port = htons(server_final_port);

		///bind，成功返回0，出错返回-1
		if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
		{
			char error_str[256]={'\0'};
			sprintf(error_str,"bind %d failed : ",server_final_port);
			perror(error_str);
			bind_success_flag=false;
			server_final_port++;
		}else
		{
			char logStr[256]={'\0'};
			sprintf(logStr,"Final bind %d port.\n",server_final_port);
			//perror(logStr);
			outputLog(logStr);
			bind_success_flag = true;
			break;
		}
	}
	if(!bind_success_flag)//如果尝试了所有端口都不成功，则放弃治疗
		exit(1);

	///listen，成功返回0，出错返回-1
	while(listen(server_sockfd,QUEUE) == -1)
	{
		perror("listen");
	}

	// 开始处理客户端连接,初始化线程标识符数组
	bzero(&threads, sizeof(pthread_t) * MAX_THREAD_NUM);
	while(1)
	{
		///客户端套接字
		Pthread_Para_ptr client;//client存放与客户机的连接句柄和客户机ip
        	client=(Pthread_Para_ptr)calloc(1,sizeof(Pthread_Para));
		socklen_t length=sizeof(client->client_addr);

		///建立客户端链接，成功返回非负描述字，出错返回-1
		if((client->socket_ser = accept(server_sockfd, (struct sockaddr*)&(client->client_addr), &length))<0)
		{
			perror("connect error");
			continue;
		}

		if (pthread_create(threads + (thread_count++), NULL, (void *)child_thread, (void *)client) != 0)
		{
			printf("Create thread Failed!\n");
			return EXIT_FAILURE;
		}

		//pthread_join( threads[ thread_count - 1 ], NULL );
		//命令主线程等待新创建的线程执行完毕后再退出，该命令执行后，主线程会迅速阻塞等待子线程。
		//具体见博文【 https://www.cnblogs.com/kex1n/p/7111589.html 】

	}
	close(server_sockfd);
	return 0;
}

void init()
{
	char *flag=NULL;//辅助指针

	//初始化日志文件路径
	flag=logFile;
	strncpy(logFile,logFilePath,strlen(logFilePath));
	flag+=strlen(logFilePath);
	strncpy(flag,logFileName,strlen(logFileName));
	flag=NULL;

	///读取配置文件./conf/server.conf
	char confBuffer[BUFFER_SIZE]={'\0'};
	FILE * fconf=NULL;
	if((fconf=fopen(conf,"r"))==NULL)
	{
        	perror("open config file");
        	exit(1);
	}
	while(!feof(fconf))
	{
		memset(confBuffer,0,sizeof(confBuffer));
		fgets(confBuffer,sizeof(confBuffer)-1,fconf);
		char *flag=NULL;
		int cpLen=0;
		if( ( flag=strstr(confBuffer,"port=") ) != NULL && strlen(confBuffer) > strlen("port=") )
		{//从配置文件中提取端口号
			cpLen=strlen(confBuffer)-5-1;
			flag+=5;
			char portTemp[32]={'\0'};
			strncpy(portTemp,flag,cpLen);
			server_default_port=atoi(portTemp);
			flag=NULL;
		}
	}
	fclose(fconf);
	if(server_default_port==0)
	{
        	perror("get server_default_port error");
       	 	exit(1);
	}
}

long file_size(char* filename)
{//获取文件大小
	struct stat statbuf;
	stat(filename,&statbuf);
	long size=statbuf.st_size;

	return size;
}

char *getLocalTime()
{
	time_t timep;
	time(&timep);
	return asctime(localtime(&timep));
}

void child_thread(Pthread_Para_ptr para)
{
	Pthread_Para_ptr client=para;
	int sockfd=client->socket_ser;
	char recv_buffer[BUFFER_SIZE];
	bzero(recv_buffer, BUFFER_SIZE);

	int self_index = -1;

	char writeLogStr[BUFFER_SIZE];
	sprintf(writeLogStr,"当前保活的TCP链接数是：%d个，当前缓存中的主机数是：%d台。\t%s",thread_count,hosts_index,getLocalTime());
	outputLog(writeLogStr);

	int ret_flag=-1;

	while(1)
	{//为了保持了这条链接而不关闭，故使用这种间隔发送消息的机制
		int len = recv(sockfd, recv_buffer, BUFFER_SIZE, 0);
		//replace(recv_buffer,"\n","\0");//收到的字符串末尾可能会有一个换行，去掉换行符
		//replace(recv_buffer,"\r","\0");//收到的字符串末尾可能会有一个回车，去掉回车符
		len=INT_BYTE_LENGTH * 2 + strlen(recv_buffer + INT_BYTE_LENGTH * 2);//重新计算接收到的字节数

		if(!len)
			break;//如果收到的信息长度为0，则不需要进行后面的操作

		//根据客户机发来的请求类型决定要进行什么操作
		//char temp[20] = {'\0'};
		//strncpy(temp,recv_buffer,indexOf(recv_buffer,"@"));
		MY_MACRO_INT data_class_flag;
		memcpy(&data_class_flag, recv_buffer, INT_BYTE_LENGTH);

		//switch(atoi(temp))
		switch(data_class_flag)
		{
		case CONNECT://该客户机初次启动，进行连接
			te_connect(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case REQUEST_PARTNER_HOST_INFO://客户机请求另一客户机相关信息
			te_request(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case DISCONNECT://客户机将退出程序，故断开连接
			ret_flag = te_disconnect(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case KEEP_ALIVE_SIGNAL://客户端的链接保活回应
			te_keepalive(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case I_AM_READY_TO_BE_CONNECTED://被请求连接的一方已经准备好了被链接
			reply_partner_is_ready(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case I_AM_NOT_ALLOW_TO_BE_CONNECTED://被请求的一方拒绝了链接请求
			partner_donot_allow_to_be_connected(&self_index,client,recv_buffer + INT_BYTE_LENGTH);
			break;
		case THIS_IS_PICTURE_DATA://这里接收到的就是本主机的屏幕截屏数据，需要将其转发给伙伴主机
			picture_data_transfer(&self_index,client,recv_buffer);
			break;
		default://在无法分辨的情况下，极有可能是图片数据，客户端自会分辨，因此这里做个转发
			picture_data_transfer(&self_index,client,recv_buffer);
			break;
		}
		if(ret_flag >= 0)
			break;//说明接收到了断开连接的请求
	}

	if(ret_flag >= 0)
	{
		//将程序员挂起一段时间，等待完成对主机的下线请求的回应，这里挂起10毫秒
		usleep(10000);

		close(sockfd);
		
		//接下来从内存中删除选中的主机
		pthread_mutex_lock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁
		free(hosts_sockets[ret_flag].the_host);
		//同时还要删除对应的套接字
		if(hosts_sockets[ret_flag].the_socket)
		{
			free(hosts_sockets[ret_flag].the_socket);//如果前面已经存在该连接的套接字，则删除之
			hosts_sockets[ret_flag].the_socket=NULL;
		}
		int j=0;
		for(j = ret_flag;j<MAX_THREAD_NUM - 1;j++)
			memcpy(&hosts_sockets[j+1], &hosts_sockets[j], sizeof(Hosts_And_Sockets_And_Their_Partner));
		hosts_sockets[MAX_THREAD_NUM - 1].the_host = NULL;
		hosts_sockets[MAX_THREAD_NUM - 1].the_socket = NULL;
		hosts_sockets[MAX_THREAD_NUM - 1].index_of_partner = -1;
		hosts_index--;
		thread_count--;
		pthread_mutex_unlock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁

		return;
	}

	close(sockfd);
	free(client);
	thread_count--;
}

int search_host(HostInfo_ptr host)
{//查找该主机是否已经注册，如果已经注册则返回其下标，否则返回-1
	int i=0;
	if(hosts_index<0)
		return 0;
	for(i=0;i<hosts_index;i++)
	{
		MAC_addr_ptr temp1=&(hosts_sockets[i].the_host->mac);
		while(temp1)
		{
			MAC_addr_ptr temp2=&(host->mac);
			while(temp2)
			{
				if(strcmp(temp1->mac,temp2->mac)==0)
					return i;
				temp2=temp2->next;
			}

			temp1=temp1->next;
		}
	}
	return -1;
}

int recod_host(HostInfo_ptr host)
{
	if(hosts_index>=MAX_THREAD_NUM)
	{
		outputLog("warning : The buffer array of hostName was full!");
		return 0;
	}
	pthread_mutex_lock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁
	hosts_sockets[hosts_index++].the_host=host;
	pthread_mutex_unlock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁

	//把目前内存中保存的主机情况全部输出一遍
	int i = 0;
	for(i=0;i<hosts_index;i++)
	{
		HostInfo_ptr temp = hosts_sockets[i].the_host;
		char buf[BUFFER_SIZE];
		sprintf(buf,"Host %d:{hostID:%d,hostPasswd:%d,hostIP:%s,hostPort:%d,(",i,temp->hostID,temp->hostPasswd,temp->hostIP,temp->hostPort);
		MAC_addr_ptr temp1 = &temp->mac;
		int nu = 0;
		while(temp1)
		{
			nu++;
			char *cp=buf + strlen(buf);
			sprintf(cp,"MAC %d:%s,",nu,temp1->mac);
			temp1 = temp1->next;
		}
		char *pp=buf + strlen(buf);
		sprintf(pp,")}\t%s",getLocalTime());
		outputLog(buf);
	}
	return hosts_index - 1;//将该主机信息存储以后，返回该主机信息存储在数组中的下标
}

void outputLog(char *buffer)
{
    //组织创建日志路径的语句
    char logPathBuffer[256]={'\0'};
    sprintf(logPathBuffer,"mkdir -p %s",logFilePath);

	FILE *log=NULL;
	if( (access( logFile, 0 )) == -1 )
	{//日志文件不存在则新建
        system(logPathBuffer);//创建日志文件路径
		creat(logFile,0755);
	}else
	{//日志文件没有读写权限时删除并新建
		if( (access( logFile, 6 )) == -1 )
		{
			remove(logFile);
            creat(logFile,0755);
        }
    }
	if(file_size(logFile)>10485760)
	{//日志文件存在且大于10M时，删除文件，然后重新创建该文件
        remove(logFile);
		creat(logFile,0755);
	}
	if((log=fopen(logFile,"a+"))==NULL)
	{
		perror("open log file");
		exit(1);
	}

	fputs(buffer,log);
	//fputs("\n",log);

	fclose(log);
}
int te_connect(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{
	/*
	 * 该函数用于处理客户机的连接请求
	 * 将在对比已经连接的客户机的MAC地址之后，如果该MAC地址尚未注册，则注册之，并生成一个ID和相应密码，发送给客户机
	 */

	MY_MACRO_INT fl;

	int sockfd=client->socket_ser;
	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	//char *sub_recv_buffer=recv_buffer + indexOf(recv_buffer,"@") + 1;
	char *sub_recv_buffer=recv_buffer;
	int len=strlen(sub_recv_buffer);//重新计算接收到的字节数

	if(!len)
	{
		fl = CONNECT;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",RECEIVED_STRING_TOO_SHORT,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		goto tail;//如果收到的信息长度为0，则不需要进行后面的操作
	}

	//将客户机信息记录到hosts_sockets数组中
	HostInfo_ptr tempHost=(HostInfo_ptr)calloc(1,sizeof(HostInfo));//记录该线程连接的主机信息
	bzero(tempHost, sizeof(HostInfo));
	//获取客户机公网IP
	while( inet_ntop(AF_INET,&client->client_addr.sin_addr,tempHost->hostIP,IP_SIZE) == NULL )
	{
		perror("child_thread(Pthread_Para_ptr) : inet_ntop ");
	}
	tempHost->hostPort = client->client_addr.sin_port;//获取客户端的端口号
	MAC_addr_ptr current_mac=&(tempHost->mac);
	strncpy(current_mac->mac,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//把刚刚收到的其中一个MAC地址保存到临时的变量中
	sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
	while(strlen(sub_recv_buffer))
	{
		MAC_addr_ptr a_mac=(MAC_addr_ptr)calloc(1,sizeof(MAC_addr));
		strncpy(a_mac->mac,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//把刚刚收到的其中一个MAC地址保存到临时的变量中
		current_mac->next = a_mac;
		current_mac = current_mac-> next;
		sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
	}

	//在客户机表中查找看有没有当前主机已经存在，如果不存在，则为其生成ID和密码，如果存在，则返回其在主机数组中的下标
	int index=search_host(tempHost);
	if(index==-1)
	{//该主机尚未注册，则注册之
		int tempID=0,tempPasswd=0;
		srand( (unsigned)time( NULL ) );
		tempID += rand()% 1000;
		//为了下一次产生的随机数不同，这里将当前系统时间除以10000取余
		srand( (unsigned)time( NULL )%10000 );
		tempPasswd += rand()%1000;
		tempHost->hostID=tempID;
		tempHost->hostPasswd=tempPasswd;

		//将该主机信息记录到主机数组中
		*self_index = recod_host(tempHost);//返回该主机所存储在的数组中的下标
		index=hosts_index - 1;
	} else//index不等于-1说明原主机是存在的，此时更新主机信息
	{//更新该主机的信息，因为有可能该主机套接字可以反映出该主机的IP和端口都应经发生了变化
	//此处采取的更新办法是，释放原主机结构体，重新建立一个结构并加入主机数组中
		tempHost->hostID = hosts_sockets[index].the_host->hostID;
		tempHost->hostPasswd = hosts_sockets[index].the_host->hostPasswd;

		pthread_mutex_lock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁
		free(hosts_sockets[index].the_host);//首先释放旧的主机信息
		hosts_sockets[index].the_host = tempHost;//然后添加主机的新信息到原位置
		pthread_mutex_unlock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁

		*self_index = index;//记录该主机的存放位置的下标
	}

	//将该主机的套接字存储到套接字数组的对应位置
	pthread_mutex_lock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁
	if(hosts_sockets[index].the_socket)
	{
		free(hosts_sockets[index].the_socket);//如果前面已经存在该连接的套接字，则删除之
		hosts_sockets[index].the_socket=NULL;
	}
	hosts_sockets[index].the_socket=client;
	pthread_mutex_unlock(&arr_mutex);//写内存中的主机名ip地址对数组的互斥锁

	//生成返回字符串
	fl = CONNECT;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%d@%d@%s@%d@%s@\t%s",REQUEST_SUCCESS,hosts_sockets[index].the_host->hostID,hosts_sockets[index].the_host->hostPasswd,\
			hosts_sockets[index].the_host->hostIP,hosts_sockets[index].the_host->hostPort,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	tail:

	//写日志文件
	outputLog(log_buffer);

	//向客户机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	send(sockfd, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);

	return 0;
}
int te_request(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{
	int sockfd=client->socket_ser;
	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);
	char send_buffer_oth[BUFFER_SIZE];
	bzero(send_buffer_oth, BUFFER_SIZE);
	char log_buffer_oth[BUFFER_SIZE];
	bzero(log_buffer_oth, BUFFER_SIZE);
	int tem_flag=0;//用来标记是否找到了伙伴主机

	MY_MACRO_INT fl;

	//char *sub_recv_buffer=recv_buffer + indexOf(recv_buffer,"@") + 1;
	char *sub_recv_buffer=recv_buffer;
	int len=strlen(sub_recv_buffer);//重新计算接收到的字节数
	if(!len)
	{
		fl = REQUEST_PARTNER_HOST_INFO;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",RECEIVED_STRING_TOO_SHORT,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		goto tail;//如果收到的信息长度为0，则不需要进行后面的操作
	}

	int tempID=0,tempPasswd=0;
	char tempStr[MAC_SIZE];
	bzero(tempStr,MAC_SIZE);
	strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//提取主机请求信息中说明的ID串
	tempID=atoi(tempStr);
	bzero(tempStr,MAC_SIZE);
	sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
	strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//提取主机请求信息中说明的密码串
	tempPasswd=atoi(tempStr);
	bzero(tempStr,MAC_SIZE);
	sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;

	//接下来查找主机请求的主机是否存在
	int ind = 0,i=0;
	for(i=0;i<hosts_index;i++)
	{
		if(hosts_sockets[i].the_host->hostID == tempID && hosts_sockets[i].the_host->hostPasswd == tempPasswd )
		{//当用户ID和密码都正确的时候，说明找到了该主机
			ind = i;
			break;
		}
	}
	if(i == hosts_index)
	{//说明查找的主机不存在
		fl = REQUEST_PARTNER_HOST_INFO;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",REQUESTED_HOST_NOT_EXISTS,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		goto tail;
	}

	//进一步查看该主机是否是请求的自身
	int flag=0;
	while(strlen(sub_recv_buffer))
	{
		flag = 0;
		bzero(tempStr,MAC_SIZE);
		strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@"));
		MAC_addr_ptr self=&(hosts_sockets[ind].the_host->mac);
		while(self)
		{
			if(strcmp(self->mac,tempStr)==0)
			{
				flag = 1;
				break;
			}
			self=self->next;
		}
		sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
		if(flag)
			break;
	}


	if(flag)//说明主机请求的是自身
	{
		fl = REQUEST_PARTNER_HOST_INFO;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",REQUESTED_YOUR_SELF,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		goto tail;
	}

	//当请求成功时，生成包含被请求主机信息的返回字符串
	fl = REQUEST_PARTNER_HOST_INFO;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%s@%d@%s@\t%s",REQUEST_SUCCESS,hosts_sockets[ind].the_host->hostIP,\
			hosts_sockets[ind].the_host->hostPort,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	//在将该主机请求的伙伴主机的信息发送给该主机之后，通知伙伴主机有人请求连接，请其先给对方发送一条UDP信息，以备打洞
	char host_ip[IP_SIZE]={'\0'};
	int host_port=0;
	//获取发起请求的客户机的客户机公网IP
	while( inet_ntop(AF_INET,&client->client_addr.sin_addr,host_ip,IP_SIZE) == NULL )
	{
		perror("child_thread(Pthread_Para_ptr) : inet_ntop {int te_request(Pthread_Para_ptr client,char *recv_buffer)}");
	}
	host_port = client->client_addr.sin_port;//获取客户端的端口号
	tem_flag=1;
	
	fl = YOU_WARE_REQUESTED_TO_BE_CONNECTED_BY_THIS_HOST;
	memcpy(send_buffer_oth, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer_oth + INT_BYTE_LENGTH,"%d@%s@%d@\t%s",REQUEST_SUCCESS,host_ip,host_port,getLocalTime());
	sprintf(log_buffer_oth, "%d@%s", fl, send_buffer_oth + INT_BYTE_LENGTH);

	tail:

	//写日志文件
	outputLog(log_buffer);
	if(tem_flag)//如果找到了伙伴主机
		outputLog(log_buffer_oth);

	//向客户机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	send(sockfd, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);
	if(tem_flag)//如果找到了伙伴主机，向伙伴主机发送通知
	{
		send(hosts_sockets[ind].the_socket->socket_ser, send_buffer_oth, INT_BYTE_LENGTH + strlen(send_buffer_oth + INT_BYTE_LENGTH), 0);

		hosts_sockets[ind].index_of_partner = *self_index;
		hosts_sockets[*self_index].index_of_partner = ind;
	}

	return 0;
}
int te_disconnect(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{
	int clo = 1;
	int sockfd=client->socket_ser;
	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	MY_MACRO_INT fl;

	//char *sub_recv_buffer=recv_buffer + indexOf(recv_buffer,"@") + 1;
	char *sub_recv_buffer=recv_buffer;
	int len=strlen(sub_recv_buffer);//重新计算接收到的字节数
	if(!len)
	{
		fl = DISCONNECT;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",RECEIVED_STRING_TOO_SHORT,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		clo = 0;//接收到的信息有误，故不关闭链接
		goto tail;//如果收到的信息长度为0，则不需要进行后面的操作
	}

	int tempID=0,tempPasswd=0;
	char tempStr[MAC_SIZE];
	bzero(tempStr,MAC_SIZE);
	strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//提取主机请求信息中说明的ID串
	tempID=atoi(tempStr);
	bzero(tempStr,MAC_SIZE);
	sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
	strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@") );//提取主机请求信息中说明的密码串
	tempPasswd=atoi(tempStr);
	bzero(tempStr,MAC_SIZE);
	sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;

	//接下来查找主机请求的主机是否存在
	if(hosts_index<=0)
	{
		fl = DISCONNECT;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",REQUESTED_HOST_NOT_EXISTS,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		clo = 0;//接收到的信息有误，故不关闭链接
		goto tail;
	}
	int ind = 0,i=0;
	for(i=0;i<hosts_index;i++)
	{
		if(hosts_sockets[i].the_host->hostID == tempID && hosts_sockets[i].the_host->hostPasswd == tempPasswd )
		{//当用户ID和密码都正确的时候，说明找到了该主机
			ind = i;
			break;
		}
	}
	if(i == hosts_index)
	{//说明查找的主机不存在
		fl = DISCONNECT;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",REQUESTED_HOST_NOT_EXISTS,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		clo = 0;//接收到的信息有误，故不关闭链接
		goto tail;
	}

	//进一步查看该主机是否是请求的自身
	int flag=0;
	while(strlen(sub_recv_buffer))
	{
		flag = 0;
		bzero(tempStr,MAC_SIZE);
		strncpy(tempStr,sub_recv_buffer,indexOf(sub_recv_buffer,"@"));
		MAC_addr_ptr self=&(hosts_sockets[ind].the_host->mac);
		while(self)
		{
			if(strcmp(self->mac,tempStr)==0)
			{
				flag = 1;
				break;
			}
			self=self->next;
		}
		sub_recv_buffer += indexOf(sub_recv_buffer,"@") + 1;
		if(flag)
			break;
	}


	if(!flag)//说明主机请求的不是自身
	{
		fl = DISCONNECT;
		memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(send_buffer + INT_BYTE_LENGTH,"%d@\t%s",REQUESTED_HOST_NOT_EXISTS,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);
		clo = 0;//接收到的信息有误，故不关闭链接
		goto tail;
	}

	//如果当前有主机正与该主机相连接，则发消息告诉对方，我要下线了，发送消息之后，不需要等待回复。
	if( hosts_sockets[*self_index].index_of_partner >= 0)
	{
		char bye_buffer[BUFFER_SIZE];
		bzero(bye_buffer, BUFFER_SIZE);

		//生成返回字符串
		fl = I_WILL_EXIT;
		memcpy(bye_buffer, &fl, INT_BYTE_LENGTH);
		sprintf(bye_buffer + INT_BYTE_LENGTH,"%d@%s@\t%s",REQUEST_SUCCESS,recv_buffer,getLocalTime());
		sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

		//写日志文件
		outputLog(log_buffer);

		//发送自己的下线信息给伙伴主机
		if(hosts_sockets[*self_index].index_of_partner >= 0)
			send(hosts_sockets[hosts_sockets[*self_index].index_of_partner].the_socket->socket_ser, bye_buffer, INT_BYTE_LENGTH + strlen(bye_buffer + INT_BYTE_LENGTH), 0);
		bzero(bye_buffer, BUFFER_SIZE);

		//将记录双方链接的主机的下边的值置为初始值
		hosts_sockets[hosts_sockets[*self_index].index_of_partner].index_of_partner = -1;
		hosts_sockets[*self_index].index_of_partner = -1;
	}

	//生成返回字符串
	fl = DISCONNECT;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%s@\t%s",REQUEST_SUCCESS,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	tail:

	//写日志文件
	outputLog(log_buffer);

	//向客户机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	send(sockfd, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);
	
	if(clo)
		return ind;
	else
		return -1;
}
int te_keepalive(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{
	int sockfd=client->socket_ser;
	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	sleep(600);//睡眠600秒，即每10分钟探测一次，以保持连接

	//生成返回字符串
	MY_MACRO_INT fl = KEEP_ALIVE_SIGNAL;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%s@\t%s",REQUEST_SUCCESS,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	//写日志文件
	outputLog(log_buffer);

	//向客户机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	send(sockfd, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);

	return 0;
}
int reply_partner_is_ready(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{//该函数用于在UDP打洞时，当被链接的一方已经准备好被链接并向服务器发送准备好字样之后，调用该函数通知连接发起方

	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	//生成返回字符串
	MY_MACRO_INT fl = REQUEST_TO_SHARE_SCREEN;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%s@\t%s",PARTNER_HOST_ARE_ALSO_READY_TO_BE_CONNECTED,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	//写日志文件
	outputLog(log_buffer);

	//向伙伴机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	if(hosts_sockets[*self_index].index_of_partner >= 0)
		send(hosts_sockets[hosts_sockets[*self_index].index_of_partner].the_socket->socket_ser, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);

	return 0;
}
int partner_donot_allow_to_be_connected(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{//该函数用于处理，当对方拒绝了你的连接请求时，所应该做的工作
	char send_buffer[BUFFER_SIZE];
	bzero(send_buffer, BUFFER_SIZE);
	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	//生成返回字符串
	MY_MACRO_INT fl = REQUEST_TO_SHARE_SCREEN;
	memcpy(send_buffer, &fl, INT_BYTE_LENGTH);
	sprintf(send_buffer + INT_BYTE_LENGTH,"%d@%s@\t%s",YOUR_PARTNER_DONOT_ALLOW_TO_BE_CONNECTED,recv_buffer,getLocalTime());
	sprintf(log_buffer, "%d@%s", fl, send_buffer + INT_BYTE_LENGTH);

	//写日志文件
	outputLog(log_buffer);

	//向伙伴机返回信息
	bzero(recv_buffer - INT_BYTE_LENGTH, BUFFER_SIZE);
	if(hosts_sockets[*self_index].index_of_partner >= 0)
		send(hosts_sockets[hosts_sockets[*self_index].index_of_partner].the_socket->socket_ser, send_buffer, INT_BYTE_LENGTH + strlen(send_buffer + INT_BYTE_LENGTH), 0);

	//将他们双方保存的对方的下标信息删除，因为这次链接失败了。
	hosts_sockets[hosts_sockets[*self_index].index_of_partner].index_of_partner = -1;
	hosts_sockets[*self_index].index_of_partner = -1;

	return 0;
}
int picture_data_transfer(int *self_index,Pthread_Para_ptr client,char *recv_buffer)
{//该函数用于从服务器段进行数据的中转，将被请求主机的屏幕数据转发到伙伴主机

	char log_buffer[BUFFER_SIZE];
	bzero(log_buffer, BUFFER_SIZE);

	MY_MACRO_INT fl = 0;
	memcpy(&fl, recv_buffer, INT_BYTE_LENGTH);
	MY_MACRO_INT length_of_data = 0;
	memcpy(&length_of_data, recv_buffer + INT_BYTE_LENGTH, INT_BYTE_LENGTH);
/*
	//在这里给图片数据加上结束标识，以使客户端能正确接收数据
	if(INT_BYTE_LENGTH * 2 + length_of_data + END_FLAG_LENGTH <= BUFFER_SIZE)
	{//当前的缓冲区足够发送数据
		memcpy(recv_buffer + INT_BYTE_LENGTH * 2 + length_of_data, dataEnd, END_FLAG_LENGTH);
		//生成日志记录字符串
		sprintf(log_buffer,"%d@%d@And the length of picture data is %d@\t%s",THIS_IS_PICTURE_DATA,REQUEST_SUCCESS,length_of_data,getLocalTime());
		outputLog(log_buffer);
	}else
	{//当前的缓冲区不够发送数据
		//生成日志记录字符串
		sprintf(log_buffer,"%d@%d@And the length of picture data is %d,so the data is too long and was discarded!@\t%s",THIS_IS_PICTURE_DATA, CURRENT_DATA_IS_TOO_LONG, length_of_data, getLocalTime());
		outputLog(log_buffer);
		bzero(recv_buffer, BUFFER_SIZE);
		return -1;
	}
*/

	if(fl == THIS_IS_PICTURE_DATA)
	{//说明接收到了截屏图片的第一段数据
		sprintf(log_buffer,"%d@%d@And the length of picture data is %d@\t%s",THIS_IS_PICTURE_DATA,REQUEST_SUCCESS,length_of_data,getLocalTime());
		outputLog(log_buffer);
	}else
	{
		sprintf(log_buffer,"This is a data fragment!\t%s",getLocalTime());
		outputLog(log_buffer);
	}

	//将截图数据转发给伙伴主机
	if(hosts_sockets[*self_index].index_of_partner >= 0)
		send(hosts_sockets[hosts_sockets[*self_index].index_of_partner].the_socket->socket_ser, recv_buffer, INT_BYTE_LENGTH * 2 + length_of_data, 0);
	bzero(recv_buffer, BUFFER_SIZE);

	return 0;
}


