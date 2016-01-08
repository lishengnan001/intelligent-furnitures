#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#include <netinet/tcp.h>
#include <resolv.h>

#include <assert.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <semaphore.h>





#include "uart_client.h"
#include "uni.h"


#define DEBUG

int serverfd;
struct sockaddr_in server_addr, client_addr;
int client_addr_len;
int clientfd[5];
static unsigned char led[38];
sem_t sem_uart_w, sem_socket_r;
#define DEBUG

int strhextodec(char *str_hex);


void  server1(void *arg);
void  server2(void *arg);
void  server3(void *arg);
void  server4(void *arg);
void server5(void *arg);
void sig_chld(int signo);
void CreateCommandServer(int *serverfd, 
						      struct sockaddr_in *server_addr);
int sk_client_create(global_data_base*glb);
void skc_read_g2(void *arg);

int OpenUart(void);
void str_echo(global_data_base *global, int server_num);
void UartRead(global_data_base *global);
void  UartServerR(void *arg);
void  UartServerW(void *arg);
void UartWrite(global_data_base *global);
void LedScreenView(char *cmd,int uart_fd);
int write_coor_uart(int fd, char *uart_cmd, int cmd_len);






 
 global_data_base global_base;





int main(int argc, char *argv[]){
	global_data_base *global = &global_base;
	
	int ret;
	int i;
	//pthread_t server_uart_r;
	//pthread_t server_uart_w;
	pthread_t server_pthread1;
	pthread_t server_pthread2;
	pthread_t server_pthread3;
	pthread_t server_pthread4;
	pthread_t server_pthread5;
	pthread_t pth_skc_read_g2;
	pthread_mutex_init(&global->mutex, NULL);
	pthread_mutex_init(&global->socket_mutex, NULL);
	for (i = 0; i < SOCKETFDLEN; i ++)
		global->socketfd_in[i] = -1;
	/*global->coor_uartfd  = OpenUart();
	if(global->coor_uartfd < 0){
		perror("open coor_uart error:");
	}else{
		printf("=====================open coor uart success!\n");
	}
	*/
	ret=sem_init(&sem_socket_r,0,1);
	if(ret!=0)
	{
		perror("sem_socket_r_init");
	}
	ret=sem_init(&sem_uart_w,0,0);
	if(ret!=0)
	{
		perror("sem_uart_w_init");
	}
	  // Create socket client, communication between Central Server and gateway.
	if (0 != sk_client_create(&global_base))
		exit(EXIT_FAILURE);
	CreateCommandServer(&serverfd, &server_addr);
	
	signal(SIGCHLD,sig_chld);
/*
	ret = pthread_create(&server_uart_r, NULL, (void*)UartServerR, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}
	


	ret = pthread_create(&server_uart_w , NULL, (void*)UartServerW, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}
	*/

	ret = pthread_create(&server_pthread1, NULL, (void*)server1, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}

	
	
	ret = pthread_create(&server_pthread2, NULL, (void*)server2, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}
	
	

	ret = pthread_create(&server_pthread3, NULL, (void*)server3, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}

	ret = pthread_create(&server_pthread4, NULL, (void*)server4, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}

	ret = pthread_create(&server_pthread5, NULL, (void*)server5, NULL);
	if(ret != 0)
	{
		perror("pthread_create: idx_th");
		exit(EXIT_FAILURE);
	}
	
	ret = pthread_create ( &pth_skc_read_g2,
		 NULL, /*&pth_attr,*/ 
		 (void *)skc_read_g2,
		 NULL );
    if (0 != ret) {
		perror("pthread_create(skc_read_g2)");
		exit(EXIT_FAILURE);
	  }
/*	*/
	//while(1){
		//printf("waiting massage...\n");
		//sleep(1);
	//}
	//pthread_join(server_uart_r, NULL);
	//pthread_join(server_uart_w, NULL);
	pthread_join(server_pthread1, NULL);
	pthread_join(server_pthread2, NULL);
	pthread_join(server_pthread3, NULL);
	pthread_join(server_pthread4, NULL);
	pthread_join(server_pthread5, NULL);
	if (0 != pthread_join(pth_skc_read_g2, NULL)) {
    perror("pthread_join");
  }
	exit(1);
}

void sig_chld(int signo){
	pid_t child_pid;
	int child_status;
	while(child_pid = waitpid(-1,&child_status, WNOHANG) > 0);
	printf("status = %d\n",child_status);
}

void UartRead(global_data_base *global){
	size_t nread, nwrite;
	int maxfdpl;
	int setect_status;
	struct timeval timeout;
	fd_set rset;
	int i;
	FD_ZERO(&rset);
	while(1){
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		FD_SET( global->coor_uartfd, &rset);
		maxfdpl =  global->coor_uartfd + 1;
		if((setect_status = select(maxfdpl, &rset, NULL, NULL, &timeout)) < 0)
			perror("select error");

		if(FD_ISSET(global->coor_uartfd, &rset)){
			again:
				if((nread = read(global->coor_uartfd, 
					global->uart_input_buf, 
					sizeof(global->uart_input_buf))) > 0){
					collection_endpoint_info(global->uart_input_buf, 1);
					//printf("***nread**:%d\n", nread);
					//printf("global->send_cmd %d\n",global->send_cmd );
					//for (i = 0; i < nread; i ++)
					//printf("data from uart is %x\n", global->uart_input_buf[i]);
					continue;
					
				}		
				else if(nread < 0 && errno == 	EINTR){
					printf("read again ,,,,,,,,,,,,,,,,,,,,,,,\n");
					goto again;
				}
				else if(nread < 0){
					perror("read error");
					return;
					//exit(1);
				}	
				
		}
		else{
			
			//printf("have no uart data to read!\n");
		}
	}
}
/**/

void  UartServerR(void *arg){
	printf("UartServerR thread is servering for you........................................\n");	
	UartRead(&global_base);
}
void UartWrite(global_data_base *global){
	size_t nwrite;
	int ret;
	printf("UartWrite is runing........................................\n");	
	while(1){
		sem_wait(&sem_uart_w);
		printf("write cmd to coor!global->uart_buf_size %d\n", global->uart_buf_size);
		if((nwrite = write(global->coor_uartfd, 
			global->uart_output_buf, 
			global->uart_buf_size)) < 0){
			perror("write error");
			return;
		}
		printf("nwrite = %d\n", nwrite);
	}
	
}

void  UartServerW(void *arg){
	printf("UartServerW thread is server for you.............................\n");
	UartWrite(&global_base);
}



void  server1(void *arg){
	printf("server1 is servering for you........................................\n");
	while(1){
	
		client_addr_len= sizeof(client_addr);
		if((global_base.socketfd_in[0]= accept(serverfd, 
			(struct sockaddr *)&client_addr, 
			&client_addr_len)) < 0){
			perror("accept error");
			continue;
		}else{
			printf("I'm glad to server for you!My id is %d\n",global_base.socketfd_in[0]);
		}
			
		str_echo(&global_base,0);
		close(global_base.socketfd_in[0]);
		printf("sever1 socket is %d when closed\n",global_base.socketfd_in[0]);
		global_base.socketfd_in[0] = -1;
		printf("Server1: I'm free^^^^^^^^^^^^^^^^^^^^^\n");
	}
}



void  server2(void *arg){
	printf("server2 is servering for you........................................\n");
	while(1){
		client_addr_len= sizeof(client_addr);
		if((global_base.socketfd_in[1]= accept(serverfd, 
			(struct sockaddr *)&client_addr, 
			&client_addr_len)) < 0){
			perror("accept error");
			continue;
		}else{
			printf("I'm glad to server for you!My id is %d\n",global_base.socketfd_in[1]);
			
		}
		str_echo(&global_base,1);
		close(global_base.socketfd_in[1]);
		printf("sever2 socket is %d when closed\n",global_base.socketfd_in[1]);
		global_base.socketfd_in[1] = -1;
		printf("Server2: I'm free^^^^^^^^^^^^^^^^^^^^^\n");
	}
}

void  server3(void *arg){
		printf("server3 is servering for you........................................\n");
		while(1){
		client_addr_len= sizeof(client_addr);
		if((global_base.socketfd_in[2]= accept(serverfd, 
			(struct sockaddr *)&client_addr, 
			&client_addr_len)) < 0){
			perror("accept error");
			continue;
		}else{
			printf("I'm glad to server for you!My id is %d\n",global_base.socketfd_in[2]);
			
		}
		str_echo(&global_base,2);
		close(global_base.socketfd_in[2]);
		printf("sever3 socket is %d when closed\n",global_base.socketfd_in[2]);
		global_base.socketfd_in[2] = -1;
		printf("Server3: I'm free^^^^^^^^^^^^^^^^^^^^^\n");
	}
}

