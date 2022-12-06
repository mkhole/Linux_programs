#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <errno.h>

static void nl_parse(void *msg)
{
	struct nlmsghdr *nlh = msg;
	int nl_len = nlh->nlmsg_len;

	for (; NLMSG_OK(nlh, nl_len); nlh = NLMSG_NEXT(nlh, nl_len)) {
		switch (nlh->nlmsg_type) {
		case RTM_NEWADDR:
			printf("NEWADDR\n");
			break;
		case RTM_DELADDR:
			printf("DELADDR\n");
			break;
		default:
			printf("nlmsg_type: %d\n", nlh->nlmsg_type);
			continue;
		}

		struct ifaddrmsg *ifah = NLMSG_DATA(nlh);
		printf("ifa_family: %d, ifa_index: %d\n", ifah->ifa_family,
		       ifah->ifa_index);

		struct rtattr *rtah = IFA_RTA(ifah);
		int rta_len = IFA_PAYLOAD(nlh);

		for (; RTA_OK(rtah, rta_len); rtah = RTA_NEXT(rtah, rta_len)) {
			if (rtah->rta_type != IFA_LOCAL)
				continue;

			char buf[INET6_ADDRSTRLEN] = {};
			inet_ntop(ifah->ifa_family, RTA_DATA(rtah), buf,
				  sizeof(buf));
			printf("address: %s\n", buf);
		}
	}
}

static void nl_recv(int fd)
{
	ssize_t bytes;
	char buf[4096];
	struct sockaddr_nl addr = {};

	struct iovec iov = {
		.iov_base = (void *)buf,
		.iov_len = sizeof(buf),
	};
	struct msghdr msg = {
		.msg_name = (void *)&addr,
		.msg_namelen = sizeof(addr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};

	bytes = recvmsg(fd, &msg, 0);

	if (bytes <= 0)
		return;

	nl_parse(&buf);
}

static int nl_socket_init(int protocol, int groups)
{
	int fd;
	int ret;
	struct sockaddr_nl saddr = {};

	fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (fd < 0)
		goto err;

	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = 0;
	saddr.nl_groups = groups;

	ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret)
		goto err;

	return fd;
err:
	if (fd > 0)
		close(fd);

	return -1;
}

int main(void)
{
	int skfd = nl_socket_init(NETLINK_ROUTE, RTMGRP_IPV4_IFADDR);

	if (skfd < 0)
		return -1;

	while (1)
		nl_recv(skfd);

	close(skfd);
	return 0;
}
