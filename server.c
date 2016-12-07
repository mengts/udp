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

void err_sys(const char* x)
{
	perror(x);
	exit(-1);
}
ssize_t readn(int fd, void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr=vptr;
	nleft=n;
	while (nleft>0) {
		if((nread=read(fd,ptr,nleft))<0)
		{
			if(errno==EINTR)
				nread=0;
			else
			 return (-1);
		}else if(nread==0)
			break;
		nleft-=nread;
		ptr+=nread;
	}
	return(n-nleft);
}
ssize_t writen(int fd, const void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr=vptr;
	nleft=n;
	while (nleft>0) {
		if((nwritten=write(fd,ptr,nleft))<0)
		{
			if(nwritten<0&&errno==EINTR)
				nwritten=0;
			else return(-1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return(n);
}

void Writen(int fd,void *ptr,size_t nbytes)
{
	if(writen(fd,ptr,nbytes)!=nbytes)
		err_sys("writen error");
}
ssize_t readline(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;
	ptr=vptr;
	for(n=1;n<maxlen;n++)
	{
again:
			if((rc=read(fd,&c,1))==1)
			{
				*ptr++=c;
				if(c=='\n')
				{
					break;
				}
			}else if (rc==0) {
				*ptr=0;
				return(n-1);
			}else{
				if(errno==EINTR)
					goto again;
				return(-1);
			}
	}
	*ptr=0;
	return(n);
}
ssize_t readc(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr,last;
	ptr=vptr;
	last='a';
	for(n=0;n<maxlen;n++)
	{
again:
			if((rc=read(fd,&c,1))==1)
			{
				//printf("%c ",c );
				*ptr++=c;
				if(c=='T'&&last=='t')
				{
						return(n-1);//
				}
				last=c;
			}else if (rc==0) {
				*ptr=0;
				return(n-1);
			}else{
				if(errno==EINTR)
					goto again;
				return(-1);
			}
	}
	//*ptr=0;//no need ending
	return(n);
}
ssize_t Readline(int fd,void *ptr,size_t maxlen)
{
	ssize_t n;
	if((n=readline(fd,ptr,maxlen))<0)
		err_sys("readline error");
	return(n);
}
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
	int n;
	again:
			if((n=accept(fd,sa,salenptr))<0)
			{
	#ifdef EPROTO
				if(errno==EPROTO||errno==ECONNABORTED)
	#else
				if(errno==ECONNABORTED)
	#endif
						goto again;
				else
						err_sys("accept error");
			}
			return (n);
}
//after connect to client
void str_echo(int sockfd)//
{//printf("goin88\n");
	int filenum=0;
	char filename[10][20];
	ssize_t n;//printf("goi111n\n");
	char buf[BUFFSIZE];//
	char command[50];
	char another[50];
	while(1)
	{
//printf("wating to  read\n");
		if((n=Readline(sockfd,command,50))==0)
		{//client close
			//close(sockfd);
			return;
			//exit(-1);
		}
		char *str=NULL;
		str=strtok(command,TOKEN_DELIMITERS);////////-------
		if(strcmp(str,"LIST")==0)
		{//printf("goin1\n");
			char send=filenum;
			write(sockfd,(char*)&send,1);
			str=strtok(NULL,TOKEN_DELIMITERS);
			if(filenum==0)
			{
				//strcpy(another,"it's empty\n");
				//Writen(sockfd,another,strlen(another));
			}
			else
			{
				int i;//sprintf(another,"%s",filename[0]);
				for(i=0;i<filenum;i++)
				{
					sprintf(another,"%s\n",filename[i]);
					Writen(sockfd,another,strlen(another));
				}//for
			}
		}//list
		else if(strcmp(str,"PUT")==0)
		{//printf("get\n" );
			str=strtok(NULL,TOKEN_DELIMITERS);//f1
			if(str==NULL)//printf("wrong\n", );
				break;
			else
			{
				str=strtok(NULL,TOKEN_DELIMITERS);//f2
				if(str==NULL)
					break;
				else
				{
					int flag=0;//file name exist or not
					if(filenum==0)flag=0;
					else
					{
						int k;
						for(k=0;k<filenum;k++)
						{
							if(strcmp(str,filename[k])==0)
							{
								flag=1;
								break;
							}
						}
					}//check the file exist or not
					if(flag==0)//file doesn't exist
					{
						strcpy(filename[filenum],str);
						filenum++;
					}
					char size[25],chr;
					int p;//printf("hig" );
					/*for(p=0;p<25;p++)
					{
						read(sockfd,(char*)&chr,1);
						if(chr=='s')
							break;
						size[p]=chr;
					}size[p]='\0';*/
					int cc;
					//cc=read(sockfd,buf,BUFFSIZE);
					//printf("%d\n",cc );int big;
					//sscanf(buf,"%ds",&big);
					
					//big=atoi(size);
					FILE *fp;
					fp=fopen(str,"w");
					if(!fp)printf("file open fail\n");
					int j;int fd=fileno(fp);
					//printf("this\n");
					//n=readc(sockfd,buf,BUFFSIZE);
			//read the databig
					for(j=0;;)
					{
						int n,checknum;
						char temp[BUFFSIZE];//,wstring[BUFFSIZE-4];
						n=read(sockfd,temp,BUFFSIZE);
					printf("hi\n");
						//sscanf(temp,"%04d%s",&checknum,wstring);
						//printf("%04d\n",checknum);
						char count[5];
						strncpy(count,temp,4);
					printf("%d\n", atoi(count));
						
						if(n < 0)
						{
							printf("Read error\n");
							break;
						}
						//printf("%d\n",n);
					printf("%d\n", n);
						fwrite(&temp[4],1,n-4,fp);
						j=j+n-4;if(atoi(count)==9999)break;
						
					}//end of read data
					printf("end\n" );
					
					fclose(fp);
					
					//fwrite(buf,sizeof(char),n,fp);
				}
			}
		}//put
		else if(strcmp(str,"GET")==0)
		{
			
			str=strtok(NULL,TOKEN_DELIMITERS);//f1
			FILE *fp;
			fp=fopen(str,"r");
			if(!fp)printf("file open fail\n");
			struct stat st;//file size
			stat(str, &st);
			char ch;
			int j;
			char size[25];
			sprintf(size,"%llds",st.st_size);
			Writen(sockfd,size,strlen(size));
			for(j=0;j<st.st_size;j++)
			{
				ch=fgetc(fp);
				write(sockfd,(char*)&ch,1);
			}
			
			fclose(fp);
			//Writen(sockfd,longstr,sizeof(char)*(j+2));
		}//GET!!

		memset(buf,'\0',sizeof(buf));
	}
}
//
void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while((pid=waitpid(-1, &stat, WNOHANG))>0)
		printf("Client has closed the connection.\n");
	
	return;
}
int main(int argc,char *argv[])
{
	if(argc<2)
		err_sys("./server [port]");
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family =AF_INET;
	servaddr.sin_addr.s_addr =htonl(INADDR_ANY);
	servaddr.sin_port =htons(atoi(argv[1]));

	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind error");

	if(listen(listenfd,15)==-1)
		err_sys("listen error");
	void sig_chld(int);
	signal(SIGCHLD, sig_chld);//must call waitpid()
//concurrent server
	pid_t pid;

	for(;;)
	{

		len=sizeof(cliaddr);
		//printf("goin\n");
		connfd=accept(listenfd, (struct sockaddr *)&cliaddr,&len);
//connect to the client
		if((pid=fork())==0)
		{//child process
			close(listenfd);
			str_echo(connfd);
			
			exit(0);
		}
		//printf("goinww\n");
		close(connfd);
	}

/*char sendline[MAXLINE],recvline[MAXLINE];
	strcpy(sendline,"hi\n");
	FILE *fp;
	fp=fopen(argv[1],"w");
	fputs(sendline,fp);
	printf("%s\n",sendline);*/
return 0;

}
