#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCK "./un_socket"

int recv_fd(int sock_fd)
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

	if (recvmsg(sock_fd, &msg, 0) < 0) {
		printf("Failed to recvmsg\n");
		return -1;
	}

	cmsg = CMSG_FIRSTHDR(&msg);
	return  *((int *)CMSG_DATA(cmsg));
}

int main(void)
{
	int rc;
	int fd;
	int server_sock;
	struct sockaddr_un addr;
	char buf[32];

	server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sock < 0) {
		printf("Failed to open socket\n");
		goto DONE;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SERVER_SOCK, sizeof(addr.sun_path) - 1);

	rc = connect(server_sock, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		printf("Failed to connect socket\n");
		goto DONE;
	}

	fd = recv_fd(server_sock);
	if (fd < 0) {
		printf("Failed to receive fd\n");
		goto DONE;
	}

	memset(buf, 0, sizeof(buf));
	if (read(fd, buf, sizeof(buf)) < 0) {
		printf("Failed to read from fd\n");
		goto DONE;
	}
	printf("%s\n", buf);

DONE:
	if (server_sock)
		close(server_sock);

	if (fd)
		close(fd);

	return 0;
}
