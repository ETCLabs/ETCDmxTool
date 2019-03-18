#include "pcapng.h"

pcapng::pcapng(void)
{
	interfaces = new QVector<struct t_interfaces>;
}

pcapng::~pcapng(void)
{
	delete interfaces;
}

void pcapng::alignto32bit(
	QByteArray &block,
	char padValue
	)
{
	while ( ( (float(block.size() - 4) / 4) ) != ( ceil( (float(block.size() - 4) / 4) )) ) {
		// 32bit align
        block.push_back(padValue);
	}
}

QByteArray pcapng::options(
	pcapng_optioncodes::t_optionCodes optionCode, 
	QByteArray const &optionValue
	)
{
	/*
		 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|      Option Code              |         Option Length         |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	/                       Option Value                            /
	/           variable length, aligned to 32 bits                 /
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	/                                                               /
	/                 . . . other options . . .                     /
	/                                                               /
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|   Option Code == opt_endofopt  |  Option Length == 0          |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	QByteArray block;

	if (optionCode != pcapng_optioncodes::opt_endofopt) {
		// Option Code
		block.append(QByteArray((const char*)&optionCode,sizeof(quint16)));

		// Option Length
		quint16 _optionLength = optionValue.size();
		block.append(QByteArray((const char*)&_optionLength,sizeof(_optionLength)));

		// Option Value
		block.append(optionValue);
		alignto32bit(block, 0);
	} else {
		// opt_endofopt
		block.append(QByteArray((const char*)&optionCode,sizeof(optionCode)));
		// Option Length
		block.push_back(char(0));
		block.push_back(char(0)); 
	}

	return block;
}

QByteArray pcapng::SectionHeaderBlock(
	QString const &options_shb_hardware = QString(), 
	QString const &options_shb_os = QString(), 
	QString const &options_shb_userappl = QString()
	)
{
	/*
		0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +---------------------------------------------------------------+
	 0 |                   Block Type = 0x0A0D0D0A                     |
	   +---------------------------------------------------------------+
	 4 |                      Block Total Length                       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 8 |                      Byte-Order Magic                         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	12 |          Major Version        |         Minor Version         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	16 |                                                               |
	   |                          Section Length                       |
	   |                                                               |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	24 /                                                               /
	   /                      Options (variable)                       /
	   /                                                               /
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      Block Total Length                       |
	   +---------------------------------------------------------------+
	*/

	// Create block and insert block type
	quint32 _blocktype = pcapng_blocktypes::SectionHeaderBlock;
	QByteArray block = QByteArray((const char*)&_blocktype,sizeof(_blocktype));

	// Block Total Length
	QByteArray _blockLength;
	_blockLength.fill(0x00, 4); // Place holder
	block.append(_blockLength);

	// Byte-Order Magic
	block.append(QByteArray((const char*)&byteOrderMagic,sizeof(byteOrderMagic)));

	// Version
	block.append(QByteArray((const char*)&majorVersion,sizeof(majorVersion)));
	block.append(QByteArray((const char*)&minorVersion,sizeof(minorVersion)));

	// Section Length
	QByteArray _sectionLength;
    _sectionLength.fill((char)0xff, 8); // Not specified
	block.append(_sectionLength);

	// Options
	bool option = false;
	if (!options_shb_hardware.isEmpty()) { 
		block.append(options(pcapng_optioncodes::shb_hardware, options_shb_hardware.toUtf8()));
		option = true;
	}
	if (!options_shb_os.isEmpty()) {
		block.append(options(pcapng_optioncodes::shb_os, options_shb_os.toUtf8()));
		option = true;
	}
	if (!options_shb_userappl.isEmpty()) {
		block.append(options(pcapng_optioncodes::shb_userappl, options_shb_userappl.toUtf8()));
		option = true;
	}
	if (option) {
		// Insert option footer
		block.append(options(pcapng_optioncodes::opt_endofopt, NULL));
	}

	// Block Total Length 
	quint32 _blockSize = block.size() + 4;
	_blockLength = QByteArray((const char*)&_blockSize,sizeof(_blockSize));
	block.append(_blockLength);
	block.replace(4,4,_blockLength); // Update placeholder

	return block;
}

