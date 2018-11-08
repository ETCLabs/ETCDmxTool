
quint8 RdmDiscoveryResponseDissector::startCode() const
{
    return 0xFE;
}

QVariant RdmDiscoveryResponseDissector::protocolName(const Packet &p, int role) const
{
    Q_UNUSED(p);
    if (role == Qt::DisplayRole) return QString("RDM");
    return QVariant();
}

QVariant RdmDiscoveryResponseDissector::getSource(const Packet &p, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    // If still blank then it was unusable
    if (p.source) return formatRdmUid(p.source);
    return QString("????");
}

QVariant RdmDiscoveryResponseDissector::getDestination(const Packet &p, int role) const
{
    Q_UNUSED(p);
    if (role != Qt::DisplayRole) return QVariant();
    // Discovery Responses have no destination
    return QString();
}

QVariant RdmDiscoveryResponseDissector::getInfo(const Packet &p, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole :
        if (p.isRdmCollision) return QObject::tr("RDM DISCOVERY COLLISION");
		return QObject::tr("DISCOVERY REPLY - ONE DEVICE");
        break;
    case Qt::BackgroundRole :
        if (p.isRdmCollision) return QBrush(Qt::yellow);
        break;
    default: return QVariant();
    }
    return QVariant();
}

int RdmDiscoveryResponseDissector::preprocessPacket(Packet &p, QList<Packet> &list) const
{
    extractRdmSourceDest(p);
	list.append(p);
	return 1;
}

void RdmDiscoveryResponseDissector::extractRdmSourceDest(Packet &p)
{
    // Don't bother if we already know it's a collision
    if (p.isRdmCollision) return;

    // Assume it's a collision
    p.isRdmCollision = true;
    int index=0;
    while(p[index]==0xFE && index<p.length())
    {
        index++;
    }

    if(p.length() - index != 17) return;

    quint8 preambleSep = p[index];
    if(preambleSep != 0xAA) return;

    index++;


    // EUID
    quint8 euid[12];
    for(int pos=11; pos>=0; pos--)
    {
        euid[pos] = p[index];
        index++;
    }

    quint64 rdmId =
        quint64(euid[11] & euid[10]) << 40 |
        quint64(euid[9] & euid[8]) << 32 |
        quint64(euid[7] & euid[6]) << 24 |
        quint64(euid[5] & euid[4]) << 16 |
        quint64(euid[3] & euid[2]) << 8 |
        quint64(euid[1] & euid[0]);


    // Checksum
    quint8 encChecksum[4];
    for(int pos=3; pos>=0; pos--)
    {
        encChecksum[pos] = p[index];
        index++;
    }
    quint16 checksum =
        (quint16)(encChecksum[3] & encChecksum[2]) << 8 |
        (quint16)(encChecksum[1] & encChecksum[0]);

    quint16 localChecksum = 0;
    for (int pos=0; pos<12; pos++)
        localChecksum += (quint16)euid[pos];

    if (localChecksum == checksum)
    {
        // Valid checksum
        p.isRdmCollision = false;
        p.source = rdmId;
    }

    // Invalid checksum
}
