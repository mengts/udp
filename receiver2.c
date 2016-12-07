#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>//
#include <signal.h>
#define MAXLINE 4096
#define BUFFSIZE 1500
#define TOKEN_DELIMITERS " \n"
#define SERV_PORT 7070
void err_sys(const char* x)
{
	perror(x);
	exit(-1);
}
int readable_timeo(int fd, int sec)
{
	fd_set			rset;
	struct timeval	tv;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return(select(fd+1, &rset, NULL, NULL, &tv));
		/* 4> 0 if descriptor is readable */
}
/* end readable_timeo */

int Readable_timeo(int fd, int sec)
{
	int		n;

	if ( (n = readable_timeo(fd, sec)) < 0)
		err_sys("readable_timeo error");
	return(n);
}
void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int			n;
	socklen_t	len;
	FILE *fp;
	fp=fopen("output.txt","w");
	char		mesg[BUFFSIZE];
	int 		checknum=0,final=0;
int finish=0;
	for ( ;!finish ; ) 
	{
		len = clilen;
		n = recvfrom(sockfd, mesg, BUFFSIZE, 0, pcliaddr, &len);
		char count[5];
		strncpy(count,mesg,4);
		if(checknum >= atoi(count) )
		{//correct checknum
			if(checknum == atoi(count))
			{
				fwrite( &mesg[4] , 1 , n-4 , fp);
				checknum++;
			}
			sendto(sockfd, mesg, n, 0, pcliaddr, len);
		}
		else if(atoi(count)==9999)
		{
			
			for(;!finish;)
			{
				if(final==0)
				{
					fwrite( &mesg[4] , 1 , n-4 , fp);
					final = 1;				
				}
				sendto(sockfd, mesg, n, 0, pcliaddr, len);//ack
				if (Readable_timeo(sockfd, 5) == 0) 
				{
					//timeout
					finish=1;
				} 
							
			}//for			
		}//checknum
		
	}//for
	fclose(fp);
}
static void sig_alrm(int signo)
{
	return;			/* just interrupt the recvfrom() */
}
int main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr, cliaddr;

	
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind error");

	dg_echo(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
	close(sockfd);
}
