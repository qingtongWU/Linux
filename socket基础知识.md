## 1.网络相关概念

### 1.1基本概念

~~~c++
1.套接口的概念
 —套接口，也叫“套接字”。是操作系统内核中的一个数据结构，它是网络中的节点进行相互通信的 门户。它是网络进程的ID。网络通信，归根到底还是进程间的通信（不同计算机上的进程间通信）。在网络中，每一个节点（计算机或路由）都有一个网络地址，也就是IP地址。两个进程通信时，首先要 确定各自所在的网络节点的网络地址。但是，网络地址只能确定进程所在的计算机，而一台计算机上很 可能同时运行着多个进程，所以仅凭网络地址还不能确定到底是和网络中的哪一个进程进行通信，因此 套接口中还需要包括其他的信息，也就是端口号（PORT）。在一台计算机中，一个端口号一次只能分配 给一个进程，也就是说，在一台计算机中，端口号和进程之间是一一对应关系。所以，使用端口号和网 络地址的组合可以唯一的确定整个网络中的一个网络进程。
例如，如网络中某一台计算机的IP为10. 92. 20. 160，操作系统分配给计算机中某一应用程序进程 的端口号为1500，则此时 10.92.20.160 1500就构成了一个套接口。
2.端口号的概念
在网络技术中，端口大致有两种意思：一是物理意义上的端口，如集线器、交换机、路由器等用于 连接其他网络设备的接口。二是指TCP/IP协议中的端口，端口号的范围从0~65535, —类是由互联网指 派名字和号码公司I CANN负责分配给一些常用的应用程序固定使用的“周知的端口”，其值一般为0~1023. 例如http的端口号是80, ftp为21, ssh为22, telnet为23等。还有一类是用户自己定义的，通常 是大于 1024 的整型值。
3.ip 地址的表示
通常用户在表达IP地址时采用的是点分十进制表示的数值（或者是为冒号分开的十进制Ipv6地 址），而在通常使用的socket编程中使用的则是二进制值，这就需要将这两个数值进行转换。 ipv4地址：32bit, 4字节，通常采用点分十进制记法。
例如对于：10000000 00001011 00000011 00011111 点分十进制表示为：128.11.3.31
4.一个进程所能打开的最大连接数
    1.)select
    单个进程所能打开的最大连接数有FD_SETSIZE宏定义，其大小是32个整数的大小（在32位的机器上，大小就是32*32，同理64位机器上FD_SETSIZE为32*64），当然我们可以对进行修改，然后重新编译内核，但是性能可能会受到影响，这需要进一步的测试。
	2.)epoll
    虽然连接数有上限，但是很大，1G内存的机器上可以打开10万左右的连接，2G内存的机器可以打开20万左右的连接,
    cat /proc/sys/fs/file-max  // linux下这个命令看

~~~

### 1.2数据存储优先顺序的转换

~~~c++
	计算机数据存储有两种字节优先顺序：高位字节优先（称为大端模式）和低位字节优先（称为小端 模式）。
内存的低地址存储数据的低字节，高地址存储数据的高字节的方式叫小端模式。
内存的高地址存储数据的低字节，低地址存储数据高字节的方式称为大端模式。
 
	例:对于内存中存放的数0x12345678来说
如果是采用大端模式存放的，则其真实的数是：0x12345678
如果是采用小端模式存放的，则其真实的数是：0x78563412 
如果某个系统所采用的字节序为主机字节序，则它可能是小端模式的，也可能是大端模式的。而端 口号和 IP 地址都是以网络字节序存储的，不是主机字节序，网络字节序都是大端模式。要把主机字节 序和网络字节序相互对应起来，需要对这两个字节存储优先顺序进行相互转化。
	这里用到四个函数：htons(), ntohs(), htonl()和ntohl().这四个地址分别实现网络字节序和主机 字节序的转化，这里的h代表host, n代表network， s代表short，1代表long。通常16位的IP端口 号用s代表，而IP地址用l来代表。

~~~

#### 1.3地址格式转化

~~~c++
	通常用户在表达地址时采用的是点分十进制表示的数值(或者是为冒号分开的十进制Ipv6地址)， 而在通常使用的socket编程中使用的则是32位的网络字节序的二进制值，这就需要将这两个数值进行 转换。这里在Ipv4中用到的函数有inet_aton()、inet_addr ()和inet_ntoa()，而IPV4和Ipv6兼容 的函数有 inet_pton()和 inet_ntop()。
IPv4 的函数原型：
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	int inet_aton(const char *straddr, struct in_addr *addrptr);
	char *inet_ntoa(struct in_addr inaddr);
	in_addr_t inet_addr(const char *straddr) ;	// in_addr_t 就是 unsigned int,代表 s_addr

函数inet_aton()：将点分十进制数的IP地址转换成为网络字节序的32位二进制数值。返回 值：成功，则返回1，不成功返回 0.
参数straddr：存放输入的点分十进制数IP地址字符串。
参数addrptr：传出参数，保存网络字节序的32位二进制数值。

函数inet_ntoa():将网络字节序的32位二进制数值转换为点分十进制的IP地址。
函数i net_addr()：功能与i ne t_aton相同，但是结果传递的方式不同。inet_addr ()若成功则返回 32 位二进制的网络字节序地址。
IPv4 和 IPv6 的函数原型:
#include <arpa/inet.h>
int inet_pton(int family, const char *src, void *dst);
const char *inet_ntop(int family, const void *src, char *dst, socklen_t len);
函数inet_pton跟inet_aton实现的功能类似，只是多了 family参数，该参数指定为AF_INET，表示是IPv4协议，如果是AF_INET6，表示IPv6协议。
函数 inet_ntop 跟 inet_ntoa 类似，其中 len 表示表示转换之后的长度(字符串的长度)。

