#include <arpa/inet.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <stdio.h>
#include <stdlib.h>

static int cb(enum nf_conntrack_msg_type type,
			  struct nf_conntrack *ct,
			  void *data)
{
	char buf[1024];
	struct nf_conntrack *obj = data;

	if (!nfct_cmp(obj, ct, NFCT_CMP_ORIG | NFCT_CMP_MASK))
		return NFCT_CB_CONTINUE;

	nfct_snprintf(buf, sizeof(buf), ct, type, NFCT_O_DEFAULT, 0);
	printf("Result: %s\n", buf);
	printf("Mark: %u\n", nfct_get_attr_u32(ct, ATTR_MARK));

	return NFCT_CB_CONTINUE;
}

int main(void)
{
	int rc = -1;
	int family = AF_INET;
	struct nf_conntrack *ct;
	struct nfct_handle *cth;

	ct = nfct_new();
	if (!ct) {
		printf("nfct_new() failed\n");
		goto DONE;
	}

	nfct_set_attr_u8(ct, ATTR_L3PROTO, AF_INET);
	nfct_set_attr_u8(ct, ATTR_ORIG_L4PROTO, IPPROTO_UDP);
	nfct_set_attr_u16(ct, ATTR_ORIG_PORT_DST, htons(53));

	cth = nfct_open(CONNTRACK, NFNL_SUBSYS_CTNETLINK);
	if (!cth) {
		printf("nfct_open() failed\n");
		goto DONE;
	}

	nfct_callback_register(cth, NFCT_T_ALL, cb, ct);
	if (nfct_query(cth, NFCT_Q_DUMP, &family) == -1) {
		printf("nfct_query() failed\n");
		goto DONE;
	}

	rc = 0;
DONE:
	if (cth)
		nfct_close(cth);

	if (ct)
		nfct_destroy(ct);

	return rc;
}
