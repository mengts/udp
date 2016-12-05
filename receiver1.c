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
#define BUFFSIZE 81923
#define TOKEN_DELIMITERS " \n"
#define SERV_PORT 7070
void err_sys(const char* x)
{
	perror(x);
	exit(-1);
}
static void sig_alrm(int);
void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
	int			n;
	socklen_t	len;
	//char		mesg[MAXLINE];

	signal(SIGALRM, sig_alrm);
	siginterrupt(SIGALRM,1);
//create file	
	char str[20],buf[BUFFSIZE];
	strcpy(str, "output.txt");

	FILE *fp;
	fp=fopen(str,"w");
	if(!fp)printf("file open fail\n");
	int j;
	int fd=fileno(fp);
	char size[25],chr;
	int p;
	printf("keep\n");
	/*for(p=0;p<25;p++)
	{
		recvfrom(sockfd, (char*)&chr, 1, 0, pcliaddr, &len);
		if(chr=='s')
			break;
		size[p]=chr;
	}size[p]='\0';
	printf("%s\n",size );
	int big;
	big=atoi(size);*/


	for ( ; ; ) {
		alarm(5);
		len = clilen;
		if ( (n = recvfrom(sockfd, buf, BUFFSIZE, 0, pcliaddr, &len)) < 0) {
			if (errno == EINTR)
				fprintf(stderr, "socket timeout\n");
			else
				err_sys("recvfrom error");
		} else {
			alarm(0);
			//mesg[n] = 0;	/* null terminate */
			//fputs(mesg, stdout);
			printf("get it%d\n",n);
			j=j+n;
			write(fd,buf,n);
			
		}
		if(n==0)break;
		//n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

	}
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