QByteArray pcapng::InterfaceDescriptionBlock(
	pcapng_linktype::t_LinkType const linktype, 
	quint32 snaplen,
	QString const &options_if_name = QString(),
	QString const &options_if_description = QString(),
	QByteArray const &options_if_IPv4addr = QByteArray(),
	QByteArray const &options_if_IPv6addr = QByteArray(),
	QByteArray const &options_if_MACaddr = QByteArray(),
	quint64 const &options_if_EUIaddr = quint64(),
	quint64 const &options_if_speed = quint64(),
	quint8 const &options_if_tsresol = quint8(),
	quint32 const &options_if_tzone = quint32(),
	QString const &options_if_filter = QString(),
	QString const &options_if_os = QString(),
	quint8 const &options_if_fcslen = quint8(),
	quint64 const &options_if_tsoffset = quint64()
	)
{
	/*
		  0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +---------------------------------------------------------------+
	 0 |                    Block Type = 0x00000001                    |
	   +---------------------------------------------------------------+
	 4 |                      Block Total Length                       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 8 |           LinkType            |           Reserved            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	12 |                            SnapLen                            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	16 /                                                               /
	   /                      Options (variable)                       /
	   /                                                               /
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      Block Total Length                       |
	   +---------------------------------------------------------------+
	*/

	// Add to interface vector
	interfaces->push_back(t_interfaces());

	// Create block and insert block type
	quint32 _blocktype = pcapng_blocktypes::InterfaceDescriptionBlock;
	QByteArray block = QByteArray((const char*)&_blocktype,sizeof(_blocktype));

	// Block Total Length
	QByteArray _blockLength;
	_blockLength.fill(0x00, 4); // Place holder
	block.append(_blockLength);

	// Link Type
	block.append(QByteArray((const char*)&linktype,sizeof(linktype)));

	// Reserved            
	block.push_back(char(0));
	block.push_back(char(0));

	// SnapLen
	block.append(QByteArray((const char*)&snaplen,sizeof(snaplen)));
	interfaces->back().snaplen = snaplen;

	// Options
	bool option = false;
	if (!options_if_name.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_name, options_if_name.toUtf8()));
		option = true;
	}
	if (!options_if_description.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_description, options_if_description.toUtf8()));
		option = true;
	}
	if (!options_if_IPv4addr.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_IPv4addr, options_if_IPv4addr));
		option = true;
	}
	if (!options_if_IPv6addr.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_IPv6addr, options_if_IPv6addr));
		option = true;
	}
	if (!options_if_MACaddr.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_MACaddr, options_if_MACaddr));
		option = true;
	}
	if (options_if_EUIaddr != NULL) {
		block.append(options(pcapng_optioncodes::if_EUIaddr, QByteArray((const char*)&options_if_EUIaddr,sizeof(options_if_EUIaddr))));
		option = true;
	}
	if (options_if_speed != NULL) {
		block.append(options(pcapng_optioncodes::if_speed, QByteArray((const char*)&options_if_speed,sizeof(options_if_speed))));
		option = true;
	}
	if (options_if_tsresol != NULL) {
		block.append(options(pcapng_optioncodes::if_tsresol, QByteArray((const char*)&options_if_tsresol,sizeof(options_if_tsresol))));
		option = true;
	}
	if (options_if_tzone != NULL) {
		block.append(options(pcapng_optioncodes::if_tzone, QByteArray((const char*)&options_if_tzone,sizeof(options_if_tzone))));
		option = true;
	}
	if (!options_if_filter.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_filter, QByteArray((const char*)&options_if_filter,sizeof(options_if_filter))));
		option = true;
	}
	if (!options_if_os.isEmpty()) {
		block.append(options(pcapng_optioncodes::if_os, QByteArray((const char*)&options_if_os,sizeof(options_if_os))));
		option = true;
	}
	if (!options_if_fcslen != NULL) {
		block.append(options(pcapng_optioncodes::if_fcslen, QByteArray((const char*)&options_if_fcslen,sizeof(options_if_fcslen))));
		option = true;
	}
	if (!options_if_tsoffset != NULL) {
		block.append(options(pcapng_optioncodes::if_tsoffset, QByteArray((const char*)&options_if_tsoffset,sizeof(options_if_tsoffset))));
		option = true;
	}
	if (option) {
		// Insert option footer
		block.append(options(pcapng_optioncodes::opt_endofopt, NULL));
	}

	// Block Total Length 
	quint32 _blockSize = block.size() + 4;
	_blockLength = QByteArray((const char*)&_blockSize,sizeof(_blockSize));
	block.append(_blockLength);
	block.replace(4,4,_blockLength); // Update placeholder

	return block;
}