~~~

### 1.4名字地址转化

~~~c++
	通常，人们在使用过程中都不愿意记忆冗长的IP地址，尤其到Ipv6时，地址长度多达128位，那 时就更加不可能一次性记忆那么长的IP地址了。因此，使用主机名或域名将会是很好的选择。主机名 与域名的区别：主机名通常在局域网里面使用，通过/etc/hosts文件，主机名可以解析到对应的ip； 域名通常是在 internet 上使用。
众所周知，百度的域名为：www. baidu. com，而这个域名其实对应了一个百度公司的IP地址，那么 百度公司的IP地址是多少呢？我们可以利用ping www. baidu. com来得到百度公司的ip地址，那么，系统是如何将www. baidu. com这个域名转化为IP地220.181.111.148的呢？
	在 linux 中，有一些函数可以实现主机名和地址的转化，最常见的有gethostbyname()、gethostbyaddr ()等，它们都可以实现IPv4和IPv6的地址和主机名之间的转化。其中gethostbyname() 是将主机名转化为IP地址，get hos tbyaddr()则是逆操作，是将IP地址转化为主机名。
函数原型：
#include <netdb.h>
struct hostent* gethostbyname(const char* hostname);
struct hostent* gethostbyaddr(const char* addr, size_t len, int family);
结构体：
struct hostent
{
	char *h_name;	/*正式主机名*/
	char **h_aliases;	/*主机别名*/
	int h_addrtype; /*主机 IP 地址类型 IPv4 为 AF_INET*/
	int h_length;	/*主机IP地址字节长度，对于IPv4是4字节，即32位*/
	char **h_addr_list; /*主机的 IP 地址列表*/
}
#define h_addr h_addr_list[0] /*保存的是 ip 地址*/
函数gethostbyname()：用于将域名(www.baidu.com)或主机名转换为IP地址。参数hostname指 向存放域名或主机名的字符串。
函数gethostbyaddr()：用于将IP地址转换为域名或主机名。参数addr是一个IP地址，此时这个 ip地址不是普通的字符串，而是要通过函数inet_aton()转换。len为IP地址的长度，AF_INET为4。 fam订y 可用 AF_INET： Ipv4 或 AF_INET6： Ipv6。必须在/etc/hosts 中有配置。

~~~

## 2.socket介绍概念

### 2.1socket概念

~~~c++
	Linux 中的网络编程是通过 socket 接口来进行的 socket 是一种特殊的 I/O 接口，它也是一种文件描 述符。它是一种常用的进程之间通信机制，通过它不仅能实现本地机器上的进程之间的通信，而且通过 网络能够在不同机器上的进程之间进行通信。
每一个socket都用一个半相关描述｛协议、本地地址、本地端口｝来表示；一个完整的套接字则用一 个相关描述｛协议、本地地址、本地端口、远程地址、远程端口｝来表示。socket也有一个类似于打开文 件的函数调用，该函数返回一个整型的socket描述符，随后的连接建立等操作都是通过这个socket描述 符来实现的.
~~~

### 2.2.socket类型

~~~c
1.流式 socket （SOCK_STREAM）	->用于 TCP 通信
流式套接字提供可靠的、面向连接的通信流；它使用TCP协议，从而保证了数据传输的正确性和顺序性。
2.数据报 socket （SOCK_DGRAM） ->用于 UDP 通信 
数据报套接字定义一种无连接的服务，数据通过相互独立的报文进行传输，是无序的，并且不保证是可靠、无差错的。它使用数据报协议UDP。
3.原始socket （S0CK_RAW）	->用于新的网络协议实现的测试等
原始套接字允许对底层协议如IP或ICMP进行直接访问，它功能强大但使用较为不便，主要用于一 些协议的开发。
~~~

### 2.3.socket信息数据结构

~~~c++
头文件<netinet/in.h>
sin_family:AF_INET ->IPv4协议  AF_INET6 ->IPv6 协议

struct sockaddr
{
unsigned short sa_family; 	/*地址族*/ 
char sa_data[14];	/*14 字节的协议地址，包含该 socket的 IP 地址和端口号。*/ 
};
struct sockaddr_in
{
short int sin_family; 		/*地址族*/ 
unsigned short int sin_port; 	/*端口号*/ 
struct in_addr sin_addr; 		/*IP 地址*/
unsigned char sin_zero[8]; 	/*填充 0 以保持与 struct sockaddr 同样大小*/ 
};
struct in_addr
{
in_addr_t s_addr;	 /* 32 位 IPv4 地址，网络字节序 */ 
};

~~~

## 3.socket编程

### 3.1 TCP通信步骤

~~~c++
服务端:
	socket
	bind
    listen
    while(1)
    {
		accept
		recv/recvfrom
		send/sendto
         close
    }
	close 
客户 端:
	socket	
	connect // 对应accept
	send/sendto
	recv/recvfrom
	close
~~~

#### 3.1.1服务端