void  server4(void *arg){
	printf("server4 is servering for you........................................\n");
	while(1){
		client_addr_len= sizeof(client_addr);
		if((global_base.socketfd_in[3]= accept(serverfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0){
			perror("accept error");
			continue;//exit(1);
		}else{
			printf("I'm glad to server for you!My id is %d\n",global_base.socketfd_in[3]);
		}
		str_echo(&global_base,3);
		close(global_base.socketfd_in[3]);
		printf("sever4 socket is %d when closed\n",global_base.socketfd_in[3]);
		global_base.socketfd_in[3] = -1;
		printf("Server4: I'm free^^^^^^^^^^^^^^^^^^^^^\n");
	}
}

void  server5(void *arg){
	printf("server5 is servering for you........................................\n");
	while(1){
		client_addr_len= sizeof(client_addr);
		if((global_base.socketfd_in[4] = accept(serverfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0){
			perror("accept error");
			continue;//exit(1);
		}else{
			printf("I'm glad to server for you!My id is %d\n",global_base.socketfd_in[4]);
		}
		str_echo(&global_base,4);
		close(global_base.socketfd_in[4]);
		printf("sever5 socket is %d when closed\n",global_base.socketfd_in[4]);
		global_base.socketfd_in[4] = -1;
		printf("Server5: I'm free^^^^^^^^^^^^^^^^^^^^^\n");
	}
}


void CreateCommandServer(int *serverfd, struct sockaddr_in *server_addr){
	int bind_result, listen_result;
	if((*serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket error");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(6666);
	//inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

	if((bind_result= bind(*serverfd, (struct sockaddr *)server_addr, sizeof(*server_addr))) < 0){
		perror("bind error");
		exit(1);
	}

	if((listen_result = listen(*serverfd, 5)) < 0){
		perror("listen error");
		exit(1);
	}
}

int sk_client_create(global_data_base *glb)
{
  unsigned int cnt;
  unsigned int optval;
  int ret = 0;
  struct timeval tv;
  tv.tv_sec = 1;  /* 3 Secs Timeout */
  cnt = 1;

 // for (cnt = 0; cnt < glb->gateway_num; cnt++) {
#ifdef DEBUG
    printf("socket create start %d\n", cnt);
#endif
   // if ((0 == glb->gw_array[cnt].state) && (1 == glb->gw_array[cnt].online)) {
      if (-1 == (glb->gw_array[cnt].sock_fd = socket(PF_INET, SOCK_STREAM, 0))) {
	perror("socket");
	glb->gw_array[cnt].inuse  = 1;
	glb->gw_array[cnt].online = 0;
	// return -1;
	// exit(EXIT_FAILURE);
      } else {
#ifdef DEBUG
        printf("glb->gw_array[%d] = %d\n", cnt, glb->gw_array[cnt].sock_fd);
#endif
        printf(" ========== socket create succeed\n");
      }
#ifdef DEBUG
      printf("setsockopt SO_REUSEADDR start\n");
#endif
      optval = 1;
      ret = setsockopt(glb->gw_array[cnt].sock_fd,
		       SOL_SOCKET, SO_REUSEADDR,
		       (char *) &optval,
		       sizeof(optval));
      if (0 != ret) {
	perror("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
	return -1;
      } else {
	printf(" ========== setsockopt SO_REUSEADDR scceed\n");
      }

      // * Set TCP Nodelay
#ifdef DEBUG
      printf("setsockopt TCP_NODELAY start\n");
#endif
      optval = 1;
      ret = setsockopt(glb->gw_array[cnt].sock_fd,
		       IPPROTO_TCP,
		       TCP_NODELAY,
		       (char *) &optval,
		       sizeof(optval));
      if (0 != ret) {
	perror("setsockopt(IPPROTO_TCP, TCP_NODELAY)");
	return -1;
      } else {
	printf(" ========== setsockopt TCP_NODELAY scceed\n");
      }

      ret = setsockopt(glb->gw_array[cnt].sock_fd,
		       SOL_SOCKET,
		       SO_RCVTIMEO,
		       (struct timeval *)&tv,
		       sizeof(struct timeval));
      if (0 != ret) {
	perror("setsockopt(SOL_SOCKET, SO_RCVTIMEO)");
	//printf("ERROR: setsockopt SO_RCVTIMEO failed\n");
      } else {
	printf(" ========== setsockopt SO_RCVTIMEO succeed\n");
      }

	  bzero(&glb->gw_array[cnt].serv_addr, sizeof(glb->gw_array[cnt].serv_addr));
	  glb->gw_array[cnt].serv_addr.sin_family = AF_INET;
	  glb->gw_array[cnt].serv_addr.sin_port = htons(8888);
	  inet_pton(AF_INET, "192.168.1.20", &glb->gw_array[cnt].serv_addr.sin_addr);

      /*
      addr.sin_family = AF_INET;
      //addr.sin_port = htons(6666);
      addr.sin_port = htons(8888);
      inet_aton("127.0.0.1", &addr.sin_addr);
      //inet_atone("192.9.127.79", &addr.sin_addr);
      */
      if (-1 == connect(glb->gw_array[cnt].sock_fd,
			(struct sockaddr *)&(glb->gw_array[cnt].serv_addr),
			sizeof(struct sockaddr_in ))) {
	printf("connect: %s\n", strerror(errno));
        glb->gw_array[cnt].state = 0;
	printf(" ********** Connect Failed. %d\n", cnt);
      } else {
	glb->gw_array[cnt].state = 1;
	//glb->gw_array[cnt].online = 1;
	//write(glb->gw_array[cnt].sock_fd, "hello", 5);
	printf(" ++++++++++ +++++++++++++++++++++++++Connect Successed. %d\n", cnt);
      }
   // }
 // }

  return 0;
}

int OpenUart(void){		
	struct termios options, oldoptions;  // 定义串口属性结构体变量，其中options用于重新设置串口属性，oldoptions用于原设置，用于恢复到原来的属性		
	int fd;		
	fd = open("/dev/ttyS/*AC*/0", O_RDWR);     // 打开串口1		
	tcgetattr(fd, &oldoptions);			// 获得当前的串口设置信息		
	tcgetattr(fd, &options);	
	cfsetispeed(&options, B115200);			// 设置输入波特率为115200		
	cfsetospeed(&options, B115200);			// 设置输出波特率为115200		
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;	
	options.c_cflag &= ~PARENB;			// 设置串口属性为：数据位为8位，停止位为1位，无奇偶校验		
	options.c_cflag &= ~CSTOPB;		
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);		// 设置串口为Raw模式		
	options.c_oflag &= ~OPOST;		
	tcsetattr(fd, TCSANOW, &options);		// 将上述设置生效		
	return(fd);
}

void str_echo(global_data_base *global, int server_num){
	size_t nread, nwrite;
	int maxfdpl;
	int setect_status;
	struct timeval timeout;
	fd_set rset;
	int i;
	FD_ZERO(&rset);
	while(1){
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		FD_SET( global->socketfd_in[server_num], &rset);
		maxfdpl =  global->socketfd_in[server_num] + 1;
		/*if(pthread_mutex_lock(&global->socket_mutex) != 0)
		{
			perror("pthread_mutex_lock");
			continue;//exit(EXIT_FAILURE);
		}*/
		if((setect_status = select(maxfdpl, &rset, NULL, NULL, &timeout)) < 0)
			perror("select error");

		if(FD_ISSET(global->socketfd_in[server_num], &rset)){
			printf("start to read cmd from client !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			again:
				if(pthread_mutex_lock(&global->mutex) != 0)
				{
					perror("pthread_mutex_lock");
					exit(EXIT_FAILURE);
				}
				
				 memset(global->socket_input_buf , 0, sizeof(global->socket_input_buf ));	
				nread = read(global->socketfd_in[server_num], 
					global->socket_input_buf , 
					sizeof(global->socket_input_buf ));
				//printf("cmd from client is %s***nread**:%d\n",global->socket_input_buf , nread);
				if(nread > 0){
					printf("cmd form client %d  is %s\n",  global->socketfd_in[server_num], global->socket_input_buf );
						//global->gw_array[1].online = 1;
						 //write_socket(global, 1);
						sk_cmd_dispatch(global->socket_input_buf );

					//
					if(pthread_mutex_unlock(&global->mutex) != 0)
					{
						perror("pthread_mutex_unlock");
						exit(EXIT_FAILURE);
					}
					//printf("###################################################\n");
					continue;
				}	
				else	if(nread < 0 && errno ==  EINTR){
					if(pthread_mutex_unlock(&global->mutex) != 0)
					{
						perror("pthread_mutex_unlock");
						exit(EXIT_FAILURE);
					}
					printf("read again ,,,,,,,,,,,,,,,,,,,,,,,\n");
					goto again;
				}
				else{
						perror("read error");
						close(global->socketfd_in[server_num]);
						if(pthread_mutex_unlock(&global->mutex) != 0)
						{
							perror("pthread_mutex_unlock");
							exit(EXIT_FAILURE);
						}
						return;
				}
				printf("222222222222222222222222222222222222222222222222222222222\n");
				/*if(pthread_mutex_unlock(&global->mutex) != 0)
				{
					perror("pthread_mutex_unlock");
					exit(EXIT_FAILURE);
				}*/
		}
		else{
			printf("%d : have no socket data to read!\n",server_num);
		}
		/*
		if(pthread_mutex_unlock(&global->socket_mutex) != 0)
		{
			perror("pthread_mutex_unlock");
			continue;//exit(EXIT_FAILURE);
		}
		*/
	}
}

void  build_sk_package ( void *buffer,
				  unsigned char nodeid,
				  unsigned char cmd ,unsigned char data)
  {
	UART_Format *tmp = (UART_Format *)buffer;
	tmp->header_1 = 0xCC;
	tmp->header_2 = 0xEE;
	tmp->node_seq = 0x01;
	tmp->node_id = nodeid;
	tmp->cmd = cmd;
	tmp->data[0] = data;
   	tmp->tailer = 0xFF;
  }



int sk_cmd_parse(char *sk_cmd)
{
  struct sk_cmd_pkg *sk_cmd_ptr = &sk_cmd_pkg_ins;
  char *ptr;
  char cache[100];
  char *p_cache;
  int para = 1;
  char *endptr, *str;
  int base = 10;

  memset(cache, 0, 16);
  sk_cmd_ptr->no = 0;
  sk_cmd_ptr->para_1 = 0;
  sk_cmd_ptr->para_2 = 0;
  sk_cmd_ptr->para_3 = 0;
  sk_cmd_ptr->para_4 = 0;
  ptr = sk_cmd;
  p_cache = cache;

#ifdef DEBUG
  printf("sk_cmd is: %s\n", sk_cmd);
  printf("ptr is: %s\n", ptr);
#endif



printf("backing......................................................................\n");
  for (; *ptr; ptr++) {

#ifdef DEBUG
    printf("*ptr = %c\n", *ptr);
#endif
    if ('#' != *ptr && ':' != *ptr){
      		*p_cache++ = *ptr;
    }
    if ('#' == *ptr) {

	      *(p_cache + 1)= '\0';
#ifdef DEBUG
	      printf("cache = %s\n", cache);
#endif
	      if (0 == strcmp("09", cache)) {
		sk_cmd_ptr->no = 1;
		memset(cache, 0, 16);
		p_cache = cache;
		goto done;
	      }
		  
	      if (0 == strcmp("0a", cache)) {
		sk_cmd_ptr->no = 1;
		memset(cache, 0, 16);
		p_cache = cache;
		goto done;
	      }
  
	      if (0 == strcmp("20", cache)) {
		sk_cmd_ptr->no = 1;
		memset(cache, 0, 16);
		p_cache = cache;
		goto done;
	      }

	      if (0 == strcmp("21", cache)) {
		sk_cmd_ptr->no = 1;
		memset(cache, 0, 16);
		p_cache = cache;
		//*(sk_cmd + 40) = '\0';
		sk_cmd_ptr->para_2 = 31;
		strncpy(sk_cmd_ptr->led_screen, sk_cmd + 9,48);
		goto screen;
	      }

	      if (0 == strcmp("0f", cache)) {
		sk_cmd_ptr->no = 1;
		memset(cache, 0, 16);
		p_cache = cache;
		goto done;
	      }

	      str = cache;
#ifdef DEBUG
	      printf(" str = %s\n", str);
#endif
	      sk_cmd_ptr->no = strtol(str, &endptr, base);
#ifdef DEBUG
	      printf("sk_cmd_ptr->no = %ld\n", sk_cmd_ptr->no);
#endif
	      memset(cache, 0, 16);
	      p_cache = cache;
    }

  done:

    if (':' == *ptr) {
      if (1 == para) {
	str = cache;
	printf("cache = %s\n", cache);
	sk_cmd_ptr->para_1 = strtol(str, &endptr, base);
	printf("sk_cmd_ptr->para_1 = %ld\n", sk_cmd_ptr->para_1);
	para++;
	memset(cache, 0, 16);
	p_cache = cache;
      }
      else if (2 == para) {
	str = cache;
	printf("cache = %s\n", cache);
	sk_cmd_ptr->para_2 = strtol(str, &endptr, base);
	printf("sk_cmd_ptr->para_2 = %ld\n", sk_cmd_ptr->para_2);
	para++;
	memset(cache, 0, 16);
	p_cache = cache;
      } else if (3 == para) {
	str = cache;
	printf("cache = %s\n", cache);
	sk_cmd_ptr->para_3 = strtol(str, &endptr, base);
	printf("sk_cmd_ptr->para_3 = %ld\n", sk_cmd_ptr->para_3);
	para++;
	memset(cache, 0, 16);
	p_cache = cache;
      }  else
	printf("Fatal error, cannot parse parameter from Central Console\n");
    }
  } // end of while
/**/
  printf("cache = %s\n", cache);
  if (2 == para) {
    str = cache;
    sk_cmd_ptr->para_2 = strtol(str, &endptr, base);
    printf("sk_cmd_ptr->para_2 = %ld\n", sk_cmd_ptr->para_2);
  } else if (3 == para) {
    str = cache;
    sk_cmd_ptr->para_3 = strtol(str, &endptr, base);
    printf("sk_cmd_ptr->para_3 = %ld\n", sk_cmd_ptr->para_3);
  } else if (4 == para) {
    str = cache;
    sk_cmd_ptr->para_4 = strtol(str, &endptr, base);
    printf("sk_cmd_ptr->para_4 = %ld\n", sk_cmd_ptr->para_4);
  }

  printf("no = %ld, para_1 = %ld, para_2 = %ld, para_3 = %ld, para_4 = %ld\n",
	 sk_cmd_ptr->no, sk_cmd_ptr->para_1, sk_cmd_ptr->para_2, sk_cmd_ptr->para_3, sk_cmd_ptr->para_4);
screen:
	printf(".......................%s\n",sk_cmd_ptr->led_screen);
  return 0;
}



void LedScreenView(char *cmd,int uart_fd){
	unsigned char led_view[38];
	int nwrite;
	 led_view[0] = 0xcc;
	led_view[1] = 0xEE;
	led_view[2] = 0x01;
	led_view[3] = CMD_ID_SET_LED_SCREEM;
	led_view[4] = 0x01;
	led_view[37] = 0xff;
	strcpy(&led_view[5], cmd);
	if((nwrite = write(uart_fd, 
					led_view, 
					38)) < 0){
					perror("write error");
					return;
				}
				printf("nwrite = %d\n", nwrite);
}
/**/

int close_socket(global_data_base *glb)
{
  unsigned int cnt = 0;
  glb = &global_base;

  for (cnt = 0; cnt < glb->gateway_num; cnt++) {
    if (1 == glb->gw_array[cnt].inuse &&
	((0 == glb->gw_array[cnt].sync) || (0 == glb->gw_array[cnt].online))) {
      printf("glb->sta_array[%d].inuse is: %d.\n",
	     cnt, glb->gw_array[cnt].inuse);
      printf("\nCLOSE SOCKET.\n\n");
      if ( -1 == close(glb->gw_array[cnt].sock_fd)) {
	perror("close");
	return -1;
      }
      glb->gw_array[cnt].state = 0;
    }
  }

  for(cnt = 0; cnt < glb->gateway_num; cnt++) {
    if (1 == glb->gw_array[cnt].inuse) {
      printf("\nsync = 0.\n\n");
      glb->gw_array[cnt].sync = 0;
      printf("\ninuse = 0.\n\n");
      glb->gw_array[cnt].inuse = 0;
    }
  }

  return 0;
}
/*
int read_socket(global_data_base *glb, unsigned int gw_cnt)
{
  int ret;
  glb->uart_buf_size = 0;
  // struct global * glb;
  glb = & global_base;

  if ((1 == glb->gw_array[gw_cnt].state) &&
      (1 == glb->gw_array[gw_cnt].online)) {
    for (;;) {
       printf("read start\n");
   
      ret = read(glb->gw_array[gw_cnt].sock_fd,
		 (void *)glb->uart_input_buf,
		 L_SIZE);

      if (ret > 0) {
	glb->uart_buf_size = ret;
	printf("data of readsocket is : %s\n",glb->uart_input_buf);
	return(ret);
      } else {
	if (errno == EINTR) {
	  printf("read EINTR \n");
	  continue;

	} else {
#ifdef DEBUG
	  // printf("errno is decimal: %d, hexadecimal: %x\n", errno, errno);
	  printf("read error %s\n", strerror(errno));
	  return -1;
	  // exit(1);
#endif
	}
      }
    }
  } else {
    return -1;
  }

  return 0;
}
*/
int read_socket(global_data_base *glb, unsigned int gw_cnt){
	size_t nread, nwrite;
	int maxfdpl;
	int setect_status;
	struct timeval timeout;
	fd_set rset;
	int i;
	FD_ZERO(&rset);
	while(1){
	 if ((1 == glb->gw_array[gw_cnt].state) &&
      (1 == glb->gw_array[gw_cnt].online)) {
	
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		FD_SET( glb->gw_array[gw_cnt].sock_fd, &rset);
		maxfdpl =  glb->gw_array[gw_cnt].sock_fd + 1;
		if((setect_status = select(maxfdpl, &rset, NULL, NULL, &timeout)) < 0)
			perror("select error");

		if(FD_ISSET(glb->gw_array[gw_cnt].sock_fd, &rset)){
			again:
				
				 memset(glb->uart_input_buf, 0, sizeof(glb->uart_input_buf ));	
				nread = read(glb->gw_array[gw_cnt].sock_fd, 
					glb->uart_input_buf , 
					sizeof(glb->uart_input_buf));
				//printf("cmd from client is %s***nread**:%d\n",global->socket_input_buf , nread);
				if(nread > 0){
					printf("cmd form client %d  is %s\n",  glb->gw_array[gw_cnt].sock_fd, glb->uart_input_buf);
					
						
						collection_endpoint_info(glb->uart_input_buf, gw_cnt);

					continue;
				}	
				else	if(nread < 0 && errno ==  EINTR){
					
					goto again;
				}
				else{
						perror("read error");
						close(glb->gw_array[gw_cnt].sock_fd);
						
						return ( -1);
				}
				
		}
		else{
			printf("have no socket data to read from QtSrver!\n");
		}
	}
}
}



int write_socket(global_data_base *glb, unsigned int gw_cnt)
{
  int ret;
  glb = & global_base;

  if ((1 == glb->gw_array[gw_cnt].state) &&
      (1 == glb->gw_array[gw_cnt].online)) {
    for (;;) {
      ret = write(glb->gw_array[gw_cnt].sock_fd,
		  (void*)glb->uart_output_buf,
		  glb->uart_buf_size);
      // ret = write(gbl->nsock_fd,"1234",4) ;
	 printf("cmd of write is : %s\n size is : %d\n",glb->socket_input_buf, ret);
      if (ret > 0 ) {
	 printf("write ok %d\n",ret);
	return 0;
      } else {

	if (errno == EINTR) {
	  printf("write EINTR \n");
	  continue;
	} else {
	  printf("write: %s\n", strerror(errno));
	  glb->gw_array[gw_cnt].inuse  = 1;
	  glb->gw_array[gw_cnt].online = 0;
	  return -1;
	  // exit(1);
	}
      }
    }
  } else {
    return -2;
  }

  return 0;
}

void skc_read_g2(void *arg)
{

  char endpoint_info[S_SIZE];
  char local_time[XS_SIZE];
  char storage_data[S_SIZE];

  global_data_base * glb;
  glb = & global_base;


  int ret;

  //struct gateway_endpoint *gw_ep_ptr;
  printf("client server for you.............\n");
	
      if ((ret = read_socket(glb, 1)) < 0) {

	return;
      }/**/
    }


int sk_cmd_dispatch(char *client_cmd)
{
  int ret;
  struct sk_cmd_pkg *sk_cmd_ptr = &sk_cmd_pkg_ins;
   global_data_base *global = &global_base;
  char local_time[XS_SIZE];
  int val;


#ifdef DEBUG
  printf("================= cmd from client = %s\n", client_cmd);
#endif
  sk_cmd_parse(client_cmd);
if(sk_cmd_ptr->no){
  switch (sk_cmd_ptr->para_2) {


  	case CMD_ID_SET_LED_ALARM:
	printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
	         if (0 == sk_cmd_ptr->para_4) {
			// Turn Led Alarm Off
			printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
			build_sk_package( global->uart_output_buf,
			  CMD_ID_SET_LED_ALARM , 
			  CMD_ID_SET_LED_1_OFF,
			  0);

			global->uart_output_buf[16] = 0;   
		    global->uart_buf_size = 17;
			global->gw_array[1].online = 1;
		    write_socket(global, 1);
			sem_post(&sem_uart_w);
			
			
			printf("Build command end\n");

		  }
		 else if (1 == sk_cmd_ptr->para_4) {
  
			// Turn Led Alarm On
			printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
			build_sk_package(global->uart_output_buf, 
			  CMD_ID_SET_LED_ALARM,
			  CMD_ID_SET_LED_1_ON,
			  0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);

			printf("Build command end\n");

		  } 
		  else
			printf(" \n********** Error occur, Leds 1 operation not matched\n");
	  break;

	case CMD_ID_SET_CHUANGLIAN_CHAN:
	printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
	         if (0 == sk_cmd_ptr->para_4) {
			// Turn Led 1 Off
			printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
			build_sk_package( global->uart_output_buf,
			  CMD_ID_SET_CHUANGLIAN_CHAN , 
			  CMD_ID_SET_LED_1_OFF,
			  0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);

			printf("Build command end\n");
	  
		  }
		 else if (1 == sk_cmd_ptr->para_4) {
  
			// Turn Led 1 On
			printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
			build_sk_package( global->uart_output_buf, 
			  CMD_ID_SET_CHUANGLIAN_CHAN,
			  CMD_ID_SET_LED_1_ON,
			  0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			printf("Build command end\n");

		  } 
		  else
			printf(" \n********** Error occur, Leds 1 operation not matched\n");
	  break;

  case CMD_ID_SET_LED1:
    printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
	if (0 == sk_cmd_ptr->para_4) {
	  // Turn Led 1 Off
	  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
	  build_sk_package( global->uart_output_buf,
	  	CMD_ID_SET_LED1 , 
	  	CMD_ID_SET_LED_1_OFF,
	  	0);

		global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
	 global->gw_array[1].online = 1;
				write_socket(global, 1);
	 //LedScreenView("关闭日光灯1",global->coor_uartfd);
	sem_post(&sem_uart_w);
	  // sleep(1);
	 //
	  printf("Build command end\n");

	} else if (1 == sk_cmd_ptr->para_4) {

	  // Turn Led 1 On
	  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
	  build_sk_package(global->uart_output_buf, 
	  	CMD_ID_SET_LED1,
	  	CMD_ID_SET_LED_1_ON,
	  	0);
		global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
	  global->gw_array[1].online = 1;
	write_socket(global, 1);

	sem_post(&sem_uart_w);
	 // sleep(1);
	 //LedScreenView("打开日光灯1",global->coor_uartfd);
	  printf("Build command end\n");

	} else
	  printf(" \n********** Error occur, Leds 1 operation not matched\n");
	break;

	case CMD_ID_SET_HUM:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_HUM , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
	global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
		 global->gw_array[1].online = 1;
				write_socket(global, 1);
		
		sem_post(&sem_uart_w);
		 // sleep(1);
		 //LedScreenView("关闭加湿器",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_HUM,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
			global->uart_output_buf[16] = 0;   
		 	 global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			
			sem_post(&sem_uart_w);
			//  sleep(1);
			// LedScreenView("打开加湿器",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_TAIDENG:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_TAIDENG , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			 //sleep(1);
			// LedScreenView("关闭台灯",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_TAIDENG,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			  //sleep(1);
			// LedScreenView("打开台灯",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

		case CMD_ID_SET_FENGSHAN:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off

		  printf("Build command \"CMD_ID_SET_FENGSHAN_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_FENGSHAN , 
		  	CMD_ID_SET_FENGSHAN_02,
		  	0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			 // sleep(1);
			// LedScreenView("关闭风扇",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_FENGSHAN1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_FENGSHAN,
		  	CMD_ID_SET_FENGSHAN_01,
		  	0);
		  
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
		  global->gw_array[1].online = 1;
		
				write_socket(global, 1);
				
			sem_post(&sem_uart_w);
			 // sleep(1);
			 //LedScreenView("打开风扇",global->coor_uartfd);
		  printf("Build command end\n");

		}else if (2 == sk_cmd_ptr->para_4) {
           /*
               Liqiang add 风扇摇头， 时间 2015-1-21 10:06:48
           */ 
		  // Turn Led 1 On
		  //printf("Build command \"CMD_ID_SET_FENGSHAN1_ON\" start\n");
            printf("&&&&&&&&&&This is yao tou\n");
          build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_FENGSHAN,
		  	CMD_ID_SET_FENGSHAN_03,
		  	0);
		  
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
		  global->gw_array[1].online = 1;
		
				write_socket(global, 1);
				
			sem_post(&sem_uart_w);
			 // sleep(1);
			 //LedScreenView("打开风扇",global->coor_uartfd);
		  printf("Build command end\n");

		}else if (3 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		 // printf("Build command \"CMD_ID_SET_FENGSHAN1_ON\" start\n");
            /*liqiang add 风扇风速控制 2015-1-21 10:07:39*/
            printf("&&&&&&&&&&This is feng su!\n");
            build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_FENGSHAN,
		  	CMD_ID_SET_FENGSHAN_04,
		  	0);
		  
		  global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
		  global->gw_array[1].online = 1;
		
				write_socket(global, 1);
				
			sem_post(&sem_uart_w);
			 // sleep(1);
			 //LedScreenView("打开风扇",global->coor_uartfd);
		  printf("Build command end\n");

		}else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LEDHEAD1:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_LEDHEAD1 , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			  //sleep(1);
			 //LedScreenView("关闭灯头1",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_LEDHEAD1,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			// LedScreenView("打开灯头1",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LEDHEAD2:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_LEDHEAD2 , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("关闭灯头2",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_LEDHEAD2,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
		 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			 // sleep(1);
			//LedScreenView("打开灯头2",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LEDHEAD3:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf,
		  	CMD_ID_SET_LEDHEAD3 , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("关闭灯头3",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_LEDHEAD3,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("打开灯头3",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LEDHEAD4:
    		printf("CMD_ID_GET_SET_LEDS_MOTOR\n");
		if (0 == sk_cmd_ptr->para_4) {
		  // Turn Led 1 Off
		  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
		  build_sk_package( global->uart_output_buf ,
		  	CMD_ID_SET_LEDHEAD4 , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("关闭灯头4",global->coor_uartfd);
		  printf("Build command end\n");

		} else if (1 == sk_cmd_ptr->para_4) {

		  // Turn Led 1 On
		  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_LEDHEAD4,
		  	CMD_ID_SET_LED_1_ON,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("打开灯头4",global->coor_uartfd);
		  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LED2:
    		printf("CMD_ID_SET_LED2\n");
		if (0 == sk_cmd_ptr->para_4){
		  printf("Build command \"CMD_ID_SET_LED2\" start\n");
		  build_sk_package(global->uart_output_buf,
		  	CMD_ID_SET_LED2 , 
		  	CMD_ID_SET_LED_1_OFF,
		  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			 // sleep(1);
			//LedScreenView("关闭日光灯2",global->coor_uartfd);
		  printf("Build command end\n");

		} 
  		else if (1 == sk_cmd_ptr->para_4) {

			  // Turn Led 1 On
			  printf("Build command \"CMD_ID_SET_LED_1_ON\" start\n");
			  build_sk_package( global->uart_output_buf, 
			  	CMD_ID_SET_LED2,
			  	CMD_ID_SET_LED_1_ON,
			  	0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
			//  sleep(1);
			//LedScreenView("打开日光灯2",global->coor_uartfd);
			  printf("Build command end\n");

		} else
		  printf(" \n********** Error occur, Leds 1 operation not matched\n");
		break;

	case CMD_ID_SET_LED_SCREEM:
			printf("CMD_ID_SET_LED_SCREEM\n");
			  printf("Build command \"CMD_ID_SET_LED_1_OFF\" start\n");
			  printf("***************** led_screen is ***************%s\n",sk_cmd_ptr->led_screen);
			 int i;
			for ( i = 0; i < sizeof(sk_cmd_ptr->led_screen); i ++)
			//printf("sk_cmd_ptr->led_screen is................................... %x\n", sk_cmd_ptr->led_screen[i]);
			memset(global->uart_output_buf, 0, 38);
			 global->uart_output_buf[0] = 0xCC;
			 global->uart_output_buf[1] = 0xEE;
			 global->uart_output_buf[2] = 0x01;
			 global->uart_output_buf[3] = CMD_ID_SET_LED_SCREEM;
			 global->uart_output_buf[4] = 0x01;
			 
			 Utf8ToGb2312(sk_cmd_ptr->led_screen, 
			 	strlen(sk_cmd_ptr->led_screen) , 
			 	&global->uart_output_buf[5]);

			 global->uart_output_buf[37] = 0xff;
			global->uart_output_buf[38] = 0x00;
			 //  
			  
			 //strcpy(global->uart_output_buf, led);
		
		  global->uart_buf_size = 39;
			  global->gw_array[1].online = 1;
			write_socket(global, 1);
			sem_post(&sem_uart_w);
			
			break;

	case CMD_ID_GET_SET_PWM_LEDOLD:
			printf("CMD_ID_GET_SET_PWM_LED\n");
			if (1 == sk_cmd_ptr->para_1) {	// gateway 1
	
			  // val = gw_ep_ins[coor_id][0].led_pwm_ins.pwm_val;
	
			  // ignored para_2, endpoint num
	
			  // Leds number
			 // LedScreenView("调光1... ...",global->coor_uartfd);
		//		sleep(1);
			  switch (sk_cmd_ptr->para_3) {
	
			  case 0:  // Led light level
	
			printf("Build command \"CMD_ID_SET_PWD_00\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,  
				CMD_ID_SET_PWD_00,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 1:
			printf("Build command \"CMD_ID_SET_PWD_01\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD, 
				CMD_ID_SET_PWD_01,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  case 2:
			printf("Build command \"CMD_ID_SET_PWD_02\" start\n");
			build_sk_package(global->uart_output_buf , 
				CMD_ID_GET_SET_PWM_LEDOLD, 
				CMD_ID_SET_PWD_02,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 3:
			printf("Build command \"CMD_ID_SET_PWD_03\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD, 
				CMD_ID_SET_PWD_03, 0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 4:
			printf("Build command \"CMD_ID_SET_PWD_04\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD, 
				CMD_ID_SET_PWD_04,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 5:
			printf("Build command \"CMD_ID_SET_PWD_05\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,
				CMD_ID_SET_PWD_05,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 6:
			printf("Build command \"CMD_ID_SET_PWD_06\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,
				CMD_ID_SET_PWD_06,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 7:
			printf("Build command \"CMD_ID_SET_PWD_07\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,
				CMD_ID_SET_PWD_07,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 8:
			printf("Build command \"CMD_ID_SET_PWD_08\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,
				CMD_ID_SET_PWD_08,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 9:
			printf("Build command \"CMD_ID_SET_PWD_09\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LEDOLD,
				CMD_ID_SET_PWD_09,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  }
			}
			break;

    case CMD_ID_GET_SET_PWM_LED1:
	    printf("CMD_ID_GET_SET_PWM_LED\n");
	    if (1 == sk_cmd_ptr->para_1) {  // gateway 1

	      // val = gw_ep_ins[coor_id][0].led_pwm_ins.pwm_val;

	      // ignored para_2, endpoint num

	      // Leds number
	     // LedScreenView("调光1... ...",global->coor_uartfd);
	//	    sleep(1);
	      switch (sk_cmd_ptr->para_3) {

	      case 0:  // Led light level

		printf("Build command \"CMD_ID_SET_PWD_00\" start\n");
		build_sk_package(global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,  
			CMD_ID_SET_PWD_00,
			0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 1:
		printf("Build command \"CMD_ID_SET_PWD_01\" start\n");
		build_sk_package(global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1, 
			CMD_ID_SET_PWD_01,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;
	      case 2:
		printf("Build command \"CMD_ID_SET_PWD_02\" start\n");
		build_sk_package(global->uart_output_buf , 
			CMD_ID_GET_SET_PWM_LED1, 
			CMD_ID_SET_PWD_02,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 3:
		printf("Build command \"CMD_ID_SET_PWD_03\" start\n");
		build_sk_package(global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1, 
			CMD_ID_SET_PWD_03, 0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 4:
		printf("Build command \"CMD_ID_SET_PWD_04\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1, 
			CMD_ID_SET_PWD_04,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 5:
		printf("Build command \"CMD_ID_SET_PWD_05\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,
			CMD_ID_SET_PWD_05,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 6:
		printf("Build command \"CMD_ID_SET_PWD_06\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,
			CMD_ID_SET_PWD_06,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 7:
		printf("Build command \"CMD_ID_SET_PWD_07\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,
			CMD_ID_SET_PWD_07,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 8:
		printf("Build command \"CMD_ID_SET_PWD_08\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,
			CMD_ID_SET_PWD_08,
			0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;

	      case 9:
		printf("Build command \"CMD_ID_SET_PWD_09\" start\n");
		build_sk_package( global->uart_output_buf, 
			CMD_ID_GET_SET_PWM_LED1,
			CMD_ID_SET_PWD_09,
			0);
		global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
			 global->gw_array[1].online = 1;
				write_socket(global, 1);
			sem_post(&sem_uart_w);
		printf("Build command end\n");

		break;
	      }
	    }
		break;
	case CMD_ID_GET_SET_PWM_LED2:
			printf("CMD_ID_GET_SET_PWM_LED\n");
			if (1 == sk_cmd_ptr->para_1) {	// gateway 1
	
			  // val = gw_ep_ins[coor_id][0].led_pwm_ins.pwm_val;
	
			  // ignored para_2, endpoint num
	
			  // Leds number
			//  LedScreenView("调光2... ...",global->coor_uartfd);
			//    sleep(1);
			  switch (sk_cmd_ptr->para_3) {
	
			  case 0:  // Led light level
	
			printf("Build command \"CMD_ID_SET_PWD_00\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,  
				CMD_ID_SET_PWD_00,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 1:
			printf("Build command \"CMD_ID_SET_PWD_01\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2, 
				CMD_ID_SET_PWD_01,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  case 2:
			printf("Build command \"CMD_ID_SET_PWD_02\" start\n");
			build_sk_package(global->uart_output_buf , 
				CMD_ID_GET_SET_PWM_LED2, 
				CMD_ID_SET_PWD_02,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 3:
			printf("Build command \"CMD_ID_SET_PWD_03\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2, 
				CMD_ID_SET_PWD_03, 0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 4:
			printf("Build command \"CMD_ID_SET_PWD_04\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2, 
				CMD_ID_SET_PWD_04,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 5:
			printf("Build command \"CMD_ID_SET_PWD_05\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,
				CMD_ID_SET_PWD_05,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 6:
			printf("Build command \"CMD_ID_SET_PWD_06\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,
				CMD_ID_SET_PWD_06,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 7:
			printf("Build command \"CMD_ID_SET_PWD_07\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,
				CMD_ID_SET_PWD_07,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 8:
			printf("Build command \"CMD_ID_SET_PWD_08\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,
				CMD_ID_SET_PWD_08,
				0);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 9:
			printf("Build command \"CMD_ID_SET_PWD_09\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED2,
				CMD_ID_SET_PWD_09,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  }
			}
			break;
	case CMD_ID_GET_SET_PWM_LED3:
			printf("CMD_ID_GET_SET_PWM_LED\n");
			if (1 == sk_cmd_ptr->para_1) {	// gateway 1
	
			  // val = gw_ep_ins[coor_id][0].led_pwm_ins.pwm_val;
	
			  // ignored para_2, endpoint num
	
			  // Leds number
			 
			  switch (sk_cmd_ptr->para_3) {
	
			  case 0:  // Led light level
	
			printf("Build command \"CMD_ID_SET_PWD_00\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,  
				CMD_ID_SET_PWD_00,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 1:
			printf("Build command \"CMD_ID_SET_PWD_01\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3, 
				CMD_ID_SET_PWD_01,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  case 2:
			printf("Build command \"CMD_ID_SET_PWD_02\" start\n");
			build_sk_package(global->uart_output_buf , 
				CMD_ID_GET_SET_PWM_LED3, 
				CMD_ID_SET_PWD_02,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 3:
			printf("Build command \"CMD_ID_SET_PWD_03\" start\n");
			build_sk_package(global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3, 
				CMD_ID_SET_PWD_03, 0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 4:
			printf("Build command \"CMD_ID_SET_PWD_04\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3, 
				CMD_ID_SET_PWD_04,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 5:
			printf("Build command \"CMD_ID_SET_PWD_05\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,
				CMD_ID_SET_PWD_05,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 6:
			printf("Build command \"CMD_ID_SET_PWD_06\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,
				CMD_ID_SET_PWD_06,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 7:
			printf("Build command \"CMD_ID_SET_PWD_07\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,
				CMD_ID_SET_PWD_07,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 8:
			printf("Build command \"CMD_ID_SET_PWD_08\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,
				CMD_ID_SET_PWD_08,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
	
			  case 9:
			printf("Build command \"CMD_ID_SET_PWD_09\" start\n");
			build_sk_package( global->uart_output_buf, 
				CMD_ID_GET_SET_PWM_LED3,
				CMD_ID_SET_PWD_09,
				0);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
			printf("Build command end\n");
	
			break;
			  }
			 // sleep(1);
			//   LedScreenView("调光3... ...",global->coor_uartfd);
			}
			break;
  case CMD_ID_SET_TV_VOLU_CHAN:

    printf("CMD_ID_SET_TV_VOLU_CHAN\n");

	    if (1 == sk_cmd_ptr->para_1) {          // gateway 1

	      // ignored para_2, endpoint num

	      if (1 == sk_cmd_ptr->para_3) {        // 01, Channel

		if (1 == sk_cmd_ptr->para_4) {

		  // Channel Backward

		  printf("Build command \"CMD_ID_SET_CHANNEL_BACKWARD\" start\n");
		  build_sk_package(global->uart_output_buf, 
		  	CMD_ID_SET_TV_VOLU_CHAN,
		  	CMD_ID_SET_TV_VC,
		  	CMD_ID_SET_CHANNEL_BACKWARD);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
				//sleep(1);
				//LedScreenView("向后调台", global->coor_uartfd);
		  printf("Build command end\n");

		} else if (2 == sk_cmd_ptr->para_4) {

		  // Channel Forward
		  printf("Build command \"CMD_ID_SET_CHANNEL_FORWARD\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_TV_VOLU_CHAN,
		  	CMD_ID_SET_TV_VC,
		  	CMD_ID_SET_CHANNEL_FORWARD);
					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
				//sleep(1);
				//LedScreenView("向前调台", global->coor_uartfd);
		  printf("Build command end\n");

		}

	      } 
		  else if (2 == sk_cmd_ptr->para_3) { // 02, Volume

			if (1 == sk_cmd_ptr->para_4) {

			  // Volume Decrease
			  printf("Build command \"CMD_ID_SET_VOLUME_DECREASE\" start\n");
			  build_sk_package( global->uart_output_buf, 
			  	CMD_ID_SET_TV_VOLU_CHAN,
			  	CMD_ID_SET_TV_VC,
			  	CMD_ID_SET_VOLUME_DECREASE);
			global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
					 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
					//sleep(1);
					//LedScreenView("音量减", global->coor_uartfd);
			  printf("Build command end\n");

			} else if (2 == sk_cmd_ptr->para_4) {

			  // Volume Increase
			  printf("Build command \"CMD_ID_SET_VOLUME_INCREASE\" start\n");
			  build_sk_package( global->uart_output_buf, 
			  	CMD_ID_SET_TV_VOLU_CHAN,
			  	CMD_ID_SET_TV_VC,
			  	CMD_ID_SET_VOLUME_INCREASE);

					global->uart_output_buf[16] = 0;   
		            global->uart_buf_size = 17;
					 global->gw_array[1].online = 1;
				write_socket(global, 1);
					sem_post(&sem_uart_w);
					//sleep(1);
					//LedScreenView("音量加", global->coor_uartfd);
			  printf("Build command end\n");

			}

	      }
		  else if (3 == sk_cmd_ptr->para_3) { // 02, Volume

			if (1 == sk_cmd_ptr->para_4) {

			  // Volume Decrease
			  printf("Build command \"CMD_ID_SET_VOLUME_DECREASE\" start\n");
			  build_sk_package( global->uart_output_buf, 
			  	CMD_ID_SET_TV_VOLU_CHAN,
			  	CMD_ID_SET_TV_VC,
			  	CMD_ID_SET_TV_SWITCH);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
					 global->gw_array[1].online = 1;
				write_socket(global, 1);
					sem_post(&sem_uart_w);
					//sleep(1);
					//LedScreenView("关闭电视", global->coor_uartfd);
			  printf("Build command end\n");

			} 
			else if (2 == sk_cmd_ptr->para_4) {

			  // Volume Increase
			  printf("Build command \"CMD_ID_SET_VOLUME_INCREASE\" start\n");
			  build_sk_package( global->uart_output_buf, 
			  	CMD_ID_SET_TV_VOLU_CHAN,
			  	CMD_ID_SET_TV_VC,
			  	CMD_ID_SET_TV_SWITCH);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
					 global->gw_array[1].online = 1;
				write_socket(global, 1);
					sem_post(&sem_uart_w);
					//sleep(1);
					//LedScreenView("打开电视", global->coor_uartfd);
			  printf("Build command end\n");

			}

	      } 
		   else if (4 == sk_cmd_ptr->para_3) { // 02, Volume

		if (1 == sk_cmd_ptr->para_4) {

		  // Volume Decrease
		  printf("Build command \"CMD_ID_SET_VOLUME_DECREASE\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_TV_VOLU_CHAN,
		  	CMD_ID_SET_TV_VC,
		  	CMD_ID_SET_VOLUME_QUICK);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
				//sleep(1);
				//LedScreenView("电视静音", global->coor_uartfd);
		  printf("Build command end\n");

		} else if (2 == sk_cmd_ptr->para_4) {

		  // Volume Increase
		  printf("Build command \"CMD_ID_SET_VOLUME_INCREASE\" start\n");
		  build_sk_package( global->uart_output_buf, 
		  	CMD_ID_SET_TV_VOLU_CHAN,
		  	CMD_ID_SET_TV_VC,
		  	CMD_ID_SET_VOLUME_QUICK);

					global->uart_output_buf[16] = 0;   
		  global->uart_buf_size = 17;
				 global->gw_array[1].online = 1;
				write_socket(global, 1);
				sem_post(&sem_uart_w);
				//sleep(1);
				//LedScreenView("电视音量开启", global->coor_uartfd);
		  printf("Build command end\n");

		}

	      } 
		  else
		  printf("\n *** Haven't match Channel or Volume.\n");

	    }
	break;
    /*
        liqiang add 空调控制模块
        包括:
            空调开关
            空调风速
            空调风向
            空调模式
            空调温度加减指令。
        时间: 2015-1-21 10:09:57
    */
    case CMD_ID_SET_AIRCONDITION:
          printf("CMD_ID_SET_AIRCONDITION\n");
             switch (sk_cmd_ptr->para_3){
                case 1:
                {
                    if(2 == sk_cmd_ptr->para_4){
                        printf("=============>feng xiang\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_FENGXIANG,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);
                    }
                    break;
                }
                case 2:
                {
                    if(1 == sk_cmd_ptr->para_4){
                        printf("==================>feng su\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_FENGSU,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);
                    }
                    break;
                }
                case 3:
                {
                    if(5 == sk_cmd_ptr->para_4){
                        printf("=================>mo shi\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_MOSHI,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);
                    }                   
                    break;
                }
                case 4:
                {
                    if(4 == sk_cmd_ptr->para_4){
                        printf("================>wen du+\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_WENDUJIA,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);
                    }else if(3 == sk_cmd_ptr->para_4){
                        printf("===================>wen du-\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_WENDUJIAN,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);

                    }
                    break;
                }
                case 5:
                {
                    if(1 == sk_cmd_ptr->para_4){
                        printf("=========================>kai guan\n");
                        build_sk_package( global->uart_output_buf,
                                CMD_ID_SET_AIRCONDITION ,
                                CMD_ID_SET_AIR_KAIGUAN,
                                0);
                        global->uart_output_buf[16] = 0;
                        global->uart_buf_size = 17;
                        global->gw_array[1].online = 1;
                        write_socket(global, 1);
                        sem_post(&sem_uart_w);
                    }
                    break;
                }
                default:
                    break;
             }
             break;
  default:
    printf("\n ********** Error occur, Command Dispatch Failed\n");
    break;
  }
  sk_cmd_ptr->no = 0;
}
}


int collection_endpoint_info(char *endpoint_info, int coor_id)
{
  //char *ptr = "hello";
  char cache[2];
  int val_hi, val_low;
 // ptr = endpoint_info;
  //struct gateway_endpoint *gw_ep_ptr;
  // gw_ep_ptr = gw_ep_ins;
  memset(cache, 0, 8);

  global_data_base * glb;
  glb = & global_base;

  char local_time[XS_SIZE];
  int ret, cnt;

#ifdef DEBUG
  printf("Collection endpoint_info = %s\n", endpoint_info);
#endif

  if (('0' == *(endpoint_info+6)) && ('1' == *(endpoint_info+7))) {

    printf("Collection, Hall\n");
    gw_ep_ins[coor_id].hall_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].hall_ins.hall_state = 0;
    else
      gw_ep_ins[coor_id].hall_ins.hall_state = 1;

    // Send Hall to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "01#%d:1:02:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].hall_ins.hall_state);
    printf("Hall Send Central Console data is: %s\n", glb->socket_output_buf);

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
		if (glb->socketfd_in[cnt] > 0){
		      ret = write(glb->socketfd_in[cnt],
				  glb->socket_output_buf,
				  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Hall, Retranslate to CS: %d OK.\n", cnt);
		      }
		}
    }

  } else if (('0' == *(endpoint_info+6)) && ('2' == *(endpoint_info+7))) {

    printf("Collection, PhotoResistor\n");
    gw_ep_ins[coor_id].pr_ins.occur = 1;

    cache[0] = *(endpoint_info+10);
    cache[1] = *(endpoint_info+11);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+12);
    cache[1] = *(endpoint_info+13);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].pr_ins.pr_val = val_hi + val_low;
    printf("gw_ep_ins[%d].pr_ins.pr_val = %d\n",
	   coor_id,
	   gw_ep_ins[coor_id].pr_ins.pr_val);

    // Send PhotoResistor to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "02#%d:2:01:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].pr_ins.pr_val);
    printf("Send Central Console data is: %s\n", glb->socket_output_buf);



    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	     if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("PhotoResistor, Retranslate CS: %d OK.\n", cnt);
		      }
	     }
    }

  } else if (('0' == *(endpoint_info+6)) && ('3' == *(endpoint_info+7))) {

    printf("Collection, TempAndHum\n");
    gw_ep_ins[coor_id].temp_hum_ins.occur = 1;

    cache[0] = *(endpoint_info+10);
    cache[1] = *(endpoint_info+11);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+12);
    cache[1] = *(endpoint_info+13);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].temp_hum_ins.temp_val = ((val_hi << 8) + val_low) / 100.0;
    printf("gw_ep_ins[%d].temp_hum_ins.temp_val = %f\n",
	   coor_id,
	   gw_ep_ins[coor_id].temp_hum_ins.temp_val);

    // Send data to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "03#%d:3:01:%s:%f\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].temp_hum_ins.temp_val);
    printf("TempAndHum, Send Central Console data is: %s\n", glb->socket_output_buf);


    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	     if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Temperature, Retranslate CS: %d OK.\n", cnt);
		      }
	     }
    }

    memset(cache, 0, 8);
    cache[0] = *(endpoint_info+14);
    cache[1] = *(endpoint_info+15);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+16);
    cache[1] = *(endpoint_info+17);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].temp_hum_ins.hum_val = ((val_hi << 8) + val_low) / 100.0;

    printf("gw_ep_ins[%d].temp_hum_ins.hum_val = %f\n",
	   coor_id,
	   gw_ep_ins[coor_id].temp_hum_ins.hum_val);

    // Send Hum to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "03#%d:3:02:%s:%f\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].temp_hum_ins.hum_val);
    printf("Send Central Console data is: %s\n", glb->socket_output_buf);


    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	     if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Hum, Retranslate CS: %d OK.\n", cnt);
		      }
	     }
    }

  } else if (('0' == *(endpoint_info+6)) && ('4' == *(endpoint_info+7))) {

    printf("Collection, Shake\n");

    gw_ep_ins[coor_id].shake_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].shake_ins.shake_state = 0;
    else
      gw_ep_ins[coor_id].shake_ins.shake_state = 1;

    // Send Shake to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "04#%d:4:02:%s:%d\n", coor_id, local_time,
	    gw_ep_ins[coor_id].shake_ins.shake_state);
    printf("Shake Send Central Console data is: %s\n", glb->socket_output_buf);

  

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	      if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Shake, Retranslate CS: %d OK.\n", cnt);
		      }
	      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('5' == *(endpoint_info+7))) {

    printf("Collection, Reed\n");
    gw_ep_ins[coor_id].reed_ins.occur = 1;

    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].reed_ins.reed_state = 0;
    else
      gw_ep_ins[coor_id].reed_ins.reed_state = 1;

    // Send Reed to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "05#%d:5:02:%s:%d\n", coor_id, local_time,
	    gw_ep_ins[coor_id].reed_ins.reed_state);
    printf("Reed, Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	     if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Reed, Retranslate CS: %d OK.\n", cnt);
		      }
	     }
    }

  } else if (('0' == *(endpoint_info+6)) && ('6' == *(endpoint_info+7))) {

    printf("Collection, Accelerate\n");
    gw_ep_ins[coor_id].acc_ins.occur = 1;

    memset(cache, 0, 8);
    cache[0] = *(endpoint_info+10);
    cache[1] = *(endpoint_info+11);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+12);
    cache[1] = *(endpoint_info+13);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].acc_ins.value_x =
      (((val_hi << 8) + val_low) - 1024) * 3.0 / 1024;
    printf("gw_ep_ins[coor_id].acc_ins.value_x = %f\n",
	   gw_ep_ins[coor_id].acc_ins.value_x);

    memset(cache, 0, 8);
    cache[0] = *(endpoint_info+14);
    cache[1] = *(endpoint_info+15);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+16);
    cache[1] = *(endpoint_info+17);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].acc_ins.value_y =
      (((val_hi << 8) + val_low) - 1024) * 3.0 / 1024;
    printf("gw_ep_ins[coor_id].acc_ins.value_y = %f\n",
	   gw_ep_ins[coor_id].acc_ins.value_y);

    memset(cache, 0, 8);
    cache[0] = *(endpoint_info+18);
    cache[1] = *(endpoint_info+19);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+20);
    cache[1] = *(endpoint_info+21);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].acc_ins.value_z =
      (((val_hi << 8) + val_low) - 1024) * 3.0 / 1024;
    printf("gw_ep_ins[coor_id].acc_ins.value_z = %f\n",
	   gw_ep_ins[coor_id].acc_ins.value_z);

    // Send Accelerate X-Y-Z, to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "06#%d:6:02:%s:%f:%f:%f\n",
	    coor_id, local_time,
	    gw_ep_ins[coor_id].acc_ins.value_x,
	    gw_ep_ins[coor_id].acc_ins.value_y,
	    gw_ep_ins[coor_id].acc_ins.value_z);
    printf("Accelerate, Send Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	      if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Accelerate, Retranslate CS: %d OK.\n", cnt);
		      }
	      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('7' == *(endpoint_info+7))) {

    printf("Collection, Smoke\n");
    gw_ep_ins[coor_id].smoke_ins.occur = 1;

    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].smoke_ins.smoke_state = 0;
    else
      gw_ep_ins[coor_id].smoke_ins.smoke_state = 1;

    // Send Smoke to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "07#%d:7:02:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].smoke_ins.smoke_state);
    printf("Smoke Send Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	     if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Smoke, Retranslate CS: %d OK.\n", cnt);
		      }
	     }
    }

  } else if (('0' == *(endpoint_info+6)) && ('8' == *(endpoint_info+7))) {

    printf("Collection, Doppler\n");
    gw_ep_ins[coor_id].doppler_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].doppler_ins.doppler_state = 0;
    else
      gw_ep_ins[coor_id].doppler_ins.doppler_state = 1;

    // Send Doppler to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "08#%d:8:02:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].doppler_ins.doppler_state);
    printf("Doppler Send Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	      if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Doppler, Retranslate CS: %d OK.\n", cnt);
		      }
	      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('9' == *(endpoint_info+7))) {

    printf("Collection, Leds_Motor\n");
    gw_ep_ins[coor_id].leds_motor_ins.occur = 1;

    switch (*(endpoint_info+11)) {

    case '1':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[0] = 1;

      // Send Led 1 On to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:01:%s:%d\n",
	      coor_id,
	      local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[0]);
      printf("Led 1 On, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	if (glb->socketfd_in[cnt] > 0){
		      ret = write(glb->socketfd_in[cnt],
				  glb->socket_output_buf,
				  strlen(glb->socket_output_buf));
		if (ret > 0) {
		  printf("Led 1, Retranslate CS: %d OK.\n", cnt);
		}
	}
      }
      break;

    case '2':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[0] = 0;

      // Send Led 1 Off to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:01:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[0]);
      printf("Led 1 Off, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	if (glb->socketfd_in[cnt] > 0){
		      ret = write(glb->socketfd_in[cnt],
				  glb->socket_output_buf,
				  strlen(glb->socket_output_buf));
		if (ret > 0) {
		  printf("Led 1 Off, Retranslate CS: %d OK.\n", cnt);
		}
	}
      }
      break;

    case '3':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[1] = 1;

      // Send Led 2 On to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:02:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[1]);
      printf("Led 2 On, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0 ; cnt < SOCKETFDLEN; cnt++) {
	if (glb->socketfd_in[cnt] > 0){
		      ret = write(glb->socketfd_in[cnt],
				  glb->socket_output_buf,
				  strlen(glb->socket_output_buf));
		if (ret > 0) {
		  printf("Led 2 On, Retranslate CS: %d OK.\n", cnt);
		}
	}
      }
      break;

    case '4':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[1] = 0;

      // Send Led 2 Off to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:02:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[1]);
      printf("Led 2 Off, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
		  if (glb->socketfd_in[cnt] > 0){
					ret = write(glb->socketfd_in[cnt],
						glb->socket_output_buf,
						strlen(glb->socket_output_buf));

			if (ret > 0) {
			  printf("Led 2 Off, Retranslate CS: %d OK.\n", cnt);
			}
		  }
      }
      break;

    case '5':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[2] = 1;

      // Send Led 3 On to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:03:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[2]);
      printf("Led 3 On, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
		if (glb->socketfd_in[cnt] > 0){
		      ret = write(glb->socketfd_in[cnt],
				  glb->socket_output_buf,
				  strlen(glb->socket_output_buf));
			if (ret > 0) {
			  printf("Led 3 On, Retranslate CS: %d OK.\n", cnt);
			}
		}
      }
      break;

    case '6':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[2] = 0;

      // Send Led 3 Off to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:03:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[2]);
      printf("Led 3 Off, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Led 3 Off, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case '7':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[3] = 1;

      // Send Led 4 On to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:04:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[3]);
      printf("Led 4 On, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Led 4 On, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case '8':
      gw_ep_ins[coor_id].leds_motor_ins.leds_state[3] = 0;

      // Send Led 4 Off to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:04:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.leds_state[3]);
      printf("Led 4 Off, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Led 4 Off, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case '9':
      gw_ep_ins[coor_id].leds_motor_ins.motor_state = 1;

      // Send Motor Left to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:05:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.motor_state);
      printf("Motor Left, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Motor Left, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case 'a':
      gw_ep_ins[coor_id].leds_motor_ins.motor_state = 2;

      // Send Motor Right to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:05:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.motor_state);
      printf("Motor Right, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Motor Right, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case 'b':
      gw_ep_ins[coor_id].leds_motor_ins.motor_state = 0;

      // Send Motor Stop to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:05:%s:%d\n", coor_id, local_time,
	      gw_ep_ins[coor_id].leds_motor_ins.motor_state);
      printf("Motor Stop, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Motor Stop, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case 'c':
      gw_ep_ins[coor_id].leds_motor_ins.leds_all_state = 1;

      // Send Leds All On, to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:00:%s:1111\n", coor_id, local_time);
      printf("Motor Stop, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Leds All On, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    case 'd':
      gw_ep_ins[coor_id].leds_motor_ins.leds_all_state = 0;

      // Send Leds All On, to Central Console.
      get_local_time(local_time);
      memset(glb->socket_output_buf, 0, L_SIZE);
      sprintf(glb->socket_output_buf, "09#%d:9:00:%s:0000\n", coor_id, local_time);
      printf("Motor Stop, Send Central Console data is: %s\n", glb->socket_output_buf);

      
      
      //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
      //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

      for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	ret = write(glb->socketfd_in[cnt],
		    glb->socket_output_buf,
		   strlen(glb->socket_output_buf));
	if (ret > 0) {
	  printf("Leds All On, Retranslate CS: %d OK.\n", cnt);
	}
      }
      break;

    default:
      printf("Error occur, unknown leds info.\n");
      break;
    }

  } else if (('0' == *(endpoint_info+6)) && ('a' == *(endpoint_info+7))) {

    printf("Collection, LED_PWM\n");
    gw_ep_ins[coor_id].led_pwm_ins.occur = 1;

    cache[0] = *(endpoint_info+10);
    cache[1] = *(endpoint_info+11);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    gw_ep_ins[coor_id].led_pwm_ins.pwm_val = val_hi;
    printf("gw_ep_ins[%d].pr_ins.pwm_val = %d\n",
	   coor_id,
	   gw_ep_ins[coor_id].led_pwm_ins.pwm_val);

    // Send PWM to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "0a#%d:10:02:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].led_pwm_ins.pwm_val);
    printf("Pwm, Send Central Console data is: %s\n", glb->socket_output_buf);

    
    
    //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
    //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
      ret = write(glb->socketfd_in[cnt],
		  glb->socket_output_buf,
		 strlen(glb->socket_output_buf));
      if (ret > 0) {
	printf("Pwm, Retranslate Server CS: %d OK.\n", cnt);
      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('b' == *(endpoint_info+7))) {

    printf("Collection, Sound\n");

    gw_ep_ins[coor_id].sound_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].sound_ins.sound_state = 0;
    else
      gw_ep_ins[coor_id].sound_ins.sound_state = 1;

    // Send Sound to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "0b#%d:11:02:%s:%d\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].sound_ins.sound_state);
    printf("Sound Send Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
      ret = write(glb->socketfd_in[cnt],
		  glb->socket_output_buf,
		 strlen(glb->socket_output_buf));
      if (ret > 0) {
	printf("Sound, Retranslate CS: %d OK.\n", cnt);
      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('c' == *(endpoint_info+7))) {

    printf("Collection, Voltage\n");

    gw_ep_ins[coor_id].voltage_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].voltage_ins.voltage_state = 0;
    else
      gw_ep_ins[coor_id].voltage_ins.voltage_state = 1;

  } else if (('0' == *(endpoint_info+6)) && ('d' == *(endpoint_info+7))) {

    printf("Collection, Current\n");

    gw_ep_ins[coor_id].current_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].current_ins.current_state = 0;
    else
      gw_ep_ins[coor_id].current_ins.current_state = 1;

  } else if (('0' == *(endpoint_info+6)) && ('e' == *(endpoint_info+7))) {

    printf("Collection, Touch\n");

    gw_ep_ins[coor_id].touch_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].touch_ins.touch_state = 0;
    else
      gw_ep_ins[coor_id].touch_ins.touch_state = 1;

    // Send Touch to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "0f#%d:12:01:%s:%d\n", coor_id, local_time,
	    gw_ep_ins[coor_id].touch_ins.touch_state);
    printf("Send Central Console data is: %s\n", glb->socket_output_buf);

    
    
    //printf("glb->output_bf_size is: %d.\n",strlen(glb->socket_output_buf));
    //printf("glb->socket_output_buf is: %s.\n", glb->socket_output_buf);

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
      ret = write(glb->socketfd_in[cnt],
		  glb->socket_output_buf,
		 strlen(glb->socket_output_buf));
      if (ret > 0) {
	printf("Retranslate Touch to CS: %d OK.\n", cnt);
      }
    }

  } else if (('0' == *(endpoint_info+6)) && ('f' == *(endpoint_info+7))) {

    printf("Collection, Supwave\n");
    gw_ep_ins[coor_id].supwave_ins.occur = 1;

    cache[0] = *(endpoint_info+10);
    cache[1] = *(endpoint_info+11);
    printf("cache = %s\n", cache);
    val_hi = strhextodec(cache);
    printf("val_hi = %d\n", val_hi);
    cache[0] = *(endpoint_info+12);
    cache[1] = *(endpoint_info+13);
    printf("cache = %s\n", cache);
    val_low = strhextodec(cache);
    printf("val_low = %d\n", val_low);
    gw_ep_ins[coor_id].supwave_ins.sw_val = val_hi + val_low;
    printf("gw_ep_ins[%d].supwave_ins.pr_val = %f\n",
	   coor_id,
	   gw_ep_ins[coor_id].supwave_ins.sw_val);

    // Send SupersonicWave to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "0d#%d:14:01:%s:%f\n",
	    coor_id,
	    local_time,
	    gw_ep_ins[coor_id].supwave_ins.sw_val);
    printf("Send Central Console data is: %s\n", glb->socket_output_buf);

    
    

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
      ret = write(glb->socketfd_in[cnt],
		  glb->socket_output_buf,
		 strlen(glb->socket_output_buf));
      if (ret > 0) {
	printf("Supwave, Retranslate CS: %d OK.\n", cnt);
      }
    }

  } else if (('1' == *(endpoint_info+6)) && ('0' == *(endpoint_info+7))) {

    printf("Collection, RFID_1356\n");

    gw_ep_ins[coor_id].rfid_ins.occur = 1;

  } else if (('1' == *(endpoint_info+6)) && ('1' == *(endpoint_info+7))) {

    printf("Collection, Pyroele\n");

    gw_ep_ins[coor_id].supwave_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].pyr_ins.pyr_state = 0;
    else
      gw_ep_ins[coor_id].pyr_ins.pyr_state = 1;

  }  else if (('1' == *(endpoint_info+6)) && ('2' == *(endpoint_info+7))) {

    printf("Collection, Fire\n");

    gw_ep_ins[coor_id].fire_ins.occur = 1;
    if ('0' == *(endpoint_info+11))
      gw_ep_ins[coor_id].fire_ins.fire_state = 0;
    else
      gw_ep_ins[coor_id].fire_ins.fire_state = 1;

    // Send Shake to Central Console.
    get_local_time(local_time);
    memset(glb->socket_output_buf, 0, L_SIZE);
    sprintf(glb->socket_output_buf, "16#%d:18:02:%s:%d\n", coor_id, local_time,
	    gw_ep_ins[coor_id].fire_ins.fire_state);
    printf("Shake Send Central Console data is: %s\n", glb->socket_output_buf);

  

    for (cnt = 0; cnt < SOCKETFDLEN; cnt++) {
	      if (glb->socketfd_in[cnt] > 0){
			      ret = write(glb->socketfd_in[cnt],
					  glb->socket_output_buf,
					  strlen(glb->socket_output_buf));
		      if (ret > 0) {
			printf("Shake, Retranslate CS: %d OK.\n", cnt);
		      }
	      }
    }

  }
  else {
    printf("%c%c\n", *(endpoint_info+6), *(endpoint_info+7));
    printf("Collection, Reserved\n");
  }

  return 0;
}


int get_local_time(char *str_local_time)
{
	  //char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	  time_t timep;
	  struct tm *t;
	
	  time(&timep);
	  t = localtime(&timep);
	
#if 0
	  printf("%d-%d-%d\n", 1900+t->tm_year, 1+t->tm_mon, t->tm_mday);
	  printf("%d-%d-%d\n", t->tm_hour, t->tm_min, t->tm_sec);
#endif
	
	  sprintf(str_local_time, "%d-%d-%d-%d-%d-%d", 
		  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
		  t->tm_hour, t->tm_min, t->tm_sec);
	
#if 0
	  printf("str_local_time is: %s\n", str_local_time);
#endif
	
	  return 0;
}

int write_coor_uart(int fd, char *uart_cmd, int cmd_len)
  {
	int ret;
	  for (;;) {
		ret = write(fd,
			(void*)uart_cmd,
			cmd_len);
		if (ret > 0 ) {
	  		 printf("write ok %d\n",ret);
	 		 return 0;
		} 
		else {
			  if (errno == EINTR) {
				printf("write EINTR \n");
				continue;
			  } else {
				printf("write: %s\n", strerror(errno));
				 return(-1);
			  }
		}
	  } 
	return 0;
  }

  int strhextodec(char *str_hex)
  {
	int index = 0;
	int sum   = 0;
	int tmp   = 0;
  
	while(str_hex[index]) {
	  switch(str_hex[index]) {
	  case 'A':
	  case 'a':
		tmp = 10;
		break;
	  case 'B':
	  case 'b':
		tmp = 11;
		break;
	  case 'C':
	  case 'c':
		tmp = 12;
		break;
	  case 'D':
	  case 'd':
		tmp = 13;
		break;
	  case 'E':
	  case 'e':
		tmp = 14;
		break;
	  case 'F':
	  case 'f':
		tmp = 15;
		break;
	  default:
		if('0' <= str_hex[index] && str_hex[index] <= '9')
	  tmp = str_hex[index]- '0';
		break;
	  }
	  index++;
	  // accumulate
	  sum = 16 * sum + tmp;
	}
  
	return sum;
  }















