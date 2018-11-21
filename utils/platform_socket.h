#ifndef __PLATFORM_SOCK_H___
#define __PLATFORM_SOCK_H___
	
#ifdef __MY_Win_32__ //windows 32
#include <winsock2.h>
#include <Mswsock.h>
#include <MSTCPIP.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

	typedef HANDLE Protable_HANDLE;
	typedef SOCKET Protable_SOCKET;
	# define Protable_INVALID_HANDLE  INVALID_SOCKET
	
	struct OLPlus 
	{
		OVERLAPPED ov; //µÚÒ»¸ö³ÉÔ±±ØÐëÊÇËü
		int opType; //²Ù×÷ÀàÐÍ
		size_t len;  //Êý¾Ý³¤¶È
		char* buf;  //Êý¾Ý»º³å
	};

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

	typedef int Protable_HANDLE;
	typedef Protable_HANDLE Protable_SOCKET;
	#define Protable_INVALID_HANDLE -1
	#define SOCKET_ERROR -1
	
	struct EpollUserData
	{
		void * pHandler;
		int opType;
	};
	
#endif	

#endif

