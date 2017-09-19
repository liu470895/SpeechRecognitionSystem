#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <string.h>

#define BUFF_SIZE 128

void usage(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s <PORT>.\n", argv[0]);
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

	int on = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));

	struct sockaddr_in srv;
	int srv_len = sizeof srv;
	bzero(&srv, srv_len);

	/*设置本地IP地址和端口参数*/
	srv.sin_family = AF_INET;
	srv.sin_port = htons(atoi(argv[1]));
	srv.sin_addr.s_addr = htonl(INADDR_ANY);

	int err = bind(sfd, (struct sockaddr*)&srv, srv_len);
	if(err == -1)
	{
		fprintf(stderr, "bind() PORT:%s error.\n", argv[1]);
		exit(1);
	}

	err = listen(sfd, 3);
	if(err == -1)
	{
		perror("listen() error");
		exit(1);
	}

	struct sockaddr_in cli;
	socklen_t cli_len;
	bzero(&cli, sizeof cli);

	printf("watting someboby connect.\n");
	int cfd = accept(sfd, (struct sockaddr*)&cli, &cli_len);
	//int cfd = accept(sfd, NULL, NULL);
	if(cfd == -1)
	{
		perror("accept() error");
		exit(1);
	}

	printf("%s connect success\n", inet_ntoa(cli.sin_addr));

	int n;
	char *buf = (char *)malloc(BUFF_SIZE);
	char *msg = "Hello";
	while(1)
	{
		write(cfd, msg, strlen(msg));	
		bzero(buf, BUFF_SIZE);
		n = read(cfd, buf, BUFF_SIZE-1);
		if(n == 0)
		{
			break;
		}
		printf("read %dbytes:%s\n", n, buf);
		sleep(1);
	}
	return 0;
}