QByteArray pcapng::EnhancedPacketBlock(
	quint32 const interfaceID,
	quint64 const timestamp,
	quint32 const capturedLength,
	quint32 const packetLength,
	QByteArray const &packetData,
	t_SimplePacketBlock_Flags const options_epb_flags,
	QByteArray const &options_epb_hash,
	quint64 const options_epb_dropcount
	)
{
	/*
	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +---------------------------------------------------------------+
	 0 |                    Block Type = 0x00000006                    |
	   +---------------------------------------------------------------+
	 4 |                      Block Total Length                       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 8 |                         Interface ID                          |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	12 |                        Timestamp (High)                       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	16 |                        Timestamp (Low)                        |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	20 |                         Captured Len                          |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	24 |                          Packet Len                           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	28 /                                                               /
	   /                          Packet Data                          /
	   /             variable length, aligned to 32 bits               /
	   /                                                               /
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   /                                                               /
	   /                      Options (variable)                       /
	   /                                                               /
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      Block Total Length                       |
	   +---------------------------------------------------------------+
   */
	// Create block and insert block type
	quint32 _blocktype = pcapng_blocktypes::EnhancedPacketBlock;
	QByteArray block = QByteArray((const char*)&_blocktype,sizeof(_blocktype));

	// Block Total Length
	QByteArray _blockLength;
	_blockLength.fill(0x00, 4); // Place holder
	block.append(_blockLength);

	// Interface ID
	block.append(QByteArray((const char*)&interfaceID,sizeof(interfaceID)));

	// Timestamp
	quint32 _timestamp_high = (timestamp & (0xFFFFFFFF00000000)) >> (8*4);
	quint32 _timestamp_low = timestamp & 0x00000000FFFFFFFF;
	block.append(QByteArray((const char*)&_timestamp_high,sizeof(_timestamp_high)));
	block.append(QByteArray((const char*)&_timestamp_low,sizeof(_timestamp_low)));

	// Captured Len
	block.append(QByteArray((const char*)&capturedLength,sizeof(capturedLength)));

	// Packet Len
	block.append(QByteArray((const char*)&packetLength,sizeof(packetLength)));

	// Packet Data
    block.append(QByteArray((const char*)packetData.data(), (int)std::min(interfaces->back().snaplen, packetLength))); // Limit packet to interface max);
	alignto32bit(block, 0);

	// Options
	bool option = false;
	if (options_epb_flags != NULL) {
		block.append(options(pcapng_optioncodes::epb_flags, QByteArray((const char*)&options_epb_flags,sizeof(options_epb_flags))));
		option = true;
	}
	if (!options_epb_hash.isEmpty()) {
		block.append(options(pcapng_optioncodes::epb_hash, options_epb_hash));
		option = true;
	}
	if (options_epb_dropcount != NULL) {
		block.append(options(pcapng_optioncodes::epb_dropcount, QByteArray((const char*)&options_epb_dropcount,sizeof(options_epb_dropcount))));
		option = true;
	}
	if (option) {
		// Insert option footer
		block.append(options(pcapng_optioncodes::opt_endofopt, NULL));
	}

	// Block Total Length 
	quint32 _blockSize = block.size() + 4;
	_blockLength = QByteArray((const char*)&_blockSize,sizeof(_blockSize));
	block.append(_blockLength);
	block.replace(4,4,_blockLength); // Update placeholder

	return block;
}


QByteArray pcapng::SimplePacketBlock(
	quint32 const packetLength,
	QByteArray const &packetData
	)
{
	/*
			0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +---------------------------------------------------------------+
	 0 |                    Block Type = 0x00000003                    |
	   +---------------------------------------------------------------+
	 4 |                      Block Total Length                       |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 8 |                          Packet Len                           |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	12 /                                                               /
	   /                          Packet Data                          /
	   /           variable length, aligned to 32 bits                 /
	   /                                                               /
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      Block Total Length                       |
	   +---------------------------------------------------------------+
   */
	// Create block and insert block type
	quint32 _blocktype = pcapng_blocktypes::SimplePacketBlock;
	QByteArray block = QByteArray((const char*)&_blocktype,sizeof(_blocktype));

	// Block Total Length
	QByteArray _blockLength;
	_blockLength.fill(0x00, 4); // Place holder
	block.append(_blockLength);

	// Packet Len
	block.append(QByteArray((const char*)&packetLength,sizeof(packetLength)));

	// Packet Data
    block.append(QByteArray((const char*)packetData.data(), std::min(interfaces->back().snaplen, packetLength))); // Limit packet to interface max);
	alignto32bit(block, 0);

	// Block Total Length 
	quint32 _blockSize = block.size() + 4;
	_blockLength = QByteArray((const char*)&_blockSize,sizeof(_blockSize));
	block.append(_blockLength);
	block.replace(4,4,_blockLength); // Update placeholder

	return block;
}
