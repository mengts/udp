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


void dg_cli(char *str, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
{
	int	n;
	char	buf[BUFFSIZE];
//read the file
	FILE *fp;
	fp=fopen(str,"r+");
	if(!fp)printf("file open fail\n");
	struct stat st;//file size
	stat(str, &st);
	//printf("%s\n",str );
	//char size[25];
	//sprintf(size,"%llds",st.st_size);//the size
	//sendto(sockfd, size, strlen(size), 0, pservaddr, servlen);
	int fd;
	fd=fileno(fp);
	int j;
	
	for(j=0;j<st.st_size;)
	{
		int num;
		num=read(fd,buf,BUFFSIZE);
		j=j+num;printf("send%d\n",num);
		sendto(sockfd, buf, num, 0, pservaddr, servlen);
	}
	fclose(fp);//close the file
	/*while (fgets(sendline, MAXLINE, fp) != NULL) {

		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
	}*/
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
