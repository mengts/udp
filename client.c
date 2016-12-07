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
#include <unistd.h>
#include <stdint.h>
#define MAXLINE 4096
#define BUFFSIZE 1500
#define TOKEN_DELIMITERS " \n"
void err_sys(const char* x)
{
	perror(x);
	exit(-1);
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

ssize_t Readline(int fd,void *ptr,size_t maxlen)
{
	ssize_t n;
	if((n=readline(fd,ptr,maxlen))<0)
		err_sys("readline error");
	return(n);
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
void Writen(int fd,void *ptr,size_t nbytes)
{
	if(writen(fd,ptr,nbytes)!=nbytes)
		err_sys("writen error");
}

void str_cli(FILE *fp,int sockfd)
{
		char sendline[MAXLINE],recvline[MAXLINE];//sendline=command
		char command[50];
		//int numstr=0;
		while(fgets(sendline,MAXLINE,fp)!=NULL)
		{//GET f1 f2
		//	Writen(sockfd,sendline,strlen(senline));

			if(strcmp(sendline,"EXIT\n")==0)
			{
				close(sockfd);
				exit(-1);
			}
			
			char *str=NULL;
			strcpy(command,sendline);
			Writen(sockfd,command,strlen(command));
			str=strtok(sendline,TOKEN_DELIMITERS);
			if(strcmp(str,"LIST")==0)
			{
					//Writen(sockfd,command,strlen(command));
					int j,numstr;
					char send;
					read(sockfd,(char*)&send,1);
					numstr=send;
					if(numstr==0)
					{
						/*if(Readline(sockfd,recvline,MAXLINE)==0)
								err_sys("str_cli:server terminated");
							fputs(recvline,stdout);
							memset(recvline,'\0',sizeof(recvline));*/
					}
					else
					{
						for(j=0;j<numstr;j++)
						{
							if(Readline(sockfd,recvline,MAXLINE)==0)
								err_sys("str_cli:server terminated");
							fputs(recvline,stdout);
							memset(recvline,'\0',sizeof(recvline));
							//clean out
						}
					}
					
						printf("List succeeded\n");
			//	}
			}//list
			else if(strcmp(str,"PUT")==0)
			{
				//numstr++;
				//Writen(sockfd,command,strlen(command));
					str=strtok(NULL,TOKEN_DELIMITERS);//f1
					if(str==NULL)printf("wrong input\n" );
					else
					{
						FILE *fp;
						fp=fopen(str,"r");
						if(!fp)printf("file open fail\n");
						struct stat st;//file size
						stat(str, &st);
						//size = st.st_size;
						str=strtok(NULL,TOKEN_DELIMITERS);//f2
						if(str==NULL)
						{
							printf("wrong input\n");fclose(fp);
						}
						else
						{
						//begin read the file
							char ch;//read the file
							int j;
							//printf("%lld\n",st.st_size );
							//char size[25];
							//sprintf(size,"%llds",st.st_size);
							//Writen(sockfd,size,strlen(size));
							/*int fd=0;
							fd=fileno(fp);
							j=read(fd,buf,BUFFSIZE);*/
							//int fd;
							//fd=fileno(fp);j<st.st_size!feof(fp)
							j=0;
							for(;!feof(fp);)
							{
								char buf[BUFFSIZE];
								char ww[5];//change the j
								//char temp[BUFFSIZE+6];
								
								//ch=fgetc(fp);
								int num;
								num=fread(&buf[4],sizeof(char),BUFFSIZE-4,fp);
								//j=j+num;
								printf("%d\n", num);
								if(num<BUFFSIZE-4)j=9999;
								sprintf(ww,"%04d",j);
								strncpy(buf,ww,4);
								
								//sprintf(temp,"%04d%s",j,buf);
								//printf("%04d\n",j);
								write(sockfd,buf,num+4);
								j++;
							}

							//printf("%lld\n",j );
							printf("end\n");
							fclose(fp);//close the file
							
						}
					}
			}//get
			else if(strcmp(str,"GET")==0)
			{
				//Writen(sockfd,command,strlen(command));
				str=strtok(NULL,TOKEN_DELIMITERS);//f1
				str=strtok(NULL,TOKEN_DELIMITERS);//f2
				FILE *fp;
				fp=fopen(str,"w+");
				if(!fp)printf("file open fail\n");
				int j;
				int fd=fileno(fp);
				//char buf[BUFFSIZE];
				char size[25],chr;
				int p;
				for(p=0;p<25;p++)
				{
					read(sockfd,(char*)&chr,1);
					if(chr=='s')
						break;
					size[p]=chr;
				}size[p]='\0';
				int big;
				big=atoi(size);
				for(j=0;j<big;j++)
				{
					int n;
					n=read(sockfd,(char*)&chr,1);
					if(n < 0)
					{
						printf("Read error\n");
						break;
					}
					//printf("%d\n",n);
					
					write(fd,(char*)&chr,1);
					
				}//end of read data

				command[strlen(command)-1]='\0';
				printf("%s succeeded\n",command );
		
				
			}//get

			memset(sendline,'\0',sizeof(sendline));
			
			//memset(buf,'\0',sizeof(buf));
		}//end of while

}

int main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		err_sys("socket error");

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family =AF_INET;
		servaddr.sin_port =htons(atoi(argv[2]));
		connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
		str_cli(stdin,sockfd);
/*char sendline[MAXLINE],recvline[MAXLINE];
	FILE *fp;
	fp=fopen(argv[1],"r");
	fgets(sendline,MAXLINE,fp);
	printf("%s\n",sendline);*/
return 0;

}
