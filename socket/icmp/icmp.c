#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

static unsigned short chksum(unsigned short *buf, int sz)
{
	unsigned int sum = 0;
	unsigned short carry;
	unsigned short *w = buf;

	while (sz > 1) {
		sum += *w++;
		sz -= 2;
	}

	if (sz == 1) {
		sum += *(unsigned char *)w;
	}

	carry = sum >> 16;
	while (carry) {
		sum = (sum & 0xFFFF) + carry;
		carry = sum >> 16;
	}

	return (unsigned short)(~sum);
}

static int send_icmp(int fd, struct sockaddr_in *daddr)
{
	ssize_t bytes;
	char packet[sizeof(struct icmphdr) + sizeof(struct timespec)] = {};
	struct icmphdr *icmph = (struct icmphdr *)packet;
	struct timespec *t;

	icmph->type = ICMP_ECHO;
	icmph->code = 0;
	icmph->checksum = 0;
	icmph->un.echo.id = 1;
	icmph->un.echo.sequence = getpid() & 0xFFFF;

	t = (struct timespec *)(packet + sizeof(struct icmphdr));
	if (clock_gettime(CLOCK_MONOTONIC, t) < 0) {
		printf("Failed to get time\n");
		return -1;
	}

	icmph->checksum = chksum((unsigned short *)packet, sizeof(packet));

	bytes = sendto(fd, packet, sizeof(packet), 0, (struct sockaddr *)daddr,
		       sizeof(struct sockaddr_in));

	if (bytes < sizeof(packet)) {
		printf("Failed to send packet\n");
		return -1;
	}

	printf("Send ICMP id: %u, seq: %u\n", icmph->un.echo.id,
	       icmph->un.echo.sequence);

	printf("Send timestamp: %lu, %lu\n", t->tv_sec, t->tv_nsec);
	return 0;
}

static int rcv_icmp(int fd)
{
	ssize_t bytes;
	unsigned int addrlen;
	struct sockaddr_in addr = {};
	char packet[1024] = {};
	struct iphdr *iph = (struct iphdr *)packet;
	struct icmphdr *icmph;
	struct timespec nt, *ot;

	addrlen = sizeof(struct sockaddr_in);
	bytes = recvfrom(fd, packet, sizeof(packet), 0,
			 (struct sockaddr *)&addr, &addrlen);

	if (bytes <= 0) {
		printf("Failed to receive packet\n");
		return -1;
	}

	icmph = (struct icmphdr *)(packet + (iph->ihl << 2));
	printf("Receive ICMP type: %d, code: %d\n", icmph->type, icmph->code);

	if (icmph->type != ICMP_ECHOREPLY || icmph->code != 0)
		return -1;

	printf("Receive ICMP id: %u, seq: %u \n", icmph->un.echo.id,
	       icmph->un.echo.sequence);

	if (clock_gettime(CLOCK_MONOTONIC, &nt) < 0) {
		printf("Failed to get time\n");
		return -1;
	}

	printf("Receive timestamp: %lu, %lu\n", nt.tv_sec, nt.tv_nsec);

	ot = (struct timespec *)((char *)icmph + sizeof(struct icmphdr));

	unsigned long time = (nt.tv_sec - ot->tv_sec) * 1000 +
			     (nt.tv_nsec - ot->tv_nsec) / 1000;

	printf("RTT: %.3lf ms\n", (double)time / 1000);

	return 0;
}

int main(int argc, char *argv[])
{
	int opt, fd;
	char ifname[16] = {};
	struct sockaddr_in saddr = {};
	struct sockaddr_in daddr = {};

	while ((opt = getopt(argc, argv, "i:b:c:")) != -1) {
		switch (opt) {
		case 'i':
			strncpy(ifname, optarg, sizeof(ifname));
			break;
		case 'b':
			if (inet_pton(AF_INET, optarg, &saddr.sin_addr) != 1) {
				printf("Failed to parse IP address\n");
				return -1;
			}
			break;
		case 'c':
			if (inet_pton(AF_INET, optarg, &daddr.sin_addr) != 1) {
				printf("Failed to parse IP address\n");
				return -1;
			}
			break;
		default:
			break;
		}
	}

	if (!daddr.sin_addr.s_addr) {
		printf("No destination address specified\n");
		return -1;
	}

	fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (fd < 0) {
		printf("Failed to create socket\n");
		return -1;
	}

	if (saddr.sin_addr.s_addr) {
		if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr))) {
			printf("Failed to bind source address\n");
			return -1;
		}
	}

	if (ifname[0]) {
		if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname,
			       strlen(ifname) + 1)) {
			printf("Failed to bind interface\n");
			return -1;
		}
	}

	if (send_icmp(fd, &daddr) < 0)
		return -1;

	if (rcv_icmp(fd) < 0)
		return -1;

	close(fd);
	return 0;
}

