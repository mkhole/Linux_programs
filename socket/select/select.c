#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	int sockfd, maxfd;
	fd_set rfds;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("socket() failed\n");
		return -1;
	}
	maxfd = sockfd + 1;

	const int flag = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {
		printf("setsockopt() failed\n");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8888);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))) {
		printf("bind() failed\n");
		return -1;
	}

	if (listen(sockfd, SOMAXCONN)) {
		printf("listen() failed\n");
		return -1;
	}

	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);

	while (1) {
		int ret;
		fd_set fds = rfds;

		ret = select(maxfd + 1, &fds, NULL, NULL, NULL);
		if (ret < 0) {
			printf("select() failed\n");
			continue;
		}

		for (int fd = 0; fd <= maxfd; fd++) {
			if (!FD_ISSET(fd, &fds))
				continue;

			if (fd == sockfd) {
				int cfd;
				socklen_t len = sizeof(struct sockaddr_in);
				struct sockaddr_in caddr;

				cfd = accept(sockfd, (struct sockaddr *)&caddr,
					     &len);
				if (cfd < 0) {
					printf("accept() failed\n");
					continue;
				}

				printf("Accept (%d): %s:%d\n", cfd,
				       inet_ntoa(caddr.sin_addr),
				       ntohs(caddr.sin_port));

				FD_SET(cfd, &rfds);
				maxfd = cfd >= maxfd ? cfd + 1 : maxfd;

			} else {
				int len;
				char buff[256] = {};

				len = recv(fd, buff, sizeof(buff), 0);

				if (len <= 0) {
					printf("recv() failed\n");
				} else {
					printf("Receive: %s\n", buff);
					send(fd, "Hello\n", 6, 0);
				}

				close(fd);
				FD_CLR(fd, &rfds);
			}
		}
	}

	return 0;
}
