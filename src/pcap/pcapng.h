#pragma once

#include <QObject>
#include <QVector>
#include <QtGlobal>
#include <algorithm> // Std::min
#include <QtEndian>

#include "pcapngLinkTypes.h"
/*
* PcapNg Generator
* https://www.winpcap.org/ntar/draft/PCAP-DumpFileFormat.html
*/

// Block Types
namespace pcapng_blocktypes 
{
    enum t_blocktypes : quint32 {
		SectionHeaderBlock = 0x0A0D0D0A, // Section Header Block
		InterfaceDescriptionBlock = 1, // Interface Description Block
		PacketBlock = 2, // Packet Block
		SimplePacketBlock = 3, // Simple Packet Block
		NameResolutionBlock = 4,	// Name Resolution Block
		InterfaceStatisticsBlock = 5, // Interface Statistics Block
		EnhancedPacketBlock = 6, // Enhanced Packet Block
	};
};

namespace pcapng_optioncodes
{
	// Option Codes
    enum t_optionCodes : quint16
	{
		opt_endofopt = 0,
		opt_comment,

		// 3.1.  Section Header Block
		shb_hardware = 2,
		shb_os,
		shb_userappl,

		// 3.2.  Interface Description Block
		if_name = 2,
		if_description,
		if_IPv4addr,
		if_IPv6addr,
		if_MACaddr,
		if_EUIaddr,
		if_speed,
		if_tsresol,
		if_tzone,
		if_filter,
		if_os,
		if_fcslen,
		if_tsoffset,

		// 3.3.  Enhanced Packet Block
		epb_flags = 2,
		epb_hash,
		epb_dropcount
	};
}

class pcapng
{
private:
	// Byte order magic number
	static const quint32 pcapng::byteOrderMagic = 0x1A2B3C4D;

	// Version
	static const quint16 majorVersion = 1;
	static const quint16 minorVersion = 0;

	/* alignto32bit - Align block to 32bit bounderies by padding out
	*
	* block - Block to to align
	* padValue - value to pad with
	*
	*/
	void alignto32bit(
		QByteArray &block,
		char padValue
	);

	/* options - Create options sub block
	* 
	* optionCode - Option codes
	* optionValue - Option values
	*/
	QByteArray options(
		pcapng_optioncodes::t_optionCodes optionCode, 
		QByteArray const &optionValue
	);

	// Interfaces
	struct t_interfaces
	{
		quint32 snaplen;
	};
	QVector<struct t_interfaces> *interfaces;

public:
	pcapng();
	~pcapng();

	/* 3.1.  Section Header Block (mandatory)
	*
	* options_shb_hardware - string containing the description of the hardware used to create this section.
	* options_shb_os - string containing the name of the operating system used to create this section.
	* options_shb_userappl - string containing the name of the application used to create this section.
	*/
	QByteArray SectionHeaderBlock(
		QString const &options_shb_hardware, 
		QString const &options_shb_os, 
		QString const &options_shb_userappl
		);

