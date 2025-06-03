#define IPNS_TYPE_A 	  0x0100
#define IPNS_TYPE_NS	  0x0200
#define IPNS_TYPE_MD	  0x0300
#define IPNS_TYPE_MF	  0x0400
#define IPNS_TYPE_CNAME	  0x0500
#define IPNS_TYPE_SOA	  0x0600
#define IPNS_TYPE_MB	  0x0700
#define IPNS_TYPE_MG	  0x0800
#define IPNS_TYPE_MR	  0x0900
#define IPNS_TYPE_NULL	  0x0A00
#define IPNS_TYPE_WKS	  0x0B00
#define IPNS_TYPE_PTR	  0x0C00
#define IPNS_TYPE_HINFO	  0x0D00
#define IPNS_TYPE_MINFO	  0x0E00
#define IPNS_TYPE_MX	  0x0F00
#define IPNS_TYPE_TXT	  0x1000

#define IPNS_QTYPE_AXFR	      0xFC00
#define IPNS_QTYPE_MAILB      0xFD00
#define IPNS_QTYPE_MAILA      0xFE00
#define IPNS_QTYPE_ALL	      0xFF00

#define IPNS_CLASS_IN	  0x0100
#define IPNS_CLASS_CS	  0x0200
#define IPNS_CLASS_CH	  0x0300
#define IPNS_CLASS_HS	  0x0400
#define IPNS_QCLASS_ALL	  0xFF00

#define IPNS_OP_STDQ	  0
#define IPNS_OP_INVQ	  1
#define IPNS_OP_STATUS	  2

#define IPNS_RC_SUCCESS	  0
#define IPNS_RC_FMTERR	  1
#define IPNS_RC_SRVFAIL	  2
#define IPNS_RC_NAMERR	  3
#define IPNS_RC_NOTIMP	  4
#define IPNS_RC_REFUSE	  5
