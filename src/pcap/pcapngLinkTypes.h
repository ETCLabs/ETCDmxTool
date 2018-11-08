//https://www.winpcap.org/ntar/draft/PCAP-DumpFileFormat.html#appendixLinkTypes

namespace pcapng_linktype
{
    enum t_LinkType : quint16
	{
		LINKTYPE_NULL = 0,	// No link layer information. A packet saved with this link layer contains a raw L3 packet preceded by a 32-bit host-byte-order AF_ value indicating the specific L3 type.
		LINKTYPE_ETHERNET = 1,	// D/I/X and 802.3 Ethernet
		LINKTYPE_EXP_ETHERNET =	2,	// Experimental Ethernet (3Mb)
		LINKTYPE_AX25 = 3,	// Amateur Radio AX.25
		LINKTYPE_PRONET	= 4,	// Proteon ProNET Token Ring
		LINKTYPE_CHAOS = 5,	// Chaos
		LINKTYPE_TOKEN_RING	= 6,	// IEEE 802 Networks
		LINKTYPE_ARCNET = 7,	// ARCNET, with BSD-style header
		LINKTYPE_SLIP = 8,	// Serial Line IP
		LINKTYPE_PPP = 9,	// Point-to-point Protocol
		LINKTYPE_FDDI = 10,	// FDDI
		LINKTYPE_PPP_HDLC = 50,	// PPP in HDLC-like framing
		LINKTYPE_PPP_ETHER = 51,	// NetBSD PPP-over-Ethernet
		LINKTYPE_SYMANTEC_FIREWALL = 99,	// Symantec Enterprise Firewall
		LINKTYPE_ATM_RFC1483 = 100,	// LLC/SNAP-encapsulated ATM
		LINKTYPE_RAW = 101,	// Raw IP
		LINKTYPE_SLIP_BSDOS = 102, // BSD/OS SLIP BPF header
		LINKTYPE_PPP_BSDOS = 103, // BSD/OS PPP BPF header
		LINKTYPE_C_HDLC	= 104,	// Cisco HDLC
		LINKTYPE_IEEE802_11	= 105,	// IEEE 802.11 (wireless)
		LINKTYPE_ATM_CLIP = 106,	// Linux Classical IP over ATM
		LINKTYPE_FRELAY = 107,	// Frame Relay
		LINKTYPE_LOOP = 108,	// OpenBSD loopback
		LINKTYPE_ENC = 109,	// OpenBSD IPSEC enc
		LINKTYPE_LANE8023 = 110,	// ATM LANE + 802.3 (Reserved for future use)
		LINKTYPE_HIPPI = 111,	// NetBSD HIPPI (Reserved for future use)
		LINKTYPE_HDLC = 112,	// NetBSD HDLC framing (Reserved for future use)
		LINKTYPE_LINUX_SLL = 113,	// Linux cooked socket capture
		LINKTYPE_LTALK = 114,	// Apple LocalTalk hardware
		LINKTYPE_ECONET = 115,	// Acorn Econet
		LINKTYPE_IPFILTER = 116,	// Reserved for use with OpenBSD ipfilter
		LINKTYPE_PFLOG = 117, // OpenBSD DLT_PFLOG
		LINKTYPE_CISCO_IOS = 118, // For Cisco-internal use
		LINKTYPE_PRISM_HEADER = 119,	// 802.11+Prism II monitor mode
		LINKTYPE_AIRONET_HEADER = 120,	// FreeBSD Aironet driver stuff
		LINKTYPE_HHDLC = 121, // Reserved for Siemens HiPath HDLC
		LINKTYPE_IP_OVER_FC = 122, // RFC 2625 IP-over-Fibre Channel
		LINKTYPE_SUNATM = 123, // Solaris+SunATM
		LINKTYPE_RIO = 124, // RapidIO - Reserved as per request from Kent Dahlgren <kent@praesum.com> for private use.
		LINKTYPE_PCI_EXP = 125,	// PCI Express - Reserved as per request from Kent Dahlgren <kent@praesum.com> for private use.
		LINKTYPE_AURORA = 126, // Xilinx Aurora link layer - Reserved as per request from Kent Dahlgren <kent@praesum.com> for private use.
		LINKTYPE_IEEE802_11_RADIO = 127, // 802.11 plus BSD radio header
		LINKTYPE_TZSP = 128, // Tazmen Sniffer Protocol - Reserved for the TZSP encapsulation, as per request from Chris Waters <chris.waters@networkchemistry.com> TZSP is a generic encapsulation for any other link type, which includes a means to include meta-information with the packet, e.g. signal strength and channel for 802.11 packets.
		LINKTYPE_ARCNET_LINUX = 129, // Linux-style headers
		LINKTYPE_JUNIPER_MLPPP = 130, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_MLFR = 131, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_ES	= 132, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_GGSN = 133, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_MFR = 134, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_ATM2 = 135,	// Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_SERVICES = 136, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_JUNIPER_ATM1 = 137, // Juniper-private data link type, as per request from Hannes Gredler <hannes@juniper.net>. The corresponding DLT_s are used for passing on chassis-internal metainformation such as QOS profiles, etc..
		LINKTYPE_APPLE_IP_OVER_IEEE1394 = 138, // Apple IP-over-IEEE 1394 cooked header
		LINKTYPE_MTP2_WITH_PHDR	= 139, // ???
		LINKTYPE_MTP2 = 140, // ???
		LINKTYPE_MTP3 = 141, // ???
		LINKTYPE_SCCP = 142, // ???
		LINKTYPE_DOCSIS = 143, // DOCSIS MAC frames
		LINKTYPE_LINUX_IRDA = 144, // Linux-IrDA
		LINKTYPE_IBM_SP = 145, // Reserved for IBM SP switch and IBM Next Federation switch.
		LINKTYPE_IBM_SN = 146 // Reserved for IBM SP switch and IBM Next Federation switch.
	};

}
