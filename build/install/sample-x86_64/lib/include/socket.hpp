#ifndef SOCKET_HPP
#define SOCKET_HPP

#ifdef __cplusplus
extern "C"{
#endif

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dlist.h"
#include "platform.h"

#define SOCKET_CFG_FILE "socket.cfg"
#define MAX_IP_LEN 20
#define MAX_DOMAIN_LEN 20
#define MAX_LISTEN_NUM 32	//监听连接队列最大长度
#define MAX_ACCEPT_NUM 1024	//最大连接数
#define MAX_BUFFER_SIZE 1500

typedef enum 
{
	UNIX_DOMAIN = AF_UNIX,//本地通信
	INET_IPV4 = AF_INET,//IPv4
	INET_IPV6 = AF_INET6,//IPv6
	PACKET = AF_PACKET,//链路层通信
}protocol_family_e;

typedef enum 
{
	IPV4_TCP = SOCK_STREAM,//创建TCP流套接字
	IPV4_UDP = SOCK_DGRAM,//创建UDP数据报套接字
	IPV4_RAW = SOCK_RAW,//创建原始套接字
}socket_type_e;

typedef enum
{
	CLOSE_RD = SHUT_RD,//将连接上的读通道关闭，未被读取的缓冲区数据将被丢弃，仍可发送数据
	CLOSE_WR = SHUT_WR,//将连接上的写通道关闭，未被发送的缓冲区数据将被丢弃，仍可接收数据
	CLOSE_RDWR = SHUT_RDWR,//读写通道都将被关闭，所有缓冲区数据都给丢弃
}close_type_e;

typedef enum
{
	OPT_KEEP_ALIVE = SO_KEEPALIVE,
	OPT_RCV_TIMEOUT = SO_RCVTIMEO,
	OPT_SND_TIMEOUT = SO_SNDTIMEO,
	OPT_BIND_DEVICE = SO_BINDTODEVICE,
	OPT_REUSEADDR = SO_REUSEADDR,
	OPT_GETTYPE = SO_TYPE,
	OPT_GETACCEPTCONN = SO_ACCEPTCONN,
	OPT_BROADCAST = SO_BROADCAST,
	OPT_SNDBUF = SO_SNDBUF,
	OPT_ECVBUF = SO_RCVBUF,
}sockopt_type_e;

typedef struct
{
	unsigned long ip_addr;
}ip_addr_t;//用来存储32位的IP地址,等同于in_addr

typedef struct
{
	unsigned short proto_family;
	unsigned short int port;
	ip_addr_t addr;
	unsigned char zero[8];
}sock_addr_t;//等同于sockaddr_in
	
typedef struct 
{
	sock_addr_t src_info;
    sock_addr_t dst_info;
	socket_type_e sock_type;
	unsigned int sock_fd;
	dlist list_client;
	char sendbuf[MAX_BUFFER_SIZE];
	char recvbuf[MAX_BUFFER_SIZE];
}socket_info_t, *p_socket_info_t;

class Socket
{
	public:
		Socket();
		~Socket();
		int sock_create(protocol_family_e proto, socket_type_e type);
		int sock_connect(char *ip, int port);
		int sock_bind();
		int sock_listen();
		int sock_accept();
		int sock_send();
		int sock_recv();
		int sock_sendto();
		int sock_recvfrom();
		int sock_close(close_type_e how);
		int sock_setopt(sockopt_type_e optname, const void *optval, int optlen);
		int sock_getopt(sockopt_type_e optname, void *optval, int *optlen);
		int sock_select();
	private:
		socket_info_t sock_info;
};




#ifdef __cplusplus
}
#endif

#endif
