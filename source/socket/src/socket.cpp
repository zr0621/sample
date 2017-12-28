#include <unistd.h>
#include <time.h>

#include "socket.hpp"
#include "cfgfile.h"
#include "log.h"
#include "utils.h"
#include "platform.h"

using namespace std;

static json_kinfo_t json_sock_addr_src[] =
{
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U16, proto_family, NULL),
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U16, port, NULL),
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U32, addr, NULL),
	
	MAKE_END_INFO()
};

static json_kinfo_t json_sock_addr_dst[] =
{
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U16, proto_family, NULL),
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U16, port, NULL),
	MAKE_KEY_INFO(sock_addr_t, KEY_TYPE_U32, addr, NULL),
	
	MAKE_END_INFO()
};
	
static json_kinfo_t json_socket_info[] =
{
	MAKE_KEY_INFO(socket_info_t, KEY_TYPE_OBJECT, src_info, json_sock_addr_src),
	MAKE_KEY_INFO(socket_info_t, KEY_TYPE_OBJECT, dst_info, json_sock_addr_dst),
	MAKE_KEY_INFO(socket_info_t, KEY_TYPE_U32, sock_type, NULL),
	MAKE_KEY_INFO(socket_info_t, KEY_TYPE_U32, sock_fd, NULL),
	
	MAKE_END_INFO()
};

static int _save_cfg(const socket_info_t *sock_info)
{
    char fileName[128];
    sprintf(fileName, "%s/%s", get_config_path(), SOCKET_CFG_FILE);
	int ret = cfg_write(fileName, json_socket_info, (void *)sock_info);
	if(ret)
	{	
		LOG_ERROR("socket.cfg write fail \n");
		return ERR_WRITE_FAIL;
	}
	else
	{
		LOG_INFO("socket.cfg write success \n");
		return ERR_NO;
	}
}

static int _read_cfg(socket_info_t *sock_info)
{	
    char fileName[128];
	memset(sock_info, 0, sizeof(socket_info_t));
    sprintf(fileName, "%s/%s", get_config_path(), SOCKET_CFG_FILE);
	int ret = access(fileName,0);
	if(!ret)
	{
		LOG_INFO("socket cfg filename exsit\n");
	}
	else
	{
		LOG_WARN("socket cfg filename not exsit,rebuild cfg file\n");
		_save_cfg(sock_info);
		
	}
	ret = cfg_read(fileName, json_socket_info, sock_info);
	if(ret)
	{	
		LOG_ERROR("socket.cfg read fail.\n");
		return ERR_READ_FAIL;
	}
	else
	{
		LOG_INFO("sock_info->dst_info.addr = %d",sock_info->dst_info.addr);
		LOG_INFO("sock_info->dst_info.port = %d",sock_info->dst_info.port);
		LOG_INFO("sock_info->dst_info.proto_family = %d",sock_info->dst_info.proto_family);
		LOG_INFO("sock_info->src_info.addr = %d",sock_info->src_info.addr);
		LOG_INFO("sock_info->src_info.port = %d",sock_info->src_info.port);
		LOG_INFO("sock_info->src_info.proto_family = %d",sock_info->src_info.proto_family);
		LOG_INFO("sock_info->sock_type = %d",sock_info->sock_type);
		LOG_INFO("sock_info->sock_fd = %d",sock_info->sock_fd);
	}
	return ERR_NO;
}

Socket::Socket()
{
	LOG_DEBUG("socket object is being created");
	_read_cfg(&sock_info);
	dlist_init(&sock_info.list_client);
}

Socket::~Socket()
{
	LOG_DEBUG("socket object is being deleted");
	_save_cfg(&sock_info);
}

//创建套接字，server&client都会调用
int Socket::sock_create(protocol_family_e proto, socket_type_e type)
{
	sock_info.sock_fd = socket(proto, type, 0);
	sock_info.src_info.proto_family = proto;
	sock_info.dst_info.proto_family = proto;
	sock_info.sock_type = type;
	if(sock_info.sock_fd < 0)
	{
		perror("socket");
	}
	LOG_TRACE("sock_info.sock_fd = %d",sock_info.sock_fd);
	return sock_info.sock_fd;
}

//建立连接,一般client端调用
int Socket::sock_connect(char *ip, int port)
{
	int ret;
	sock_info.dst_info.port = htons(port);//htons是字节顺序转换函数
	//inet_aton函数将一个字符串转换成一个网络地址，并把该网络地址赋给第二个参数
	if(inet_aton(ip, (in_addr *)&sock_info.dst_info.addr) < 0)
	{
		perror("inet_aton");
	}
	ret = connect(sock_info.sock_fd, (sockaddr *)&sock_info.dst_info, sizeof(sock_addr_t));
	return ret;
}