~~~c++
1.	头文件包含：
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h> 
#include<string.h> 
#include <stdio.h> 
#include <stdlib.h>
2.	socket 函数：生成一个套接口描述符。
原型：int socket(int domain,int type,int protocol);
参数：
domain -> { AF_INET： Ipv4 网络协议 AF_INET6： IPv6 网络协议} 
type -> {tcp： SOCK_STREAM udp： SOCK_DGRAM} 
Protocol -> 指定socket所使用的传输协议编号。通常为0.
返回值：成功则返回套接口描述符，失败返回-1。
常用实例： int sfd = socket(AF_INET, SOCK_STREAM, 0);
if(sfd == -1){perror("socket");exit(-1);}
3.	bind函数：用来绑定一个端口号和IP地址，使套接口与指定的端口号和IP地址相关联。
原型： int bind(int sockfd,struct sockaddr * my_addr,socklen_t addrlen); 
参数：
sockfd -> 为前面socket的返回值。
my_addr -> 为结构体指针变量
对于不同的socket domain定义了一个通用的数据结构
struct sockaddr //此结构体不常用
{
unsigned short int sa_family; //调用 socket ()时的 domain 参数，即 AF_INET 值。 
charsa_data[14]; //最多使用 14 个字符长度
};
此sockaddr结构会因使用不同的socket domain而有不同结构定义，
例如使用AF_INET domain，其socketaddr结构定义便为
struct sockaddr_in //常用的结构体
{
unsigned short int sin_family; //即为 sa_familyAF_INET uint16_t sin_port; //为使用的 port 编号
struct in_addr sin_addr; //为 IP 地址 
unsigned char sin_zero[8]; //未使用
};
struct in_addr
{
uint32_t s_addr;
};
addrlenTsockaddr 的结构体长度。通常是计算sizeof(struct sockaddr); 
返回值：成功则返回 0，失败返回-1 
常用实例： 
struct sockaddr_in my_addr; //定义结构体变量 
memset(&my_addr, 0, sizeof(struct sockaddr)); //将结构体清空或 bzero(&my_addr, sizeof(struct sockaddr));
my_addr.sin_family = AF_INET; //表示采用Ipv4 网络协议 
my_addr.sin_port = htons(8888); //表示端口号为 8888，通常是大于 1024的一个值。
// htons()用来将参数指定的16位hostshort转换成网络字符顺序
my_addr.sin_addr.s_addr = inet_addr("192.168.0.101"); //inet_addr()用来将 IP 地址字符串转换成网络 所使用的二进制数字，如果为INADDR_ANY，这表示服务器自动填充本机IP地址。
if(bind(sfd, (struct sockaddr*)&my_str, sizeof(struct socketaddr)) == -1) 
{perror("bind");close(sfd);exit(-1);}
(注：通过将 my_addr.sin_port 置为 0，函数会自动为你选择一个未占用的端口来使用。同样，通过将 my_addr.sin_addr.s_addr置为INADDR_ANY，系统会自动填入本机IP地址。)
4.	listen函数：使服务器的这个端口和IP处于监听状态，等待网络中某一客户机的连接请求。如果客户 端有连接请求，端口就会接受这个连接。
原型： int listen(int sockfd,int backlog);
参数：sockfd为前面socket的返回值.即sfd
backlog指定同时能处理的最大连接要求，通常为10或者5。最大值可设至128
返回值：成功则返回0，失败返回-1
常用实例： if(listen(sfd, 10) == -1)
{perror("listen");close(sfd);exit(-1);}
5.	accept函数：接受远程计算机的连接请求，建立起与客户机之间的通信连接。服务器处于监听状态时, 如果某时刻获得客户机的连接请求，此时并不是立即处理这个请求，而是将这个请求放在等待队列中， 当系统空闲时再处理客户机的连接请求。当accept函数接受一个连接时，会返回一个新的socket标识符, 以后的数据传输和读取就要通过这个新的socket标识符来处理，原来参数中的socket也可以继续使用， 继续监听其它客户机的连接请求。(也就是说，类似于移动营业厅，如果有客户打电话给 10086，此时 服务器就会请求连接，处理一些事务之后，就通知一个话务员接听客户的电话，也就是说，后面的所有 操作，此时已经于服务器没有关系，而是话务员跟客户的交流。对应过来，客户请求连接我们的服务器， 我们服务器先做了一些绑定和监听等等操作之后，如果允许连接，则调用accept函数产生一个新的套接字，然后用这个新的套接字跟我们的客户进行收发数据。也就是说，服务器跟一个客户端连接成功，会 有两个套接字。)
原型： int accept(int sockfd,struct sockaddr * addr,socklen_t* addrlen);
参数：
sockfd -> 为前面socket的返回值.即sfd
addr -> 为结构体指针变量，和bind的结构体是同种类型的，系统会把远程主机的信息(远程 主机的地址和端口号信息)保存到这个指针所指的结构体中。
addrlen -> 表示结构体的长度，为整型指针
返回值：成功则返回新的socket处理代码new_fd，失败返回-1
常用实例： struct sockaddr_in clientaddr;
		memset(&clientaddr, 0, sizeof(struct sockaddr));
		int addrlen = sizeof(struct sockaddr);
		int new_fd = accept(sfd, (struct sockaddr*)&clientaddr, &addrlen);
		if(new_fd == -1) 
         {perror("accept");close(sfd);exit(-1);} 
		printf("%s %d success connect\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
6.	recv函数：用新的套接字来接收远端主机传来的数据，并把数据存到由参数buf指向的内存空间
原型： int recv(int socknfd,void *buf,int len,unsigned int flags);
参数：
socknfd -> 为前面accept的返回值.即new_fd，也就是新的套接字。
buf->表示缓冲区
len->表示缓冲区的长度
flags->通常为0
返回值：成功则返回实际接收到的字符数，可能会少于你所指定的接收长度。失败返回-1 
常用实例： charbuf[512] = {0};
if(recv(new_fd, buf, sizeof(buf), 0) == -1) {perror("recv");close(new_fd);close(sfd);exit(-1);} puts(buf);
7.	send 函数：用新的套接字发送数据给指定的远端主机
原型： int send(int socknfd,const void * msg,int len,unsigned int flags);
参数：
socknfd -> 为前面accept的返回值.即new_fd
msg -> 一般为常量字符串
len -> 表示长度
flags -> 通常为0 
返回值：成功则返回实际传送出去的字符数，可能会少于你所指定的发送长度。失败返回-1 
常用实例： if(send(new_fd, "hello", 6, 0) == -1)
{perror("send");close(new_fd);close(sfd);exit(-1);}
8.close函数：当使用完文件后若已不再需要则可使用close()关闭该文件，并且close()会让数据写回磁盘, 并释放该文件所占用的资源
原型： int close(int fd);
参数：fd -> 为前面的sfd,new_fd
返回值：若文件顺利关闭则返回 0，发生错误时返回-1
常用实例： close(new_fd); close(sfd);

~~~

#### 3.1.2 客户端

~~~c
connect 函数：用来请求连接远程服务器，将参数 sockfd 的 socket 连至参数 serv_addr 指定的服务器 IP 和端口号上去。
原型： int connect (int sockfd,struct sockaddr * serv_addr,int addrlen);
参数：
sockfd -> 为前面socket的返回值，即sfd
serv_addr -> 为结构体指针变量，存储着远程服务器的IP与端口号信息。 addrlenT表示结构体变量的长度
返回值：成功则返回 0，失败返回-1
常用实例：struct sockaddr_in seraddr;//请求连接服务器
memset(&seraddr, 0, sizeof(struct sockaddr));
seraddr.sin_family = AF_INET;
seraddr.sin_port=htons(8888); //服务器的端口号 
seraddr.sin_addr.s_addr= inet_addr("192.168.0.101"); //服务器的ip
if(connect(sfd, (struct sockaddr*)&seraddr, sizeof(struct sockaddr)) == -1) 
{perror("connect");close(sfd);exit(-1);}

~~~

### 3.2  UDP通信步骤

~~~c++
服务端：
    socket
    bind
    recvfrom
    sendto
    close 
客户端：
    socket
    sendto
    recvfrom
    close
sendto()函数原型：
int sendto(int sockfd, const void *msg,int len,unsigned int flags,const struct sockaddr *to, socklen_t tolen);
该函数比send()函数多了两个参数，to表示目地机的IP地址和端口号信息，而tolen常常被赋值为 sizeof(structsockaddr)。 sendto 函数也返回实际发送的数据字节长度或在出现发送错误时返回-1。
recvfrom ()函数原型：
int recvfrom(int sockfd,void *buf,int len,unsigned int flags,struct sockaddr *from,int *fromlen);
from是一个struct sockaddr类型的变量,该变量保存连接机的IP地址及端口号。fromlen常置为sizeof (struct sockaddr)c当recvfrom()返回时，fromlen包含实际存入from中的数据字节数。Recvfrom()函数返 回接收到的字节数或当出现错误时返回-1，并置相应的 errno。

~~~

### 3.3  一些函数

~~~c++
1.fcntl 函数简介
fcntl 函数可以改变已打开的文件描述符性质
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
定义函数 int fcntl(int fd, int cmd);
int fcntl(int fd, int cmd, int arg);
int fcntl(int fd, int cmd, struct flock *lock);
fcntl的返回值与命令有关。如果出错，所有命令都返回-1,如果成功则返回某个其他值。
fcntl()针对(文件)描述符提供控制，参数fd是被参数cmd操作(如下面的描述)的描述符。
针对cmd的值，fcntl能够接受第三个参数int arg
参数fd代表欲设置的文件描述符。
参数cmd代表打算操作的指令。
fcntl函数有5种功能：
1.	复制一个现有的描述符(cmd=F_DUPFD)
2.	获得/设置文件描述符标记(cmd=F_GETFD或F_SETFD)
3.	获得/设置文件状态标记(cmd=F_GETFL或F_SETFL)
4.	获得/设置异步I/O所有权(cmd=F_GETOWN或F_SETOWN)
5.	获得 / 设置记录锁(cmd=F_GETLK,F_SETLK 或 F_SETLKW)
cmd有如下选项设置
F_DUPFD用来查找大于或等于参数arg的最小且仍未使用的文件描述符，并且复制参数fd的文件 描述符。执行成功则返回新复制的文件描述符。新描述符与fd共享同一文件表项，但是新描述符有它自 己的一套文件描述符标志,其中FD_CLOEXEC文件描述符标志被清除。请参考dup2()。
F_GETFD取得close-on-exec旗标(在计算机科学中,旗标被理解为一个单个整型值，结合一对 被称为P和V的函数。)。若此旗标的FD_CLOEXEC位为0代表在调用exec()相关函数时文件将不 会关闭。
F_SETFD设置close-on-exec旗标。该旗标以参数arg的FD_CLOEXEC位决定。
F_GETFL取得文件描述符状态旗标，此旗标为open ()的参数flags。
F_SETFL 设置文件描述符状态旗标，参数 arg 为新旗标，但只允许 O_APPEND、O_NONBLOCK 和 O_ASYNC 位的改变，其他位的改变将不受影响。
F_GETLK 取得文件锁定的状态。
F_SETLK设置文件锁定的状态。此时flcok结构的l_type值必须是F_RDLCK、F_WRLCK或 F_UNLCK。如果无法建立锁定，则返回-1,错误代码为EACCES或EAGAIN。
F_SETLKWF_SETLK 作用相同，但是无法建立锁定时，此调用会一直等到锁定动作成功为止。若 在等待锁定的过程中被信号中断时，会立即返回-1,错误代码为EINTR。

2.Socketpair 函数简介
int socketpair(int domain, int type, int protocol, int sv[2]); 前面 3 个参数参照 socket， domain 变为 AF_LOCAL, sv[2], fd[2]
3.Sendmsg 函数简介
ssize_t sendmsg (int s, const struct msghdr *msg, int flags);
sendmsg系统调用用于发送消息到另一个套接字
函数参数描述如下：
要在其上发送消息的套接口 s
信息头结构指针msg,这会控制函数调用的功能
可选的标记位参数flags。这与send或是sendto函数调用的标记参数相同。
函数的返回值为实际发送的字节数。否则，返回-1表明发生了错误，而errno表明错误原因。
理解 struct msghdr
其结构定义如下：
struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	size_t	msg_iovlen;
	void	*msg_control;
	size_t	msg_controllen;
	int	msg_flags;
};
结构成员可以分为四组。他们是：
套接口地址成员 msg_name与 msg_namelen。
I/O 向量引用 msg_iov 与 msg_iovlen。
附属数据缓冲区成员msg_control与msg_controllen。
接收信息标记位msg_flags。

