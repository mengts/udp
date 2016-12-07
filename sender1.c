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

static void sig_alrm(int);
void dg_cli(char *str, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
{
	
//read the file
	FILE *fp;
	fp=fopen(str,"r+");//if(!fp)printf("file open fail\n");
		
//set the signal
	signal(SIGALRM, sig_alrm);
	siginterrupt(SIGALRM,1);
	
	int fd,j,n;
	fd=fileno(fp);
	
int checknum=0,ack=1,final=0;	
	for(;;)
	{
		if(final == 1 && ack == 1)
			break;

		char ww[5];//change the j
		char	buf[BUFFSIZE],rec[BUFFSIZE];
		int num;
		if(ack==1)//receive the ack & read the new file & make the buffer
		{
			num=fread(&buf[4],sizeof(char),BUFFSIZE-4,fp);	
			if(feof(fp))
			{//the end of file num<BUFFSIZE-4)
				checknum=9999;	
				final=1;
			}	
			sprintf(ww,"%04d",checknum);
			strncpy(buf,ww,4);//buf[0~3]checknum
			ack=0;
		}	
		
		sendto(sockfd, buf, num+4, 0, pservaddr, servlen);

		alarm(5);
		
		if ( (n = recvfrom(sockfd, rec, BUFFSIZE, 0, NULL, NULL)) < 0) 
		{
			if (errno == EINTR)
			{//timeout
				//fprintf(stderr, "socket timeout\n");
				ack=0;// resend the buf
			}
			else
				err_sys("recvfrom error");
		} 
		else 
		{//receive the ack
			alarm(0);
			//check the checknum buf[0~3]
			char count[5];
			strncpy(count,rec,4);
			if(checknum == atoi(count))
			{
				ack = 1;//correct checknum
				if(final == 0)
					checknum++;

			}		
		}//recv end		

	}//for end
	fclose(fp);//close the file	
}

static void sig_alrm(int signo)
{
	return;			/* just interrupt the recvfrom() */
}


int main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 3)
		err_sys("usage: udpcli <IPaddress> [file_name]");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	//open the file
	//FILE *fp;
	//fp=fopen(argv[2],"r");
	//if(!fp)printf("file open fail\n");
	
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket error");

	dg_cli(argv[2], sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	close(sockfd);
	exit(0);
}