//绑定端口和套接字,一般server端调用
int Socket::sock_bind()
{
	int ret;
	ret = bind(sock_info.sock_fd, (sockaddr *)&sock_info.dst_info, sizeof(sock_addr_t));
	if(ret < 0)
	{
		perror("bind");
	}
	return ret;
}

//在套接字上监听，一般server端调用，先create然后bind再listen最后accpet
int Socket::sock_listen()
{
	int ret = listen(sock_info.sock_fd, MAX_LISTEN_NUM);
	if(ret < 0)
	{
		perror("listen");
	}
	return ret;
}

//接受连接，一般server端调用，只适用于面向连接的如TCP连接的套接字
//建立连接得到新的fd后可以进行send和recv
int Socket::sock_accept()
{
	int addr_len = sizeof(sock_addr_t);
	int ret = accept(sock_info.sock_fd, (sockaddr *)&sock_info.dst_info,(socklen_t *)&addr_len);
	if(ret < 0)
	{
		perror("accept");
	}
	else
	{
		LOG_INFO("new client fd = %d",ret);
		dlist_add(&sock_info.list_client,&ret);
	}
	return ret;
}

//TCP 发送数据
int Socket::sock_send()
{
	int ret;
	ret = send((long)sock_info.list_client.head->data, sock_info.sendbuf, MAX_BUFFER_SIZE, 0);
	if(ret < 0)
	{
		perror("send");
	}
	return ret;//ssize_t，实际发送数据的字节数
}

//TCP 接收数据
int Socket::sock_recv()
{
	int ret;
	ret = recv((long)sock_info.list_client.head->data, sock_info.sendbuf, MAX_BUFFER_SIZE, 0);
	if(ret < 0)
	{
		perror("recv");
	}
	return ret;//ssize_t，实际接收数据的字节数
}

//UDP 发送数据
int Socket::sock_sendto()
{
	int ret;
	ret = sendto((long)sock_info.list_client.head->data, sock_info.sendbuf, MAX_BUFFER_SIZE, 0,
		 (sockaddr *)&sock_info.dst_info, sizeof(sock_addr_t));
	if(ret < 0)
	{
		perror("sendto");
	}
	return ret;//ssize_t，实际发送数据的字节数
}

//UDP 接收数据
int Socket::sock_recvfrom()
{
	int ret;
	int addr_len = sizeof(sock_addr_t);
	ret = recvfrom((long)sock_info.list_client.head->data, sock_info.sendbuf, MAX_BUFFER_SIZE, 0,
		 (sockaddr *)&sock_info.dst_info, (socklen_t *)&addr_len);
	if(ret < 0)
	{
		perror("recvfrom");
	}
	return ret;//ssize_t，实际接收数据的字节数
}

//关闭套接字
int Socket::sock_close(close_type_e how)
{
	int ret;
	ret = shutdown(sock_info.sock_fd, how);
	if(ret < 0)
	{
		perror("shutdown");
	}
	return ret;
}

//设置套接字属性
int Socket::sock_setopt(sockopt_type_e optname, const void *optval, int optlen)
{
	int ret;
	ret = setsockopt(sock_info.sock_fd, SOL_SOCKET, optname, optval, (socklen_t)optlen);
	if(ret < 0)
	{
		perror("setsockopt");
	}
	return ret;
}

//获取套接字属性
int Socket::sock_getopt(sockopt_type_e optname, void *optval, int *optlen)
{
	int ret;
	ret = getsockopt(sock_info.sock_fd, SOL_SOCKET, optname, optval, (socklen_t *)optlen);
	if(ret < 0)
	{
		perror("getsockopt");
	}
	return ret;
}

//多路复用
int Socket::sock_select()
{
	fd_set readfds;
	timeval timeout;
	int ret,i,fd;
	FD_ZERO(&readfds);
	for(i = 0; i < sock_info.list_client.count; i++)
	{
		fd = (long)dlist_get_data(&sock_info.list_client, i);
		FD_SET(fd,&readfds);
	}
	timeout.tv_sec = 60;
	timeout.tv_usec = 0;
	while(1)
	{
		ret = select(sock_info.list_client.count+1, &readfds, NULL, NULL, &timeout);
		switch(ret)
		{
			case 0:
				LOG_INFO("no data in 60 seconds.");
				break;
			case -1:
				perror("select");
				break;
			default:
				LOG_INFO("data is available now.");
		}
	}
	return 0;
}