成员 msg_name 与 msg_namelen
这些成员只有当我们的套接口是一个数据报套接口时才需要。msg_name成员指向我们要发送或是 接收信息的套接口地址。成员msg_namelen指明了这个套接口地址的长度。
当调用recvmsg时，msg_name会指向一个将要接收的地址的接收区域。当调用sendmsg时，这 会指向一个数据报将要发送到的目的地址。
注意，msg_name定义为一个(void *)数据类型。我们并不需要将我们的套接口地址转换为(struct sockaddr*)。
成员 msg_iov 与 msg_iovlen
这些成员指定了我们的I/O向量数组的位置以及他包含多少项。msg_iov成员指向一个struct iovec 数组。我们将会回忆起I/O向量指向我们的缓冲区。成员msg_iov指明了在我们的I/O向量数组中 有多少元素。
成员 msg_control 与 msg_controllen
这些成员指向了我们附属数据缓冲区并且表明了缓冲区大小。msg_control指向附属数据缓冲区， 而msg_controllen指明了缓冲区大小。
这个通过man cmsg 了解,或者通过下面的cmsg讲解熟悉
4.	Recvmsg
int recvmsg(int s, struct msghdr *msg, unsigned int flags);
函数参数如下：
要在其上接收信息的套接口 s
信息头结构指针msg，这会控制函数调用的操作。
可选标记位参数flags。这与recv或是recvfrom函数调用的标记参数相同。 这个函数的返回值为实际接收的字节数。否则，返回-1表明发生了错误，而errno表明错误原因。
5.	Writev
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
一次写入多个buf内容
struct iovec {
	void *iov_base; /* Starting address */ 
    size_t iov_len; /* Number of bytes to transfer*/
};
6.	Readv
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
一次读取多个buf内容
7.	Cmsg用来设定*msg_control指针
size_t CMSG_LEN(size_t length);
返回结构体cmsghdr的大小，length填入的是cmsg_data[]的大小，填入的是描述符fd，所以是 sizeof(int)
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);
struct cmsghdr {
	socklen_t cmsg_len; /* data byte count, including header */
	int	cmsg_level; /* originating protocol */
	int	cmsg_type; /* protocol-specific type */
	/* followed by unsigned char cmsg_data[]; */
};
CMSG_DATA 返回 cmsg_data 的起始地址，也就是通过它放入文件描述符
*(int*)CMSG_DATA(cmsg)=fd;
cmsg->cmsg_level = SOL_SOCKET;
cmsg->cmsg_type = SCM_RIGHTS;

