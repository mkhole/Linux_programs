#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCK "./un_socket"

int send_fd(int sock, int fd)
{
	struct msghdr msg;
	struct iovec iov;
	struct cmsghdr *cmsg = NULL;
	char cmsgbuf[CMSG_SPACE(sizeof(int))];
	char data;

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);

	data = '\0';
	iov.iov_base = &data;
	iov.iov_len = sizeof(data);

	memset(cmsgbuf, 0, sizeof(cmsgbuf));
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*((int *)CMSG_DATA(cmsg)) = fd;

	return sendmsg(sock, &msg, 0);
}

int main(void)
{
	int rc;
	int fd;
	int server_sock, client_sock;
	struct sockaddr_un addr;

	server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sock < 0) {
		printf("Failed to open socket\n");
		goto DONE;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SERVER_SOCK, sizeof(addr.sun_path) - 1);
	unlink(SERVER_SOCK);

	rc = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		printf("Failed to bind socket\n");
		goto DONE;
	}

	rc = listen(server_sock, 8);
	if (rc < 0) {
		printf("Failed to listen socket\n");
		return -1;
	}

	for (;;) {
		client_sock = accept(server_sock, NULL, 0);
		if (client_sock < 0) {
			printf("Failed to accept socket\n");
			continue;
		}

		fd = open("./HelloWorld", O_RDONLY);
		if (fd < 0) {
			printf("Failed to open file\n");
			goto DONE;
		}

		send_fd(client_sock, fd);
		close(client_sock);
		close(fd);
	}

DONE:
	if (server_sock)
		close(server_sock);

	if (fd)
		close(fd);

	return 0;
}
