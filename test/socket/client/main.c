#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFF_SIZE 128

void usage(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s <IP> <PORT>\n", argv[0]);
		exit(1);
	}
}
int main(int argc, char *argv[])
{
	usage(argc, argv);

	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("socket() error");
		exit(1);
	}
	
	struct sockaddr_in srv;
	int srv_len = sizeof srv;
	bzero(&srv, srv_len);

	/*转换服务器的ip地址为网络字节序*/
	srv.sin_family = AF_INET;
	srv.sin_port = htons(atoi(argv[2])); 
	srv.sin_addr.s_addr = inet_addr(argv[1]); //172.xx.xx.xx

	int err = connect(sfd, (struct sockaddr*)&srv, srv_len);
	if(err == -1)
	{
		fprintf(stderr, "connet() to %s:%s error", argv[1], argv[2]);
		perror(NULL);
		exit(1);
	}

	
	int n;
	char *buf = (char *)malloc(BUFF_SIZE);
	while(1)
	{
		bzero(buf, BUFF_SIZE);
		n = read(sfd, buf, BUFF_SIZE-1);
		if(n == 0)
		{
			break;
		}
		printf("read %dbytes:%s\n", n, buf);
		sleep(1);
		write(sfd, buf, n);
	}
	

	close(sfd);

	return 0;
}