~~~

## 4.select和epoll

### 4.1 select原理

~~~c++
调用select时，会发生以下事情：
1.	从用户空间拷贝fd_set到内核空间；
2.	注册回调函数__pollwait；
3.	遍历所有fd，对全部指定设备做一次poll (这里的poll是一个文件操作，它有两个参数，一个是文件fd本身，一个是当设备尚未就绪时调用的回调函数__pollwait,这个函数把设备自己特有的等 待队列传给内核，让内核把当前的进程挂载到其中)；
4.	当设备就绪时，设备就会唤醒在自己特有等待队列中的【所有】节点，于是当前进程就获取到了完成的信号。poll文件操作返回的是一组标准的掩码，其中的各个位指示当前的不同的就绪状态(全 0为没有任何事件触发)，根据mask可对fd_set赋值；
5.	如果所有设备返回的掩码都没有显示任何的事件触发，就去掉回调函数的函数指针，进入有限时的睡眠状态，再恢复和不断做poll，再作有限时的睡眠，直到其中一个设备有事件触发为止。
6.	只要有事件触发，系统调用返回，将fd_set从内核空间拷贝到用户空间，回到用户态，用户就可以对相关的fd作进一步的读或者写操作了。
~~~

### 4.2 epoll原理

~~~c++
调用 epoll_create 时，做了以下事情：
1. 内核帮我们在 epoll 文件系统里建了个 file 结点；
2. 在内核 cache 里建了个红黑树用于存储以后 epoll_ctl 传来的 socket；
3. 建立一个 list 链表，用于存储准备就绪的事件。
调用 epoll_ctl 时，做了以下事情：
1. 把 socket 放到 epoll 文件系统里 file 对象对应的红黑树上； 
2. 给内核中断处理程序注册一个回调函数，告诉内核，如果这个句柄的中断到了，就把它放到准备就绪 list 链表里。

调用 epoll_wait 时，做了以下事情： 
    观察 list 链表里有没有数据。有数据就返回，没有数据就 sleep，等到 timeout 时间到后即使链表没 数据也返回。而且，通常情况下即使我们要监控百万计的句柄，大多一次也只返回很少量的准备就绪句柄而已，所以，epoll_wait 仅需要从内核态 copy 少量的句柄到用户态而已。

总结如下：
一颗红黑树，一张准备就绪句柄链表，少量的内核cache，解决了大并发下的socket处理 问题。
执行 epoll_create 时，创建了红黑树和就绪链表； 执行epoll_ctl时，如果增加socket句柄，则检查在红黑树中是否存在，存在立即返回， 不存在则添加到树干上，然后向内核注册回调函数，用于当中断事件来临时向准备就绪链
表中插入数据;
执行 epoll_wait时立刻返回准备就绪链表里的数据即可。
~~~

### 4.3 优缺点分析