	/* 3.2.  Interface Description Block (mandatory)
	*
	* linktype - Standardized Link Layer Type code
	* snaplen - Maximum number of bytes dumped from each packet.
	* options_if_name - string containing the name of the device used to capture data.
	* options_if_description - string containing the description of the device used to capture data.
	* options_if_IPv4addr - Interface network address and netmask. This option can be repeated multiple times within the same Interface Description Block when multiple IPv4 addresses are assigned to the interface.
	* options_if_IPv6addr - Interface network address and prefix length (stored in the last byte). This option can be repeated multiple times within the same Interface Description Block when multiple IPv6 addresses are assigned to the interface.
	* options_if_MACaddr - Interface Hardware MAC address (48 bits).
	* options_if_EUIaddr - Interface Hardware EUI address (64 bits), if available.
	* options_if_speed - Interface speed (in bps).
	* options_if_tsresol - 	Resolution of timestamps. If the Most Significant Bit is equal to zero, the remaining bits indicates the resolution of the timestamp as as a negative power of 10 (e.g. 6 means microsecond resolution, timestamps are the number of microseconds since 1/1/1970). If the Most Significant Bit is equal to one, the remaining bits indicates the resolution as as negative power of 2 (e.g. 10 means 1/1024 of second). If this option is not present, a resolution of 10^-6 is assumed (i.e. timestamps have the same resolution of the standard 'libpcap' timestamps).
	* options_if_tzone - Time zone for GMT support (TODO: specify better).
	* options_if_filter - The filter (e.g. "capture only TCP traffic") used to capture traffic. The first byte of the Option Data keeps a code of the filter used (e.g. if this is a libpcap string, or BPF bytecode, and more). More details about this format will be presented in Appendix XXX (TODO). (TODO: better use different options for different fields? e.g. if_filter_pcap, if_filter_bpf, ...)
	* options_if_os - string containing the name of the operating system of the machine in which this interface is installed. This can be different from the same information that can be contained by the Section Header Block (Section 3.1) because the capture can have been done on a remote machine.
	* options_if_fcslen - An integer value that specified the length of the Frame Check Sequence (in bits) for this interface. For link layers whose FCS length can change during time, the Packet Block Flags Word can be used 
	* options_if_tsoffset - A 64 bits integer value that specifies an offset (in seconds) that must be added to the timestamp of each packet to obtain the absolute timestamp of a packet. If the option is missing, the timestamps stored in the packet must be considered absolute timestamps. The time zone of the offset can be specified with the option if_tzone. TODO: won't a if_tsoffset_low for fractional second offsets be useful for highly syncronized capture systems?
	*/
	QByteArray InterfaceDescriptionBlock(
		pcapng_linktype::t_LinkType const linktype, 
		quint32 snaplen,
		QString const &options_if_name,
		QString const &options_if_description,
		QByteArray const &options_if_IPv4addr,
		QByteArray const &options_if_IPv6addr,
		QByteArray const &options_if_MACaddr,
		quint64 const &options_if_EUIaddr,
		quint64 const &options_if_speed,
		quint8 const &options_if_tsresol,
		quint32 const &options_if_tzone,
		QString const &options_if_filter,
		QString const &options_if_os,
		quint8 const &options_if_fcslen,
		quint64 const &options_if_tsoffset
		);

	/* 3.3.  Enhanced Packet Block 
	*
	* interfaceID - it specifies the interface this packet comes from; the correct interface will be the one whose Interface Description Block (within the current Section of the file) is identified by the same number (see Section 3.2) of this field.
	* timestamp - number of seconds since 1/1/1970
	* capturedLength - number of bytes captured from the packet (i.e. the length of the Packet Data field). It will be the minimum value among the actual Packet Length and the snapshot length (defined in Figure 9). The value of this field does not include the padding bytes added at the end of the Packet Data field to align the Packet Data Field to a 32-bit boundary
	* packetLength - actual length of the packet when it was transmitted on the network. Can be different from captured len if the packet has been truncated by the capture process.
	* packetData - the data coming from the network, including link-layers headers. The length of this field can be derived from the field Block Total Length, present in the Block Header, and it is the minimum value among the SnapLen (present in the Interface Description Block) and the Packet Len (present in this header).
	* options_epb_flags - A flags word containing link-layer information. A complete specification of the allowed flags can be found in Appendix A.
	* options_epb_flags - This option contains a hash of the packet.
	* options_epb_dropcount - A 64bit integer value specifying the number of packets lost (by the interface and the operating system) between this packet and the preceding one.
	*/
    enum t_SimplePacketBlock_Flags : quint32
	{
		direction_inbound = 1,
		direction_outbound = 2,
		reception_unicast = 4,
		reception_multicast = 8,
		reception_broadcast = 12,
		reception_promiscuous = 16
	};
	QByteArray EnhancedPacketBlock(
		quint32 const interfaceID,
		quint64 const timestamp,
		quint32 const capturedLength,
		quint32 const packetLength,
		QByteArray const &packetData,
		t_SimplePacketBlock_Flags const options_epb_flags,
		QByteArray const &options_epb_hash,
		quint64 const options_epb_dropcount
		);

	/* 3.4.  Simple Packet Block
	*
	* packetLength - actual length of the packet when it was transmitted on the network. Can be different from captured len if the packet has been truncated by the capture process.
	* packetData - the data coming from the network, including link-layers headers. The length of this field can be derived from the field Block Total Length, present in the Block Header, and it is the minimum value among the SnapLen (present in the Interface Description Block) and the Packet Len (present in this header).
	*/
	QByteArray SimplePacketBlock(
		quint32 const packetLength,
		QByteArray const &packetData
		);

};
