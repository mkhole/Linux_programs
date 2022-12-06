#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>

static int custom_func(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	int nl_len = nlmsg_get_max_size(msg);

	for (; nlmsg_ok(nlh, nl_len); nlh = nlmsg_next(nlh, &nl_len)) {
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

		struct ifaddrmsg *ifah = nlmsg_data(nlh);
		printf("ifa_family: %d, ifa_index: %d\n", ifah->ifa_family,
		       ifah->ifa_index);

		struct nlattr *nla[__IFA_MAX];
		nlmsg_parse(nlh, sizeof(struct ifaddrmsg), nla, __IFA_MAX - 1,
			    NULL);

		if (!nla[IFA_LOCAL])
			return NL_SKIP;

		struct in_addr addr;
		nla_memcpy(&addr, nla[IFA_LOCAL], sizeof(addr));

		char buf[INET6_ADDRSTRLEN] = {};
		inet_ntop(AF_INET, &addr, buf, sizeof(buf));
		printf("address: %s\n", buf);
	}

	return NL_OK;
}

int main(void)
{
	struct nl_sock *nlsk = nl_socket_alloc();

	nl_socket_disable_seq_check(nlsk);

	nl_socket_modify_cb(nlsk, NL_CB_VALID, NL_CB_CUSTOM, custom_func, NULL);

	nl_connect(nlsk, NETLINK_ROUTE);

	nl_socket_add_memberships(nlsk, RTNLGRP_IPV4_IFADDR, 0);

	while (1)
		nl_recvmsgs_default(nlsk);

	nl_socket_free(nlsk);
	return 0;
}