~~~c++
1. select缺点
口诀： 加入集合找就绪，1024有修改，内核用户来回跑；
1. 每次调用将监听的文件描述符加入集合中，从用户空间拷贝到内核空间；
2. 每次都要遍历整个位图，找到就绪的文件描述符，每次都要将所用的文件描述符从内核拷贝到用户空间；
3. 位图有1024位，每次只能监听1024个，数量有限；
4. 每次调用都会修改集合中的数据
    
2. epoll优点
口诀：红黑就绪有链表
1.把要监听的文件描述符加入红黑树(这个时候才有内存拷贝)，若不主动删除，则一直保持(一次注册，永久有效)；
2.文件描述符就绪的时候，会自己调用自己的回调函数，把自己加入双向链表中，不用遍历红黑树；
3.当双向链表中有数据的时候，epoll_wait从双向链表(表空则sleep())中取就绪的文件描述符，到用户空间的结构体数组中(根据业务并发量动态调整) 
    
总结：当然，以上的优缺点仅仅是特定场景下的情况：高并发，且任一时间只有少数socket是活跃的。如果没有大量的idle -connection或者dead-connection，epoll的效率并不会比select/poll高很多，但是当遇 到大量的idle- connection，就会发现epoll的效率大大高于select/poll

~~~

### 4.4 select的基本概念

~~~c
使用一个机制把所有可能导致阻塞的资源(本质是文件)管理起来，将等待状态从等待某个资源改成等待整个集合。
//select的原理
//找fd_set结构体声明
$ gcc -E add.c -o add.i
$ grep -nE fd_set add.i
看第一个所在行数，比如在1195
$ vim +1195 add.i
//fd_set结构体
typedef struct{
    _fd_mask _fds_bits[1024/(8*(int)sizeof(_fd_mask))];//一个数组
}fd_set;
数组
    元素类型  _fa_mask
	大小     1024/(8*(sizeof(_fd_mask))
总空间大小1024/8bytes=1024位 			
    位图：每一位代表文件描述符，FD_ZERO将每一位置为0，FD_SET将文件描述符对应的那一位置1
执行select时，将fd_set拷贝到内核态，轮询0~nfds-1
    轮询缺点：						改进：epoll
            1.时间复杂度高O(n)    		1.红黑树，2.高并发量
            2.并发量优先					3.通知机制，数据来了才检查
                 

//介绍
#include<sys/select.h>
int select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,
           struct timeval *timeout)
    //所监听的文件描述符的最大值+1,写，读，异常和超时导致的阻塞
    //返回值是所有集合中已经就绪的文件描述符的总数
void FD_CLR(int fd,fd_set *set)
int FD_ISSET(int fd,fd_set *set)//遍历集合，检查是否处于就绪
int FD_SET(int fd,fd_set *set)//创建集合
void FD_ZERO(fd_set *set)//初始化集合
//流程
1.创建集合，定义一个fd_set类型的变量;
2.初始化集合，FD_ZERO(&set);
3.把待监听的文件描述符注册到集合中，FD_SET;
4.使用select,会使进程处于阻塞状态；
5.若监听的fd中，有至少一个处于就绪，select不在阻塞进程；
6.遍历所有的fd,使用fd_ISSET;
7.若就绪，就获取资源。

//select超时机制
struct timeval{
	long tv_sec;// 秒
    long tv_usec;// 微秒
};

struct timeval timeout;
timtout.tv_sec=2;
timeout.tv_usec=500000;
int sret=select(fdr+1,&rdset,NULL,NULL,&timeout);
if(sret==0){
    printf("time out!\n");
    continue;
}
~~~

### 4.5 select写阻塞问题

~~~c++
//原理
写入内核管道缓冲区---暂存区---读缓冲区
内核管道写缓冲区
    空的时候才就绪
    缓冲区满的时候，再写入会阻塞

//write.c
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdw=open(argv[1],O_WRONLY);
    int cnt=0;
    while(1){
        printf("cnt=%d\n",cnt++);
        write(fdw,"I miss you!",11)};
	}
return 0;
}

//read.c 只打开不读，就会阻塞啦
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdr=open(argv[1],O_RDONLY);
    while(1);//没有读操作
return 0;
}
//用select管理写阻塞
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdr=open(argv[1],O_RDWR);//以非阻塞的方式打开管道的一端
    int fdw=open(argv[1],O_RDWR);
    int count=0;
    fd_set rdset;
    fd_set wrset;//定义读写变量
    char buf[16384]={0};
    while(1){
        FD_ZERO(&rdset);
        FD_ZERO(&wrset);//初始化集合
        FD_SET(fdr,&rdset);
        FD_SET(fdw,&wrset);//待监听的文件描述符注册到集合
        select(fdw+1,&rdset,&wrset,NULL,NULL);//异常，时间都不要
        //读
        if(FD_ISSET(fdr,&rdset)){
           printf("count=%d,read\n",count++);
           read(fdr,buf,1024);
          // sleep(1);//进程阻塞一秒
           }   
        //写
           if(FD_ISSET(fdw,&wrset)){
              printf("count=%d,write\n",count++);
              write(fdw,buf,4096);//写缓冲区是读的4倍，>4096的时候会处于永久阻塞的状态
             // sleep(1);                                                 
            }   
     }   
    return 0;
}
~~~



### 4.6  epoll的几个函数

