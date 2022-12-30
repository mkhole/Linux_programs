#include <arpa/inet.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <stdio.h>
#include <stdlib.h>

static int cb(enum nf_conntrack_msg_type type, struct nf_conntrack *ct, void *data)
{
	char buf[1024];
	unsigned int status;

	nfct_snprintf(buf, sizeof(buf), ct, type, NFCT_O_DEFAULT, 0);
	printf("Result: %s\n", buf);

	printf("MSG Type: %d\n", type);
	printf("Timeout: %u\n", nfct_get_attr_u32(ct, ATTR_TIMEOUT));
	status = nfct_get_attr_u32(ct, ATTR_STATUS);

	if (IPS_SRC_NAT & status) {
		printf("IPS_SRC_NAT is set\n");
	}

	if (IPS_DST_NAT & status) {
		printf("IPS_DST_NAT is set\n");
	}

	if (IPS_SRC_NAT_DONE & status) {
		printf("IPS_SRC_NAT_DONE is set\n");
	}

	if (IPS_DST_NAT_DONE & status) {
		printf("IPS_DST_NAT_DONE is set\n");
	}

	return NFCT_CB_CONTINUE;
}

int main(void)
{
	int rc = -1;
	struct nfct_handle *cth = NULL;
	struct nfct_filter *ctflt = NULL;

	cth = nfct_open(CONNTRACK, NFCT_ALL_CT_GROUPS);
	if (!cth) {
		printf("nfct_open() failed\n");
		goto DONE;
	}

	ctflt = nfct_filter_create();
	if (!ctflt) {
		printf("nfct_filter_create() failed\n");
		goto DONE;
	}

	nfct_filter_add_attr_u32(ctflt, NFCT_FILTER_L4PROTO, IPPROTO_ICMP);

	rc = nfct_filter_attach(nfct_fd(cth), ctflt);
	if (rc < 0) {
		printf("nfct_filter_attach() failed\n");
		goto DONE;
	}

	rc = nfct_callback_register(cth, NFCT_T_ALL, cb, NULL);
	if (rc < 0) {
		printf("nfct_callback_register() failed\n");
		goto DONE;
	}

	rc = nfct_catch(cth);

DONE:
	if (ctflt)
		nfct_filter_destroy(ctflt);

	if (cth)
		nfct_close(cth);

	return rc;
}