~~~c++
epoll操作过程需要三个接口，分别如下：
#include <sys/epoll.h>
int epoll_create(int size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
(1) int epoll_create(int size);
创建一个epoll的实例，并返回指向该实例的句柄(也是文件描述符)，size参数已经没有意义，但 是必须大于0, 一般填1即可。需要注意的是，当创建好epoll句柄后，它就是会占用一个fd值，在linux 下如果查看/proc/进程id/fd/,是能够看到这个fd的，所以在使用完epoll后，必须调用close()关闭，否 则可能导致fd被耗尽。
(2)int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll的事件注册函数，它不同于select()是在监听事件时告诉内核要监听什么类型的事件，而是在这 里先注册要监听的事件类型。
第一个参数是epoll_create()的返回值， 第二个参数表示动作，用三个宏来表示：
EPOLL_CTL_ADD：注册新的 fd 到 epfd 中；
EP0LL_CTL_M0D：修改己经注册的fd的监听事件；
EP0LL_CTL_DEL：从 epfd 中删除一个 fd；
第三个参数是需要监听的 fd
第四个参数是告诉内核需要监听什么事，struct epoll_event结构如下:
struct epoll_event {
	__uint32_t events; /* Epoll events */
	epoll_data_t data; /* User data variable */
};
typedef union epoll_data {
	void	*ptr;
	int	fd;
	uint32_t	u32;
	uint64_t	u64;
} epoll_data_t;	

events 可以是以下几个宏的集合：
EPOLLIN :表示对应的文件描述符可以读(包括对端SOCKET正常关闭)；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读(这里应该表示有带外数据到来)；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说 的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话， 需要再次把这个socket加入到EPOLL队列里

epoll对文件描述符的操作有两种模式：(重要)
LT (level trigger)和ET (edge trigger)。LT模式是默认模 式，LT模式与ET模式的区别如下：
LT模式：Level_trigger(水平触发)：当被监控的文件描述符上有可读写事件发生时，epoll_wait()会 通知处理程序去读写。如果这次没有把数据一次性全部读写完(如读写缓冲区太小)，那么下次调用 epoll_wait()时，它还会通知你在上次没读写完的文件描述符上继续读写。
ET模式：Edge_trigger(边缘触发)：当被监控的文件描述符上有可读写事件发生时，epoll_wait()会 通知处理程序去读写。如果这次没有把数据全部读写完(如读写缓冲区太小)，那么下次调用 epoll_wait() 时，它不会通知你，也就是它只会通知你一次，直到该文件描述符上出现第二次可读写事件才会通知你.
    ET模式在很大程度上减少了 epoll事件被重复触发的次数，因此效率要比LT模式高。epoll工作在ET 模式的时候，必须使用非阻塞套接口，以避免由于一个文件句柄的阻塞读/阻塞写操作把处理多个文 件描述符的任务饿死。
(3)	int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
等待事件的产生，类似于select()调用。参数events用来从内核得到事件的集合，maxevents告之内 核这个 events 有多大。参数 timeout 是超时时间(毫秒，0 会立即返回， -1 将不确定，也有说法说是永久 阻塞)。该函数返回需要处理的事件数目，如返回 0 表示已超时。

~~~

### 4.7 epoll的非阻塞接口

~~~c++
1.recv()第四个参数设为 MSG_DONTWAIT，变为非阻塞的，没有接收数据会返回-1；
   if(-1 == ret){
       break;
   }

2.文件描述符属性的修改
void setNonBlock(int fd){
    int status = 0;
    //使用fcntl函数获取文件描述符的状态信息
    status = fcntl(fd, F_GETFL);
    //把文件描述符的状态设置为非阻塞的
    status |= O_NONBLOCK;
    //对描述符设置为非阻塞的
    fcntl(fd, F_SETFL, status);
}
// fcntl放在accept后面。

大多数阻塞函数，它的阻塞性和它的参数，也就是文件描述符有关，如果文件描述符数阻塞性的，则函数也是阻塞性的，否则反之。
~~~

### 4.8 设置缓冲区大小

~~~c++
getsockopt() -- 获取缓冲区大小
setsockopt() -- 设置缓冲区大小

SOL_SOCKET
SO_RCVBUF --接收缓冲区大小(一般64KB)
SO_SNDBUF -- 发送缓冲区大小
SO_RECVLOWAT --接收缓冲区下限(大于下限的时候，打印数据，网络差的时候)
SO_SNDWAIT --发送缓冲区下限
    
//示例代码
//获取缓冲区大小
socklen_t bufSize = 0;
socklen_t len = sizeof(bufSize);
getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, &len);
printf("bufSize = %d\n", bufSize);

//设置缓冲大小
bufSize = 4096;
setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, len);
getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, &len);
printf("bufSize = %d\n", bufSize);

//设置接收缓冲下限
int lowat = 10; 
socklen_t len = sizeof(lowat);
setsockopt(newFd, SOL_SOCKET, SO_RCVLOWAT, &lowat, len);
~~~

## 5.I/O模型

~~~c++
1.阻塞IO（blocking I/O）
A拿着一支鱼竿在河边钓鱼，并且一直在鱼竿前等，在等的时候不做其他的事情，十分专心。只有
鱼上钩的时，才结束掉等的动作，把鱼钓上来。
    
2.非阻塞IO（noblocking I/O）
B也在河边钓鱼，但是B不想将自己的所有时间都花费在钓鱼上，在等鱼上钩这个时间段中，B也在
做其他的事情（一会看看书，一会读读报纸，一会又去看其他人的钓鱼等），但B在做这些事情的时
候，每隔一个固定的时间检查鱼是否上钩。一旦检查到有鱼上钩，就停下手中的事情，把鱼钓上来。
    
3.信号驱动IO（signal blocking I/O）
C也在河边钓鱼，但与A、B不同的是，C比较聪明，他给鱼竿上挂一个铃铛，当有鱼上钩的时候，
这个铃铛就会被碰响，C就会将鱼钓上来。
    
4.IO多路复用（I/O multiplexing）
D同样也在河边钓鱼，但是D生活水平比较好，D拿了很多的鱼竿，一次性有很多鱼竿在等，（对于
select）而且每个鱼杆上都挂了铃铛，当铃铛响了的时候，D轮询的查找是哪个鱼杆上的铃铛响的。（对于epoll）给每个鱼杆都设置了不同的响铃方式，这样不用轮询，直接到相应鱼杆把鱼拉上来即可
    
5.异步IO（asynchronous I/O）
E也想钓鱼，但E有事情，于是他雇来了F，让F帮他钓鱼，一旦有鱼上钩，F就钓鱼上来，然后打电
话通知E钓鱼完成。
~~~

## 6.TCP/UDP概念

### 6.1TCP

#### 6.1.1TCP特点

~~~c++
特点：面向连接，全双工，速度慢，可靠数据传输
    如何保证可靠？
    	1.采样随机序列号 --- 防止黑客获取，保证系统稳定性
    	2.确认重传机制
    	3.滑动窗口协议
~~~

#### 6.1.2三次握手

~~~c++
标志位：ACK -- 协议规定，数据传输都要加ACK
        SYN -- =1,表示发起一个连接请求
    	FIN -- =1，表示报文发完
//三次握手 A <--->B   发送的有连接请求和数据包
    1.SYN=1,seq=x   A-->B
    2.SYN=1,ACK=1,seq=y,ack=x+1 
    3.ACK=1,seq=x+1,ack=y+1
    ACK--不占用序列号，表明确认号是合法的
    ack=x+1,ack是确认号，
    	1.希望下次发送包的序列号
    	2.SYN占用了一个序列号
三次握手为应用程序提供可靠的通信连接，适用于一次传输大批数据。
 //三次握手中丢失的探讨：
A先向B发送连接请求：
第一次握手丢失，会触发A的超时重传机制；
第二次握手丢失，A和B都以为对方没有收到自己的连接请求，会触发A和B的超时重传机制；
第三次握手丢失，1.如果双方都没有发送数据，B没收到A的确认信息，会重发一下；
    		  2.A要发送数据，而数据包的序列号就是x+1，B收到以后，会认为连接建立完成；
    		  3.B要发送给A数据，由于B没收到第二次握手确认，会一直重传第二次握手消息。
    
 //为什么要三次，两次行不行？
TCP是面向连接的，双方在发生数据之前要建立连接，建立连接包括参数的设置、内存空间的分配，收发双方参数的协商等，前两次握手建立连接，第三次握手确认连接可靠；
如果只有两次握手的话，客户端发送一个连接请求SYN报文段，而这个报文段由于网络原因，服务端暂时没有收到，而客户端认为丢包，又重发一个SYN，而此时建立了连接，等连接释放以后，客户端之前发的SYN有被服务器收到了，服务器就会认为客户端有连接请求，会继续向客户端发送数据，而此刻客户端已经关闭了，这样就会造成资源的浪费。
    
//三次握手过程，内核干了啥？
B收到A的连接请求，放入半连接队列(最大128，取系统配置和listen()中最小的，listen()是指定的)
B发出连接请求，当收到第三次连接的时候，内核会将其从半连接队列加入全连接队列，内核空闲的时候，accept()从连接队列中取出第一个。
~~~

#### 6.1.3四次挥手

~~~c
产生原因：通信双方结束通信，为了关闭各自的收发缓冲区，引入了四次挥手。
A <--->B,A先向B发送断开连接的请求
四次挥手：
    1.FIN=1,seq=u     --- FIN也占用一个序列号
    2.ACK=1,seq=v,ack=u+1；
    
    3.FIN=1,ACK=1,seq=w,ack=u+1
    4.ACK=1,seq=u+1,ack=w+1     --- B立即关闭数据通道
 A经过一定时间2MSL(Linux中60s)等待，A的连接才关闭
    因为60s的时间，能够让网络中延迟的数据包到达跳数并消失
    1.对第四次挥手的确认，保证通信双方的四次挥手能够顺利完成，正常关闭TCP连接，(如果客户端发送完第四次挥手后直接关闭连接，此时第四次挥手丢失，服务端由于没收到确认信息，会重传第三次挥手；那么服务器就不能按正常步骤进入close状态。这样就会耗费服务器的资源）
    2.保证B在2,3两次挥手的时候发送的旧的报文在网络中消失，这样在下一次连接的时候就不会出现旧连接的报文段啦。
 若改为3次，将第2,3两次合并，B在第3次没有数据传输的情况下。
~~~

### 6.2UDP特点

~~~c++
1.没有各种连接：在传输数据前不需要建立连接；
2.不重新排序：对到达顺序混乱的数据包不进行重新排序；
3.没有确认：发送的数据包无需等待对方确认，可以用udp协议随时发送数据，但无法保证数据成功被对方接收；
 丢包问题：
  	操作系统的UDP 接收流程如下：收到一个UDP 包后，验证没有错误后，放入一个包队列中，队列中的每一个元素就是一个完整的UDP 包。当应用程序通过recvfrom()读取时，OS 把相应的一个完整UDP 包取出，然后拷贝到用户提供的内存中，物理用户提供的内存大小是多少，OS 都会完整取出一个UDP 包。如果用户提供的内存小于这个UDP 包的大小，那么在填充满内存后，UDP 包剩余的部分就会被丢弃，以后再也无法取回。  
UDP协议包括：TFTP,SNMP,NFS,DNS,BOOTP
~~~

### 6.3协议选择

~~~c++
1.对数据可靠性要求高的应用，比如验证码，密码的传送需要选择TCP协议；而对数据可靠性要求不那么高的应用，可以选择UDP协议；
2.对实时性要求比较高的应用，比如语音通话，视频监控等，选择UDP协议，而对实时性要求不那么高的应用可以选择TCP协议；
3.在网络状况比较差的情况下，比如广域网中，选择TCP协议，而在网络状况良好的情况下，可以选择UDP协议来减少网络负荷。
~~~

